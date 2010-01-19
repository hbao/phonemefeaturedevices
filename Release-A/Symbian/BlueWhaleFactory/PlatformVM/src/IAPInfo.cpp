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

#include <OSversion.h>
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#include <e32debug.h>
#endif
#include "IAPInfo.h"

#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif


//#define __IAPINFO_LOGGING__

#ifdef __IAPINFO_LOGGING__
#define __ENABLE_LOG__
_LIT(KLogFileName,"IAPInfo.txt");
#endif
#include "FileLogger.h"

MUnknown * CConnectionMonitorWrapper::NewL( TAny * aConstructionParameters )
{
	CConnectionMonitorWrapper* self = new (ELeave) CConnectionMonitorWrapper(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;

}

CConnectionMonitorWrapper::CConnectionMonitorWrapper(): CEComPlusRefCountedBase(NULL)
{}

CConnectionMonitorWrapper::~CConnectionMonitorWrapper()
{}

CConnectionMonitorWrapper::CConnectionMonitorWrapper(TAny * aConstructionParameters ) : CEComPlusRefCountedBase(aConstructionParameters)
{}

void CConnectionMonitorWrapper::ConstructL()
{}

MUnknown * CConnectionMonitorWrapper::QueryInterfaceL( TInt aInterfaceId )
{	
	if( KIID_MConnectionMonitorWrapper == aInterfaceId )
	{
		AddRef();
		return static_cast<MConnectionMonitorWrapper*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

void CConnectionMonitorWrapper::Close()
{
	iConnectionMonitor.Close();
}

TInt CConnectionMonitorWrapper::ConnectL()
{
	return iConnectionMonitor.ConnectL();
}

TInt CConnectionMonitorWrapper::NotifyEventL( MConnectionMonitorObserver& aObserver )
{
	return iConnectionMonitor.NotifyEventL(aObserver);
}

TInt CConnectionMonitorWrapper::GetConnectionInfo(const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount) const
{
	return iConnectionMonitor.GetConnectionInfo(aIndex,aConnectionId,aSubConnectionCount);
}

void CConnectionMonitorWrapper::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{
	iConnectionMonitor.GetStringAttribute(aConnectionId,aSubConnectionId,aAttribute,aValue,aStatus);
}

void CConnectionMonitorWrapper::GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const
{
	iConnectionMonitor.GetPckgAttribute(aConnectionId,aSubConnectionId,aAttribute,aValue,aStatus);
}

void CConnectionMonitorWrapper::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{
	iConnectionMonitor.GetConnectionCount(aConnectionCount,aStatus);
}

void CConnectionMonitorWrapper::CancelAsyncRequest(TInt aReqToCancel)
{
	iConnectionMonitor.CancelAsyncRequest(aReqToCancel);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_DECL CIAPInfo* CIAPInfo::NewL()
{
	CIAPInfo* self = new (ELeave) CIAPInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CIAPInfo::~CIAPInfo()
{
	Cancel();
	iIAPs.Reset();
	iWlans.Reset();
	if(iConnectionMonitor)
	{
		iConnectionMonitor->Close();
		iConnectionMonitor->Release();
		iConnectionMonitor = NULL;
	}
	iRequests.Reset();
}

CIAPInfo::CIAPInfo() : CActive(CActive::EPriorityStandard),iWlans(4)
{
	CActiveScheduler::Add(this);
}

void CIAPInfo::ConstructL()
{
	iConnectionMonitor = static_cast<MConnectionMonitorWrapper*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MConnectionMonitorWrapper), TUid::Uid(KIID_MConnectionMonitorWrapper),NULL));

	iConnectionMonitor->ConnectL();
	iConnectionMonitor->NotifyEventL(*this);
}

EXPORT_DECL void CIAPInfo::ActiveIAP(TDes& aName)
{
	if(iConnectionCount>0)
	{
		TUint subConnectionCount;
		TUint id;
		TInt ret = iConnectionMonitor->GetConnectionInfo(1,id,subConnectionCount);
		TRequestStatus status;
		iConnectionMonitor->GetStringAttribute( id, 0, KIAPName, aName, status );
		User::WaitForRequest( status );
		DEBUGMSG1(_L("Connected IAP %S"),&aName);
	}
}

EXPORT_DECL void CIAPInfo::ScanL()
{
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
	iRequests.AppendL(KIapAvailability);
#endif
	iRequests.AppendL(KNetworkNames);
	iRequests.AppendL(KBWMConnectionCount);
	if(!IsActive())
	{
		SetActive();
		// ping ourselves
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status,KErrNone);
	}
	
}
// Only to be called from RunL
void CIAPInfo::GetWlanNetworks()
{	
	iConnectionMonitor->GetPckgAttribute(EBearerIdWLAN, 0, KNetworkNames, iNetworkInfoBuf, iStatus);
	SetActive();
}

// Only to be called from RunL
void CIAPInfo::GetAvailableIAPs()
{	
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
	iConnectionMonitor->GetPckgAttribute(EBearerIdAll, 0, KIapAvailability, iIAPInfoBuf, iStatus);
	SetActive();
#endif
}

void CIAPInfo::GetConnectionCount()
{
	iConnectionMonitor->GetConnectionCount(iConnectionCount, iStatus );
	SetActive();
}

void CIAPInfo::RunL()
{
	switch(iCurrentRequest)
	{
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
		case KIapAvailability:
		{
			iIAPs.Reset();
			TConnMonIapInfo info = iIAPInfoBuf();
			TInt count = info.iCount;
			for(TUint i=0;i<count;i++)
			{
				DEBUGMSG1(_L("IAP %d"),info.iIap[i].iIapId);
				iIAPs.AppendL(info.iIap[i].iIapId);
			}
			if(iObserver)
			{
				iObserver->IAPAvailabilityChange();
			}
			break;
		}
#endif
		case KNetworkNames:
		{
			iWlans.Reset();
			TConnMonNetworkNames names = iNetworkInfoBuf();
			TUint count = names.iCount;
			for(TUint i=0;i<count  ;i++)
			{
				TBuf<32> name16;
				name16.Copy(names.iNetwork[i].iName);
				iWlans.AppendL(name16);
			}
			if(iObserver)
			{
				iObserver->WLANAvailabilityChange();
			}
			break;
		}
		case KBWMConnectionCount:
			if(iObserver)
			{
				iObserver->ConnectionCountChange();
			}

			break;
		default:
		{}
	}
	if(iRequests.Count() > 0)
	{
		iCurrentRequest = iRequests[0];
		iRequests.Remove(0);
		switch(iCurrentRequest)
		{
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
			case KIapAvailability:
				GetAvailableIAPs();
				break;
#endif
			case KNetworkNames:
				GetWlanNetworks();
				break;
			case KBWMConnectionCount:
				GetConnectionCount();
				break;
		}
			
	}
	else
	{
		iCurrentRequest = 0;
	}
}


void CIAPInfo::DoCancel()
{
	iConnectionMonitor->CancelAsyncRequest(EConnMonGetPckgAttribute);
}

TInt CIAPInfo::RunError(TInt /*aError*/)
{
	return KErrNone;
}

void CIAPInfo::EventL(const CConnMonEventBase &aConnMonEvent)
{
	TBuf<256> event;
	TUint connId = aConnMonEvent.ConnectionId();  
	switch(aConnMonEvent.EventType())
	{
		case EConnMonCreateConnection:
		{
			event.Copy(_L("EConnMonCreateConnection "));
			TBuf<128> name;
			TRequestStatus status;
			iConnectionMonitor->GetStringAttribute(connId, 0, KIAPName, name, status);
			User::WaitForRequest( status );
			event.Append(name);
			break;
		}
		case EConnMonDeleteConnection:
		{
			event.Copy(_L("EConnMonDeleteConnection"));
			TBuf<32> info;
			TInt bytes = static_cast<const CConnMonDeleteConnection*>(&aConnMonEvent)->DownlinkData();			
			info.Format(_L(" bytes %d"),bytes);
			event.Append(info);
			break;
		}
		case EConnMonCreateSubConnection:
			event.Copy(_L("EConnMonCreateSubConnection"));
			break;
		case EConnMonDeleteSubConnection :
			event.Copy(_L("EConnMonDeleteSubConnection"));
			break;
		case EConnMonDownlinkDataThreshold:
			event.Copy(_L("EConnMonDownlinkDataThreshold"));
			break;
		case EConnMonUplinkDataThreshold :
			event.Copy(_L("EConnMonUplinkDataThreshold"));
			break;
		case EConnMonNetworkStatusChange :
		{
			event.Copy(_L("EConnMonNetworkStatusChange"));
			TBuf<32> info;
			TInt status = static_cast<const CConnMonNetworkStatusChange*>(&aConnMonEvent)->NetworkStatus();			
			info.Format(_L("connId %d status %d"),connId,status);
			event.Append(info);
			break;
		}
		case EConnMonConnectionStatusChange:
		{
			event.Copy(_L("EConnMonConnectionStatusChange"));
			TBuf<12> stat;
			TInt status = static_cast<const CConnMonConnectionStatusChange*>(&aConnMonEvent)->ConnectionStatus();			
			stat.Format(_L(" Status %d"),status);
			event.Append(stat);
			break;
		}
		case EConnMonConnectionActivityChange:
			event.Copy(_L("EConnMonConnectionActivityChange"));
			break;
		case EConnMonNetworkRegistrationChange:
			event.Copy(_L("EConnMonNetworkRegistrationChange"));
			break;
		case EConnMonBearerChange:
			event.Copy(_L("EConnMonBearerChange"));
			break;
		case EConnMonSignalStrengthChange:
			event.Copy(_L("EConnMonSignalStrengthChange"));
			break;
		case EConnMonBearerAvailabilityChange:
			event.Copy(_L("EConnMonBearerAvailabilityChange"));
			break;
#if (__S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		case EConnMonIapAvailabilityChange:
		{
			event.Copy(_L("EConnMonIapAvailabilityChange"));
			TConnMonIapInfo iapInfo = static_cast<const CConnMonIapAvailabilityChange*>(&aConnMonEvent)->IapAvailability();
			iIAPs.Reset();
			for(TUint i=0;i<iapInfo.iCount;i++)
			{
				iIAPs.AppendL(iapInfo.iIap[i].iIapId);
			}
			break;
		}
#endif
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		case EConnMonTransmitPowerChange:  
			event.Copy(_L("EConnMonTransmitPowerChange"));  
			break;
#endif
		case EConnMonPluginEventBase:
			event.Copy(_L("EConnMonPluginEventBase"));
			break;
	}
	DEBUGMSG1(_L("%S"),&event);
	ScanL();
}


