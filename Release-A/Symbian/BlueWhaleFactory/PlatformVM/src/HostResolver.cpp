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


CHostResolver::CHostResolver(TAny * aConstructionParameters,MSocketManager* aFactory,CThreadRunner& aThreadRunner)
	: CEComPlusRefCountedBase(aConstructionParameters),iFactory(aFactory),iHost(NULL),iThreadRunner(aThreadRunner)
{
	iState = EStart;
}

CHostResolver::~CHostResolver()
{
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	if(iConnection)
	{
		iConnection->Release();
		iConnection = NULL;
	}
	delete iHost;	
}

void CHostResolver::ConstructL()
{
	iProperties = DiL( MProperties );
	MConnectionCallback * connectionCallback = reinterpret_cast<MConnectionCallback*>(QueryInterfaceL(KIID_MConnectionCallback));
	CleanupReleasePushL(*connectionCallback);
	iProperties->SetObjectL(KPropertyObjectStateMachineCallback, connectionCallback);
	CleanupStack::PopAndDestroy(connectionCallback);

	// We've just added ourselves to a MProperties object which we own.
	// To avoid refcount dependencies that could prevent our own ref 
	// count from ever going back down to 0, lower the ref count.
	// to compensate for it getting raised in SetObjectL above.
	connectionCallback->Release(); 

	MLogger * logger = DiL(MLogger );
	CleanupReleasePushL( *logger );
	logger->SetLoggingLevel(MLogger::EError);
	iProperties->SetObjectL( KPropertyObjectLogger, logger );
	CleanupStack::PopAndDestroy(logger);

	iState = EStart;
}

void CHostResolver::ConstructConnectionL()
{
	enum {KReadBufferSize = 100};
	HBufC8 * readBuffer = HBufC8::NewLC(KReadBufferSize);
	iProperties->SetString8L(KPropertyString8ConnectionReadBuffer, readBuffer);
	CleanupStack::Pop(readBuffer); // Now owned by iProperties.

	iConnection = CiL( KCID_MSocketEngine, MWritableConnection );
	iConnection->InitializeL(iProperties);
}

TInt CHostResolver::ResolveHost(const TDesC& aHost,TUint32& aAddr)
{	
	DEBUGPRINT2(1,_L("CHostResolver::ResolveHost %d %S"),iState, &aHost);
	TInt ret = PCSL_NET_IOERROR;
 	switch(iState)
	{
	case EStart:
		{
			if(iFactory->CanResolve())
			{
				iState = EResolving;
				iHost = aHost.AllocL();
				if(iThreadRunner.DoCallback(ResolveHostCallback,this) == KErrNone)
				{
					ret = PCSL_NET_WOULDBLOCK;
				}
			}
			else
			{
				ret = PCSL_NET_WOULDBLOCK;
			}
			break;
		}
	case EResolving:
		ret = PCSL_NET_WOULDBLOCK;
		break;
	case EError:
		aAddr = 0;
		delete iHost;	
		iHost = NULL;
		break;
	case EResolved:
		{
			MResolvedConnection* resolver = QiL(iConnection,MResolvedConnection);
			CleanupReleasePushL(*resolver);
			TUint32 resolvedAddr = resolver->ResolvedAddress();
			CleanupStack::PopAndDestroy(resolver);
			if(resolvedAddr != 0)
			{
				aAddr = resolvedAddr;
				ret = PCSL_NET_SUCCESS;
				#ifdef _DEBUG_SOCKET_
					TBuf<16> address;
					address.Format(_L("%d.%d.%d.%d"),(resolvedAddr>>24)&0xff,(resolvedAddr>>16)&0xff,(resolvedAddr>>8)&0xff,resolvedAddr&0xff);
				#endif
				DEBUGPRINT2(1,_L("Resolved %S %S"),&aHost,&address);
			}
			delete iHost;	
			iHost = NULL;
			break;
		}
	default:
		aAddr = 0;
		iState = EError;
	}
	return ret;
}

void CHostResolver::Close()
{
	DEBUGPRINT1(1,_L("CHostResolver::Close 0x%08x"),this);
	iFactory->ResolverDestroyed(this);
}

MUnknown * CHostResolver::QueryInterfaceL( TInt aInterfaceId )
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

void CHostResolver::CallbackCommandL( MStateMachine::TCommand aCommand )
{
	if(aCommand == KCommandStateMachineFinishedNegotiatingConnection)
	{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt iap = -1;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		iIAPSession->ReportStatus(ETrue,iap,0);
		iState = EResolved;
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
		iFactory->HostResolved(this);
		iIAPSession->Release();
		iIAPSession = NULL;
#else
		TInt iap = 0;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		if(iap != 0)
		{
			iFactory->SetIAPForPortL(0,iap);
		}
	
		iState = EResolved;
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
		iFactory->HostResolved(this);
#endif
	}
}

void CHostResolver::ReportStateChanged( TInt /*aComponentId*/, MStateMachine::TState /*aState*/ )
{}

void CHostResolver::ReportError(TErrorType /*aErrorType*/, TInt aErrorCode)
{
	DEBUGERR1(_L("CHostResolver::ReportError %d"),aErrorCode);
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	if(aErrorCode == KErrCancel)
	{
		iState = EError;
		iFactory->HostResolved(this);
		iIAPSession->Release();
		iIAPSession = NULL;
	}
	else
	{
		TInt iap = -1;
		TRAPD(iapErr,iap = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		iIAPSession->ReportStatus(EFalse,iap,0);
		iap = iIAPSession->GetNextIAP(0);
		if(iap != KErrCancel && iap != KErrNotFound && iConnection)
		{
			iConnection->AcceptCommandL(KCommandConnectionDisconnect);
			iProperties->SetIntL(KPropertyIntSocketConnectionIap, iap);
			iConnection->AcceptCommandL(KCommandConnectionResolveHost);
		}
		else
		{
			iState = EError;
			iFactory->HostResolved(this);
			iIAPSession->Release();
			iIAPSession = NULL;
		}
	}
#else
	TInt iap = iFactory->IAPForPort(0);
	if(iap != iIAP && aErrorCode != KErrCancel && iConnection)
	{
		iIAP = iap;
		iConnection->AcceptCommandL(KCommandConnectionDisconnect);
		iProperties->SetIntL(KPropertyIntSocketConnectionIap, iIAP);
		iConnection->AcceptCommandL(KCommandConnectionResolveHost);
	}
	else
	{
		iState = EError;
		iFactory->HostResolved(this);
	}
#endif
}

void CHostResolver::DoResolveHostCallbackL()
{
	DEBUGLEAVE
	ConstructL();
	ConstructConnectionL();
	iProperties->SetStringL(KPropertyStringSocketServer,*iHost);
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	TInt iap = -1;
	iIAPSession = iFactory->StartIAPSession();
	iap = iIAPSession->GetNextIAP(0);
	if(iap != KErrNotFound)
	{
		iProperties->SetIntL(KPropertyIntSocketConnectionIap, iap);
		iConnection->AcceptCommandL(KCommandConnectionResolveHost);
	}
	else
	{
		User::Leave(KErrNotFound);
	}
#else
	iIAP = iFactory->IAPForPort(0);
	if(iIAP != -1)
	{
		iProperties->SetIntL(KPropertyIntSocketConnectionIap, iIAP);
		iConnection->AcceptCommandL(KCommandConnectionResolveHost);
	}
#endif
}

void CHostResolver::ResolveHostCallback(TAny* aThis)
{
	CHostResolver* This = static_cast<CHostResolver*>(aThis);
	TRAPD(error,This->DoResolveHostCallbackL());
	if(error != KErrNone)
	{
		This->ReportError(EErrorDisconnected, error);
	}
}

void CHostResolver::DebugResolver()
{
#ifdef __WINS__
	if(iHost)
	{
		RDebug::Print(_L("Resolver 0x%08x host %S to %d.%d.%d.%d"),this,iHost,(iResolved>>24)&0xff,(iResolved>>16)&0xff,(iResolved>>8)&0xff,iResolved&0xff);
	}
#endif
}
