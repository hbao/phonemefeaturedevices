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

 
#include <e32std.h>

#include "TestIAPInfo.h"

void CTestIAPInfo::setUp()
{}

void CTestIAPInfo::tearDown()
{
	REComPlusSession::SetDelegate(NULL);
}

void CTestIAPInfo::testCreateObject()
{
	CIAPInfo* info = CIAPInfo::NewL();
	delete info;
	TS_ASSERT(ETrue);
}

void CTestIAPInfo::testScan()
{
	CIAPInfo* info = CIAPInfo::NewL();
	CleanupStack::PushL(info);
	CScanTestObserver* observer = new (ELeave)CScanTestObserver;
	CleanupStack::PushL(observer);
	info->SetObserver(observer);
	info->ScanL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(info);
	TS_ASSERT(ETrue);
}

MUnknown * CTestIAPInfo::CreateIAPAvailabilitySimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MConnectionMonitorWrapper:
		return new CIAPAvailabilityTestMonitor;
	default:
		return NULL;
	}
}

void CTestIAPInfo::testIAPAvailabilityChange()
{
	REComPlusSession::SetDelegate(CTestIAPInfo::CreateIAPAvailabilitySimL);
	CIAPInfo* info = CIAPInfo::NewL();
	CleanupStack::PushL(info);
	info->ScanL();
	CActiveScheduler::Start();
	RArray<TUint>& iaps(info->IAPs()); 
	TS_ASSERT(iaps.Count() == 1);
	TS_ASSERT(iaps[0] == 0xdeadbeef);
	CleanupStack::PopAndDestroy(info);
}

MUnknown * CTestIAPInfo::CreateNetworkChangSimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MConnectionMonitorWrapper:
		return new CNetworkTestMonitor;
	default:
		return NULL;
	}
}

void CTestIAPInfo::testNetworkChange()
{
	REComPlusSession::SetDelegate(CTestIAPInfo::CreateNetworkChangSimL);
	CIAPInfo* info = CIAPInfo::NewL();
	CleanupStack::PushL(info);
	CNetworkTestObserver* observer = new (ELeave)CNetworkTestObserver;
	CleanupStack::PushL(observer);
	info->SetObserver(observer);
	info->ScanL();
	CActiveScheduler::Start();
	CDesC16Array& wlans = info->WlanNames();
	TS_ASSERT(wlans.Count() == 1);
	TS_ASSERT(wlans[0].Compare(_L("Marks network")) == 0);
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(info);
}

MUnknown * CTestIAPInfo::CreateConnectionCountSimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MConnectionMonitorWrapper:
		return new CConnectionCountMonitor;
	default:
		return NULL;
	}
}

void CTestIAPInfo::testConnectionCount()
{
	REComPlusSession::SetDelegate(CTestIAPInfo::CreateConnectionCountSimL);
	CIAPInfo* info = CIAPInfo::NewL();
	CleanupStack::PushL(info);
	CConnectionCountTestObserver* observer = new (ELeave)CConnectionCountTestObserver;
	CleanupStack::PushL(observer);
	info->SetObserver(observer);
	info->ScanL();
	CActiveScheduler::Start();
	TS_ASSERT(ETrue);
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(info);
}

////////////////////////////////////////////////////////////////////////////////////////////////
CScanTestObserver::CScanTestObserver()
{}

void CScanTestObserver::IAPAvailabilityChange()
{
	iStep++;
	if(iStep == 3)
	{
		CActiveScheduler::Stop();
	}
}

void CScanTestObserver::WLANAvailabilityChange()
{
	iStep++;
	if(iStep == 3)
	{
		CActiveScheduler::Stop();
	}
}

void CScanTestObserver::ConnectionCountChange()
{
	iStep++;
	if(iStep == 3)
	{
		CActiveScheduler::Stop();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
CIAPAvailabilityTestMonitor::CIAPAvailabilityTestMonitor()
{}

CIAPAvailabilityTestMonitor::~CIAPAvailabilityTestMonitor()
{}

void CIAPAvailabilityTestMonitor::Close()
{
}

TInt CIAPAvailabilityTestMonitor::ConnectL()
{
	return KErrNone;
}

TInt CIAPAvailabilityTestMonitor::NotifyEventL( MConnectionMonitorObserver& aObserver )
{
	return KErrNone;
}

TInt CIAPAvailabilityTestMonitor::GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const
{
	return KErrNone;
}

void CIAPAvailabilityTestMonitor::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{
	User::Invariant();
}

void CIAPAvailabilityTestMonitor::GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	switch(aAttribute)
	{
		case KIapAvailability:
		{
			TConnMonIapInfo info;
			TPckg<TConnMonIapInfo> infoPckg(info);
			
			info.iCount = 1;
			info.iIap[0].iIapId = 0xdeadbeef;
			aValue.Copy(infoPckg);
			User::RequestComplete(iClientStatus,KErrNone);
			iClientStatus = NULL;
			break;
		}
		default:
			CActiveScheduler::Stop();
	}
}

void CIAPAvailabilityTestMonitor::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{
	User::Invariant();
}

void CIAPAvailabilityTestMonitor::CancelAsyncRequest(TInt aReqToCancel)
{
	if(iClientStatus)
	{
		User::RequestComplete(iClientStatus,KErrCancel);
		iClientStatus = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CNetworkTestMonitor::CNetworkTestMonitor()
{}

CNetworkTestMonitor::~CNetworkTestMonitor()
{}
void CNetworkTestMonitor::Close()
{}

TInt CNetworkTestMonitor::ConnectL()
{
	return KErrNone;
}

TInt CNetworkTestMonitor::NotifyEventL( MConnectionMonitorObserver& aObserver )
{
	return KErrNone;
}

TInt CNetworkTestMonitor::GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const
{
	return KErrNone;
}

void CNetworkTestMonitor::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{}

void CNetworkTestMonitor::GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	switch(aAttribute)
	{
		case KNetworkNames:
		{
			TConnMonNetworkNames info;
			TPckg<TConnMonNetworkNames> infoPckg(info);
			info.iCount = 1;
			info.iNetwork[0].iName.Copy(_L("Marks network"));
			info.iNetwork[0].iSignalStrength = 10;
			aValue.Copy(infoPckg);
			TRequestStatus* status = &aStatus; 
			User::RequestComplete(status,KErrNone);
			break;
		}
		default:
			TRequestStatus* status = &aStatus; 
			User::RequestComplete(status,KErrNone);
	}
}


void CNetworkTestMonitor::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{}

void CNetworkTestMonitor::CancelAsyncRequest(TInt aReqToCancel)
{
	if(iClientStatus)
	{
		User::RequestComplete(iClientStatus,KErrCancel);
		iClientStatus = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CConnectionCountMonitor::CConnectionCountMonitor()
{}

CConnectionCountMonitor::~CConnectionCountMonitor()
{}

void CConnectionCountMonitor::Close()
{}

TInt CConnectionCountMonitor::ConnectL()
{
	return KErrNone;
}

TInt CConnectionCountMonitor::NotifyEventL( MConnectionMonitorObserver& aObserver )
{
	return KErrNone;
}

TInt CConnectionCountMonitor::GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const
{
	return KErrNone;
}

void CConnectionCountMonitor::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{}

void CConnectionCountMonitor::GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	TRequestStatus* status = &aStatus; 
	User::RequestComplete(status,KErrNone);
}


void CConnectionCountMonitor::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	aConnectionCount = 1;
	TRequestStatus* status = &aStatus; 
	User::RequestComplete(status,KErrNone);
}

void CConnectionCountMonitor::CancelAsyncRequest(TInt aReqToCancel)
{
	if(iClientStatus)
	{
		User::RequestComplete(iClientStatus,KErrCancel);
		iClientStatus = NULL;
	}
}
