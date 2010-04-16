/**
 * Copyright (c) 2004-2008 Blue Whale Systems Ltd. All Rights Reserved. 
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
 *  
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 
 * 2 only, as published by the Free Software Foundation.  
 *  
 * This software is provided "as is," and the copyright holder makes no representations or warranties, express or
 * implied, including but not limited to warranties of merchantability or fitness for any particular purpose or that the
 * use of this software or documentation will not infringe any third party patents, copyrights, trademarks or other
 * rights.
 * 
 * The copyright holder will not be liable for any direct, indirect special or consequential damages arising out of any
 * use of this software or documentation.
 * 
 * See the GNU  General Public License version 2 for more details 
 * (a copy is included at /legal/license.txt).  
 *  
 * You should have received a copy of the GNU General Public License 
 * version 2 along with this work; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 
 * 02110-1301 USA  
 *  
 * Please contact Blue Whale Systems Ltd, Suite 123, The LightBox
 * 111 Power Road, Chiswick, London, W4 5PY, United Kingdom or visit 
 * www.bluewhalesystems.com if you need additional 
 * information or have any questions.  
 */ 


#include <e32base.h>
#include <Commdb.h>
#include <EcomPlusRefCountedBase.h>
#include <pcsl_network.h>
#include "Application.h"
#include "Sockets.h"
#include "SocketFactory.h"
#include "HostResolver.h"
#include "WritableConnection.h"
#include "SocketEngine.h"
#include "Logger.h"
#include "DataAccess.h"
#include "DebugUtilities.h"
#include "SocketDebug.h"

#define KReadBufferSize 2048


CSocket::CSocket(TAny * aConstructionParameters,MSocketManager* aFactory,MThread& aThread,CThreadRunner& aThreadRunner) 
	: CEComPlusRefCountedBase(aConstructionParameters),iFactory(aFactory),iThread(aThread),iThreadRunner(aThreadRunner)
{
	DEBUGPRINT1(1,_L("Socket created 0x%08x"),static_cast<MSocket*>(this));
}

CSocket::~CSocket()
{
	DEBUGPRINT1(1,_L("~CSocket 0x%08x"),static_cast<MSocket*>(this));
	iMutex.Close();
	if(iConnection)
	{
		iConnection->Release();
		iConnection = NULL;
	}
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	if(iIAPSession)
	{
		iIAPSession->Release();
		iIAPSession = NULL;
	}
#endif
	delete iBuffer;
	delete iTx;
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
}

void CSocket::ConstructL()
{
	User::LeaveIfError(iMutex.CreateLocal());
	iBuffer = CBufFlat::NewL(1024);
	iProperties = DiL( MProperties );
	MConnectionCallback * connectionCallback = QiL(this,MConnectionCallback);
	CleanupReleasePushL(*connectionCallback);
	iProperties->SetObjectL(KPropertyObjectStateMachineCallback, connectionCallback);
	CleanupStack::PopAndDestroy(connectionCallback);
	
	MLogger * logger = DiL(MLogger );
	CleanupReleasePushL( *logger );
	logger->SetLoggingLevel(MLogger::EError);
	iProperties->SetObjectL( KPropertyObjectLogger, logger );
	CleanupStack::PopAndDestroy(logger);

}

void CSocket::ConstructConnectionL()
{
	HBufC8 * readBuffer = HBufC8::NewLC(KReadBufferSize);
	iProperties->SetString8L(KPropertyString8ConnectionReadBuffer, readBuffer);
	CleanupStack::Pop(readBuffer); // Now owned by iProperties.
	
	iConnection = CiL( KCID_MSocketEngine, MWritableConnection );
	iConnection->InitializeL(iProperties);
	iProperties->SetObjectL(KPropertyObjectVMWritableConnection,iConnection);
}

MUnknown * CSocket::QueryInterfaceL( TInt aInterfaceId )
{
	if(aInterfaceId == KIID_MConnectionCallback)
	{
		AddRef();
		return static_cast<MConnectionCallback*>(this);
	}
	else if(aInterfaceId == KIID_MStateMachineCallback)
	{
		AddRef();
		return static_cast<MStateMachineCallback*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

void CSocket::SignalMutex(TAny* aPtr)
{
	static_cast<RMutex*>(aPtr)->Signal();
}

void CSocket::CallbackCommandL( MStateMachine::TCommand aCommand )
{
	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);

	if(aCommand == KCommandStateMachineFinishedNegotiatingConnection)
	{
		DEBUGPRINT(1,_L("CSocket Connected"));
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt iap = -1;
		TInt port = -1;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		TRAPD(portErr,port = iProperties->GetIntL(KPropertyIntSocketPort));
		iIAPSession->ReportStatus(ETrue,iap,port);
		iIAPSession->Release();
		iIAPSession = NULL;
#else
		TInt iap = 0;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		if(iap != 0)
		{
			iFactory->SetIAPForPortL(iAddr.Port(),iap);
		}
#endif
		iConnected = EConnected;
		iFactory->DataSent(this);
	}
	else if(aCommand == KCommandStateMachineWriteCompleted)
	{
		DEBUGPRINT(2,_L("CSocket Data Sent"));
		iTxBytes += iCurrentLen;
		iFactory->DataSent(this);
		if(iTxIndex >= iTx->Des().Length())
		{
			iSending = ESent;
		}
	}
	else if(aCommand == KCommandStateMachineAcceptReadBuffer)
	{
		if(iWaitingOnRead)
		{
			iFactory->DataReceived(this);
			iWaitingOnRead = EFalse;
		}
		HBufC8* buffer = iProperties->GetStringBuffer8L(KPropertyString8ConnectionReadBuffer);
		TInt pos = iBuffer->Ptr(0).Length();
		iBuffer->InsertL(pos, buffer->Des());
		iRxBytes += buffer->Des().Length(); 
	}
	CleanupStack::PopAndDestroy();	// cleanupItem
}

void CSocket::ReportStateChanged( TInt /*aComponentId*/, MStateMachine::TState /*aState*/ )
{}

void CSocket::ReportError(TErrorType /*aErrorType*/, TInt aErrorCode)
{
	DEBUGERR2(_L("CSocket::ReportError 0x%08x error %d"),static_cast<MSocket*>(this),aErrorCode);

	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);
	
	if(iConnected == EConnecting)
	{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt iap = -1;
		TInt port = -1;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		TRAPD(portErr,port = iProperties->GetIntL(KPropertyIntSocketPort));
		iIAPSession->ReportStatus(EFalse,iap,port);
		iap = iIAPSession->GetNextIAP(port);
		if(iConnection && iap != KErrNotFound)
		{
			iConnection->AcceptCommandL(KCommandConnectionDisconnect);
			iProperties->SetIntL(KPropertyIntSocketConnectionIap, iap);
			iConnection->AcceptCommandL(KCommandConnectionConnectWithoutGoingSecure);
			CleanupStack::PopAndDestroy(); //cleanupitem
			return;
		}
#else
		TInt iap = iFactory->IAPForPort(iAddr.Port());
		if(iap != iIAP && iConnection)
		{
			iIAP = iap;
			iConnection->AcceptCommandL(KCommandConnectionDisconnect);
			iProperties->SetIntL(KPropertyIntSocketConnectionIap, iIAP);
			iConnection->AcceptCommandL(KCommandConnectionConnectWithoutGoingSecure);
			CleanupStack::PopAndDestroy(); //cleanupitem
			return;
		}
#endif
		else
		{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
			iIAPSession->Release();
			iIAPSession = NULL;
#endif
			iFactory->DataSent(this);
		}
	}
	
	if(iSending == ESending)
	{
		iSending = EReadyToSend;
		iFactory->DataSent(this);
	}
	if(iConnection)
	{
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
	}
	iConnected = EDisconnected;
	iErrorStatus = aErrorCode;
	// this should ping any waiting java threads into action
	if(iWaitingOnRead)
	{
		DEBUGPRINT1(2,_L("iFactory->DataReceived 0x%08x"),static_cast<MSocket*>(this));
		iFactory->DataReceived(this);
		iWaitingOnRead = EFalse;
	}
	CleanupStack::PopAndDestroy(); //cleanupitem
}

void CSocket::ConnectCallback(TAny* aThis)
{
	CSocket* This = static_cast<CSocket*>(aThis);
	TRAPD(error,This->DoConnectCallbackL());
	if(error != KErrNone)
	{
		This->ReportError(EErrorDisconnected, error);
	}
}

void CSocket::DoConnectCallbackL()
{
	ConstructL();
	ConstructConnectionL();
	iConnected = EConnecting;
	iRxBytes = 0;
	iTxBytes = 0;
	TBuf<16> address;
	TUint32 add = iAddr.Address();
	address.Format(_L("%d.%d.%d.%d"),(add>>24)&0xff,(add>>16)&0xff,(add>>8)&0xff,add&0xff);
	DEBUGLEAVE
	iProperties->SetStringL(KPropertyStringSocketServer,address);
	iProperties->SetIntL(KPropertyIntSocketPort, iAddr.Port());
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	TInt iap = -1;
	iIAPSession = iFactory->StartIAPSession();
	iap = iIAPSession->GetNextIAP(iAddr.Port());
	if(iap != KErrNotFound)
	{
		iProperties->SetIntL(KPropertyIntSocketConnectionIap, iap);
		iConnection->AcceptCommandL(KCommandConnectionConnectWithoutGoingSecure);
	}
	else
	{
		User::Leave(KErrNotFound);
	}
#else
	iIAP = iFactory->IAPForPort(iAddr.Port());
	iProperties->SetIntL(KPropertyIntSocketConnectionIap, iIAP);
	iConnection->AcceptCommandL(KCommandConnectionConnectWithoutGoingSecure);
#endif
}

TInt CSocket::Connect(TInetAddr aAddr)
{
	DEBUGFAIL
#ifdef _DEBUG_SOCKET_
	TBuf<16> address;
	TUint32 add = aAddr.Address();
	address.Format(_L("%d.%d.%d.%d"),(add>>24)&0xff,(add>>16)&0xff,(add>>8)&0xff,add&0xff);
#endif
	DEBUGPRINT2(1,_L("CSocket::Connect 0x%08x %S"),static_cast<MSocket*>(this),&address);
	TInt ret = PCSL_NET_IOERROR;
	if(iConnected == EReadyToConnect)
	{
		iAddr = aAddr;
		iErrorStatus = 0;
		if(iThreadRunner.DoCallback(ConnectCallback,this) == KErrNone)
		{
			ret = PCSL_NET_WOULDBLOCK;
		}
	}
	else if(iConnected == EConnecting)
	{
		ret = PCSL_NET_WOULDBLOCK;
	}
	else if(iConnected == EConnected)
	{
		ret = PCSL_NET_SUCCESS;
	}
	else
	{
		if(iErrorStatus != 0)
		{
			ret = PCSL_NET_IOERROR;
		}
	}
	DEBUGPRINT2(1,_L("CSocket::Connect 0x%08x ret %d"),static_cast<MSocket*>(this),ret);
	return ret;
}

void CSocket::CloseSocket()
{
	DEBUGPRINT1(1,_L("CSocket::CloseSocket 0x%08x ret %d"),this);
	iConnected = EDisconnected;
	if(iConnection)
	{
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
		iConnection->Release();
		iConnection = NULL;
	}
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	iErrorStatus = KErrDisconnected;
}

void CSocket::DebugSocket()
{
#ifdef __WINS__
	TUint32 add = iAddr.Address();
	if(iBuffer)
	{
		RDebug::Print(_L("Socket 0x%08x connected %d %d to %d.%d.%d.%d port %d Rx %d Tx %d RX buffer len %d"),static_cast<MSocket*>(this),iConnected,iErrorStatus,(add>>24)&0xff,(add>>16)&0xff,(add>>8)&0xff,add&0xff,iAddr.Port(),iRxBytes,iTxBytes,iBuffer->Ptr(0).Length());
	}
	else
	{
		RDebug::Print(_L("Socket 0x%08x connected %d %d to %d.%d.%d.%d port %d Rx %d Tx %d"),static_cast<MSocket*>(this),iConnected,iErrorStatus,(add>>24)&0xff,(add>>16)&0xff,(add>>8)&0xff,add&0xff,iAddr.Port(),iRxBytes,iTxBytes);
	}
#endif
}

void CSocket::CloseCallback(TAny* aThis)
{
	CSocket* This = static_cast<CSocket*>(aThis);
	TRAPD(ignore,This->DoCloseCallbackL());
}

void CSocket::DoCloseCallbackL()
{
	iConnected = EDisconnected;
	if(iConnection)
	{
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
		iConnection->Release();
		iConnection = NULL;
	}
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	delete iBuffer;
	iBuffer = NULL;
	DEBUGPRINT1(1,_L("iFactory->SocketDestroyed 0x%08x"),static_cast<MSocket*>(this));
	iFactory->SocketDestroyed(this);
}

void CSocket::Close()
{
	DEBUGPRINT1(1,_L("CSocket::Close 0x%08x"),static_cast<MSocket*>(this));
	iThreadRunner.DoCallback(CloseCallback,this);
}

TInt CSocket::Available()
{
	DEBUGFAIL
	if(iErrorStatus == KErrNone)
	{
		return iBuffer->Ptr(0).Length();
	}
	else
	{
		return -1;
	}
}

TInt CSocket::Read(TPtr8 aData,TInt* aBytesRead)
{
	DEBUGFAIL
	DEBUGPRINT1(2,_L("CSocket::Read 0x%08x"),static_cast<MSocket*>(this));
	TInt ret = PCSL_NET_IOERROR;

	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);
		
	*aBytesRead = Min(iBuffer->Ptr(0).Length(), aData.MaxLength());
	
	if(iErrorStatus != KErrNone)
	{
		if(*aBytesRead == 0)
		{
			ret = PCSL_NET_IOERROR;
		}
		else
		{
			TPtr8 buffer(iBuffer->Ptr(0));
			buffer.SetLength(*aBytesRead);
			aData.Copy(buffer);
			iBuffer->Delete(0, *aBytesRead);
			iBuffer->Compress();
			ret = PCSL_NET_SUCCESS;
		}
	}
	else
	{
		if(aData.MaxLength() > 0 && iBuffer->Ptr(0).Length() == 0)
		{
			ret = PCSL_NET_WOULDBLOCK;
			iWaitingOnRead = ETrue;
		}
		else
		{
			TPtr8 buffer(iBuffer->Ptr(0));
			buffer.SetLength(*aBytesRead);
			aData.Copy(buffer);
			iBuffer->Delete(0, *aBytesRead);
			iBuffer->Compress();
			ret = PCSL_NET_SUCCESS;		
		}
	}
	
	DEBUGPRINT2(2,_L(":read 0x%08x %d"),static_cast<MSocket*>(this),ret);
	CleanupStack::PopAndDestroy(); //cleanupitem
	return ret;
}

void CSocket::SendCallback(TAny* aThis)
{
	CSocket* This = static_cast<CSocket*>(aThis);
	TRAPD(error,This->DoSendCallbackL());
	if(error != KErrNone)
	{
		This->ReportError(EErrorDisconnected, error);
	}
}

void CSocket::DoSendCallbackL()
{
	DEBUGPRINT(3,_L("CSocket::DoSendCallbackL"));
	TInt len = iTx->Des().Length() - iTxIndex;
	TInt txLen = len < KReadBufferSize ? len : KReadBufferSize;
	TPtrC8 sendPtr(iTx->Des().Mid(iTxIndex,txLen));
	iConnection->WriteL(sendPtr);
	iTxIndex += txLen;
}

TInt CSocket::Send(TPtr8 aData, TInt* aTxLen)
{
	DEBUGPRINT2(1,_L("CSocket::Send 0x%08x %d bytes"),static_cast<MSocket*>(this),aData.Length());
	DEBUGFAIL
	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);

	TInt ret = PCSL_NET_IOERROR; 
	if(iErrorStatus == 0 && iConnected == EConnected)
	{
		if(iSending == EReadyToSend)
		{
			delete iTx;
			iTx = NULL;
			iTx = aData.AllocL();
			iCurrentLen = aData.Length();
			iSending = ESending;
			iTxIndex = 0;
			*aTxLen = 0;
			if(iThreadRunner.DoCallback(SendCallback,this) == KErrNone)
			{
				ret = PCSL_NET_WOULDBLOCK;
			}
		}
		else if(iSending == ESending)
		{
			if(iThreadRunner.DoCallback(SendCallback,this) == KErrNone)
			{
				ret = PCSL_NET_WOULDBLOCK;
			}
		}
		else if(iSending == ESent)
		{
			*aTxLen = iCurrentLen;
			iSending = EReadyToSend;
			ret = PCSL_NET_SUCCESS;
		}
		else
		{
			ret = PCSL_NET_WOULDBLOCK;
		}
	}
	CleanupStack::PopAndDestroy(); //cleanupitem
	DEBUGPRINT2(3,_L("CSocket::Send 0x%08x ret %d"),static_cast<MSocket*>(this),ret);
	return ret;
}

/* 
 * Get the connected socket address.
 * return 0 if not connected
 */
TInt CSocket::GetIPv4Address()
{
	DEBUGPRINT1(1,_L("CSocket::GetIPv4Address 0x%08x"),static_cast<MSocket*>(this));
	if(iErrorStatus == 0 && iConnected == EConnected)
	{
		DEBUGPRINT1(1,_L("Connected 0x%08x"),iAddr.Address());
		return iAddr.Address();
	}
	else
	{
		DEBUGPRINT(1,_L("NOT Connected"));
		return 0;
	}
}

void CSocket::SetOptionCallback(TAny* aThis)
{
	CSocket* This = static_cast<CSocket*>(aThis);
	TRAPD(error,This->DoSetOptionCallbackL());
	if(error != KErrNone)
	{
		This->ReportError(EErrorDisconnected, error);
	}
}

void CSocket::DoSetOptionCallbackL()
{
	iConnection->SetOption(iOption,iOptVal);
}

TInt CSocket::SetOption(TUint aOption,TUint aVal)
{
	DEBUGFAIL
	iOption = aOption;
	iOptVal = aVal;
	iThreadRunner.DoCallback(SetOptionCallback,this);
	return 0 ;
}
