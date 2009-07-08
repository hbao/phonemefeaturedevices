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

#include <Commdb.h>
#include "SocketFactory.h"
#include "Sockets.h"
#include "HostResolver.h"
#include "SocketDebug.h"
#include "midpservices.h"

CSocketFactory::CSocketFactory(MEventQueue* aQueue,MProperties* aProperties,MPropertyPersist* aPersist)
	: iQueue(aQueue),iProperties(aProperties),iPersist(aPersist)
{
	iProperties->AddRef();
}
CSocketFactory::~CSocketFactory()
{
	delete iThreadRunner;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	delete iAPNManager;
#endif
}

void CSocketFactory::ConstructL()
{
	iCurrentPort = -1;
	iCanResolve = ETrue;
}

void CSocketFactory::StartL(RThread& /*aThread*/)
{
	iThreadRunner = new (ELeave)CThreadRunner();
	iThreadRunner->ConstructL();
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	iAPNManager = CAPNManager::NewL(iProperties);
#endif
}

void CSocketFactory::StopL()
{
	TInt count = iSockets.Count();
#ifdef __WINS__
	if(count > 0)
	{
		DEBUGPRINT(1,_L("!!! sockets left open !!!"));
		DebugFactory();
	}
#endif
	while(count > 0)
	{
		CSocket* sock = iSockets[0];
		delete sock;
//		sock->Release();
		iSockets.Remove(0);
		count = iSockets.Count();
	}
	iSockets.Close();

	count = iResolvers.Count();
#ifdef __WINS__
	if(count > 0)
	{
		DEBUGPRINT(1,_L("!!! resolvers left open !!!"));
		DebugFactory();
	}
#endif
	while(count > 0)
	{
		CHostResolver* resolver = iResolvers[0];
		resolver->Release();
		iResolvers.Remove(0);
		count = iResolvers.Count();
	}
	iResolvers.Close();
	
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
}

void CSocketFactory::SetThread(MThread* aThread)
{
	iThread = aThread;
}

MSocket* CSocketFactory::CreateSocket()
{
	MSocket* ret = NULL;
	CSocket* created = new (ELeave) CSocket(NULL,this,*iThread,*iThreadRunner);
	if(created)
	{
		created->AddRef();
		DEBUGPRINT1(1,_L("Created new socket 0x%08x"),created);
		if(iSockets.Append(created) == KErrNone)
		{
			ret = created;
		}
		if(ret == NULL)
		{
			created->Release();
		}
	}
	return ret;
}

MHostResolver* CSocketFactory::CreateResolver()
{
	DEBUGPRINT(1,_L("CSocketFactory::CreateResolver"));
	MHostResolver* ret = NULL;
	CHostResolver* created = new CHostResolver(NULL,this,*iThreadRunner);
	if(created)
	{
		created->AddRef();
		DEBUGPRINT1(1,_L("Created new resolver 0x%08x"),created);
		if(iResolvers.Append(created) == KErrNone)
		{
			ret = created;
		}
		if(ret == NULL)
		{
			created->Release();
		}
	}
	return ret;
}
void CSocketFactory::DebugFactory()
{
#ifdef __WINS__
	TInt count = iSockets.Count();
	for(TInt i = 0;i<count;i++)
	{
		iSockets[i]->DebugSocket();
	}
	count = iResolvers.Count();
	for(TInt i = 0;i<count;i++)
	{
		iResolvers[i]->DebugResolver();
	}
#endif
}

void CSocketFactory::SocketDestroyed(CSocket* aSocket)
{
	TInt found = iSockets.Find(aSocket);
	if(found != KErrNotFound)
	{
		DEBUGPRINT1(1,_L("Removing socket 0x%08x"),aSocket);
		aSocket->Release();
		iSockets.Remove(found);
		DebugFactory();
	}
}
void CSocketFactory::ResolverDestroyedCallback(TAny* aThis)
{
	CSocketFactory* This = static_cast<CSocketFactory*>(aThis);
	TRAPD(ignore,This->DoResolverDestroyedCallbackL());
}

void CSocketFactory::DoResolverDestroyedCallbackL()
{
	TInt found = iResolvers.Find(iResolver);
	if(found != KErrNotFound)
	{
		DEBUGPRINT1(1,_L("Removing resolver 0x%08x"),iResolver);
		iResolver->Release();
		iResolvers.Remove(found);
		DebugFactory();
	}
}

void CSocketFactory::ResolverDestroyed(CHostResolver* aResolver)
{
	iResolver = aResolver;
	iThreadRunner->DoCallback(ResolverDestroyedCallback,this);
}

void CSocketFactory::DataReceived(MSocket* aClient)
{
	//DEBUGPRINT1(_L("CSocketFactory::DataReceived 0x%08x"),aClient);
	// one of our sockets got some data, signal the event
	TEventInfo info;
	info.iEvent = NETWORK_READ_SIGNAL;
	info.iHandle = (TInt) aClient;
	iQueue->AddEvent(info);
}

void CSocketFactory::DataSent(MSocket* aClient)
{
	//DEBUGPRINT1(_L("CSocketFactory::DataSent 0x%08x"),aClient);
	TEventInfo info;
	info.iEvent = NETWORK_WRITE_SIGNAL;
	info.iHandle = (TInt) aClient;
	iQueue->AddEvent(info);
}

void CSocketFactory::HostResolved(MHostResolver* aClient)
{
	DEBUGPRINT1(1,_L("CSocketFactory::HostResolved 0x%08x"),aClient);
	// one of our sockets got some data, signal the event
	TInt count = iResolvers.Count();
	for(TInt i=0;i<count;i++)
	{
		TEventInfo info;
		info.iEvent = HOST_NAME_LOOKUP_SIGNAL;
		info.iHandle = (TInt) static_cast<MHostResolver*>(iResolvers[i]);
		iQueue->AddEvent(info);
	}
	iCanResolve = ETrue;
}

void CSocketFactory::ResetAPNCallback(TAny* aThis)
{
	CSocketFactory* This = static_cast<CSocketFactory*>(aThis);
	TRAPD(ignore,This->DoResetAPNCallbackL());
}

void CSocketFactory::DoResetAPNCallbackL()
{
	DEBUGPRINT(1,_L("CSocketFactory::DoResetAPNCallbackL "));
	iProperties->Reset();
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	iAPNManager->Reset();
#endif
	// close all open sockets
	TInt count = iSockets.Count();
	for(TInt i = 0;i<count;i++)
	{
		iSockets[i]->CloseSocket();
	}
}

void CSocketFactory::ResetAPN()
{
	DEBUGPRINT(1,_L("CSocketFactory::ResetAPN"));
	iThreadRunner->DoCallback(ResetAPNCallback,this);
}
void CSocketFactory::SetAutoAPNCallback(TAny* aThis)
{
	CSocketFactory* This = static_cast<CSocketFactory*>(aThis);
	This->DoSetAutoAPNCallback();
}

void CSocketFactory::DoSetAutoAPNCallback()
{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	iAPNManager->SetAutoAPN();
#endif
}

void CSocketFactory::SetAutoAPN()
{
	DEBUGPRINT(1,_L("CSocketFactory::SetAutoAPN"));
	iThreadRunner->DoCallback(SetAutoAPNCallback,this);
}


TBool CSocketFactory::CheckIAPValidityL(TInt aIap)
{
	TBool ret = EFalse;
	CCommsDatabase* commDb;
    CCommsDbTableView* view;

    TUint32 iapId = 0;
    commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    view = commDb->OpenTableLC(TPtrC(IAP));

    TInt status = view->GotoFirstRecord();
    while(status == KErrNone)
    {
    	view->ReadUintL(TPtrC(COMMDB_ID), iapId);
    	if(iapId == aIap)
    	{
    		ret = ETrue;
    		break;
    	}
        status = view->GotoNextRecord();
    }

    CleanupStack::PopAndDestroy(view);
    CleanupStack::PopAndDestroy(commDb);
    return ret;
}

TBool CSocketFactory::CanResolve()
{
	if(iCanResolve)
	{
		iCanResolve = EFalse;
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
MIAPSession* CSocketFactory::StartIAPSession()
{
	return iAPNManager->StartIAPSession();
}
#else
TInt CSocketFactory::IAPForPort(TInt aPort)
{
	DEBUGPRINT1(1,_L("CSocketFactory::IAPForPort %d"),aPort);
	TIntUniqueKey iapKey = {KIID_MConnection, aPort};
	TInt iap = 0;
	TRAPD(iapError,iap = iProperties->GetIntL(iapKey));
	if(iapError != KErrNone)
	{
		// Algorithmn #1
		// store a generic APN in key zero. If that is not stored prompt user for select APN
		// can't find APN for specific port
		// try generic APN
		TIntUniqueKey genericIapKey = {KIID_MConnection, 0};
		TRAP(iapError,iap = iProperties->GetIntL(genericIapKey));
		// NOT using access point selection stuff just yet
#if 0
		/* Algorithmn #2
		 * Try all of our stored APNs
		 * When at end try all APNs stored on the device
		 */
		iap = 0;
		if(aPort != iCurrentPort)
		{
			DEBUGPRINT1(_L("CSocketFactory::IAPForPort try all for new port %d"),aPort);
			iCurrentPort = aPort;
			delete iCommDb;
			iCommDb = NULL;
			iCommDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
			delete iApSelect;
			iApSelect = NULL;
			iApSelect = CApSelect::NewLC(*iCommDb, KEApIspTypeAll ,EApBearerTypeAllBearers ,KEApSortUidAscending);
			CleanupStack::Pop(iApSelect);
			TUint32 count = iApSelect->Count();
			DEBUGPRINT1(_L("CSocketFactory::IAPForPort found %d"),count);
			
			if(iApSelect->MoveToFirst())
			{
				iap = iApSelect->Uid();
			}
		}
		else
		{
			if(iApSelect->MoveNext())
			{
				iap = iApSelect->Uid();
			}
		}
#endif		
	}
	DEBUGPRINT1(1,_L("CSocketFactory::IAPForPort IAP found %d"),iap);
	TBool valid = EFalse;
	TRAPD(err,valid = CheckIAPValidityL(iap));
	if(!valid || err != KErrNone)
	{
		iap = 0;
	}
	return iap;
}

void CSocketFactory::SetIAPForPortL(TInt aPort,TInt aIap)
{
	DEBUGPRINT2(1,_L("CSocketFactory::SetIAPForPort port=%d IAP=%d"),aPort,aIap);
	TIntUniqueKey iapKey = {KIID_MConnection, aPort};
	TInt iap = 0;
	TRAPD(iapError,iap = iProperties->GetIntL(iapKey));
	if(iapError != KErrNone)
	{
		iProperties->SetIntL(iapKey, aIap);
		iPersist->SavePropertiesL();
	}
	else if(aIap != iap)
	{
		iProperties->DeleteInt(iapKey);
		iProperties->SetIntL(iapKey, aIap);
		iPersist->SavePropertiesL();
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
