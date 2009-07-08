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
#include "TestAPNManager.h"
#include "APNManager.h"
#include "DummyCommDb.h"
#include "TestAPNManagerTelephonyWrapper.h"

_LIT(KNetwork2,"33"); // Orange
_LIT(KTestWlanWithIAP,"TestWLANIAP");
_LIT(KTestWlan,"TestWLAN");

class CTestAPNManagerCommDBWrapper;

static CTestAPNManagerCommDBWrapper* gCurrentDB = NULL;
static CTestAPNManagerConnectionMonitorWithIAPWrapper* gConnectionWrapper = NULL;
void CTestAPNManager::setUp()
{
}

void CTestAPNManager::tearDown()
{
	REComPlusSession::SetDelegate(NULL);
	gCurrentDB = NULL;
	gConnectionWrapper = NULL;
}

CTestAPNManagerConnectionMonitorWrapper::CTestAPNManagerConnectionMonitorWrapper()
{}

CTestAPNManagerConnectionMonitorWrapper::~CTestAPNManagerConnectionMonitorWrapper()
{
	delete iTimer;
}

void CTestAPNManagerConnectionMonitorWrapper::Close()
{}

TInt CTestAPNManagerConnectionMonitorWrapper::ConnectL()
{
	return KErrNone;
}

TInt CTestAPNManagerConnectionMonitorWrapper::NotifyEventL( MConnectionMonitorObserver& aObserver )
{
	return KErrNone;
}

TInt CTestAPNManagerConnectionMonitorWrapper::GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const
{
	return KErrNone;
}
void CTestAPNManagerConnectionMonitorWrapper::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{
	User::Invariant();
}

void CTestAPNManagerConnectionMonitorWrapper::GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	switch(aAttribute)
	{
		case KNetworkNames:
		{
			TConnMonNetworkNames info;
			TPckg<TConnMonNetworkNames> infoPckg(info);
			info.iCount = 2;
			info.iNetwork[0].iName.Copy(KTestWlanWithIAP);
			info.iNetwork[0].iSignalStrength = 10;
			
			info.iNetwork[1].iName.Copy(KTestWlan);
			info.iNetwork[1].iSignalStrength = 10;
			
			aValue.Copy(infoPckg);
			TRequestStatus* status = &aStatus; 
			User::RequestComplete(status,KErrNone);
			break;
		}
		case KIapAvailability:
		{
			TConnMonIapInfo info;
			TPckg<TConnMonIapInfo> infoPckg(info);
			
			info.iCount = 1;
			info.iIap[0].iIapId = 1;
			aValue.Copy(infoPckg);
			User::RequestComplete(iClientStatus,KErrNone);
			iClientStatus = NULL;
			break;
		}
	}
	if(!iTimer)
	{
		iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
		TCallBack callBack(TimeOut,(TAny*)this);
		iTimer->Start(10000000,0,callBack);
	}

}
TInt CTestAPNManagerConnectionMonitorWrapper::TimeOut(TAny* aThis)
{
	CTestAPNManagerConnectionMonitorWrapper* This = static_cast<CTestAPNManagerConnectionMonitorWrapper*>(aThis);
	return This->TimeOut();
}

TInt CTestAPNManagerConnectionMonitorWrapper::TimeOut()
{
	CActiveScheduler::Stop();
	return KErrNone;
}
	
void CTestAPNManagerConnectionMonitorWrapper::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{
	aConnectionCount = 0;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status,KErrNone);
}

void CTestAPNManagerConnectionMonitorWrapper::CancelAsyncRequest(TInt aReqToCancel)
{
	if(iClientStatus)
	{
		User::RequestComplete(iClientStatus,KErrCancel);
		iClientStatus = NULL;
	}
}

void CTestAPNManagerConnectionMonitorWithIAPWrapper::GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )
{
	aConnectionCount = 1;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status,KErrNone);
}

void CTestAPNManagerConnectionMonitorWithIAPWrapper::GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
{
	switch(aAttribute)
	{
		case  KIAPName:
		{
			//aValue.Copy(_L("BlueWhale!"));
			aValue.Copy(_L("Winsock Service"));
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status,KErrNone);
			break;
		}
	}
}


class CTestAPNManagerTelephonyWrapper : public CBaseTelephonyWrapper
{
public:
	CTestAPNManagerTelephonyWrapper()
	{}
public: // MTelephonyWrapper

	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KNullDesC);
		info.iNetworkId.Copy(KNullDesC);
		aNetworkInfo.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
	}
	
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const
	{
		const_cast<CTestAPNManagerTelephonyWrapper*>(this)->iClientStatus = (TRequestStatus*)&aReqStatus;
		*iClientStatus = KRequestPending;
	}
	
	virtual TInt CancelAsync(CTelephony::TCancellationRequest aCancel) const
	{
		if(iClientStatus)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus,KErrNone);
		}
		return KErrNone;
	}
public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId )
	{
		return NULL;
	}
	void AddRef() {}
	void Release() 
	{
		delete this;
	}
};

class CTestAPNManagerChangingTelephonyWrapper : public CTestAPNManagerTelephonyWrapper
{
public:
	CTestAPNManagerChangingTelephonyWrapper(){}
	virtual ~CTestAPNManagerChangingTelephonyWrapper()
	{
		delete iTimer;
	}
	
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const
	{
		aReqStatus = KRequestPending;
		if(aEvent == CTelephony::ECurrentNetworkInfoChange)
		{
			iClientStatus = (TRequestStatus*)&aReqStatus;
			iClientBuffer = &aDes;
			if(!iTimer)
			{
				iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
				TCallBack callBack(TimeOut,(TAny*)this);
				iTimer->Start(0,500000,callBack);
			}
		}
		else
		{
			iClientStatus2 = (TRequestStatus*)&aReqStatus; 
		}
	}

	virtual TInt CancelAsync(CTelephony::TCancellationRequest aCancel) const
	{
		if(iClientStatus)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus,KErrNone);
		}
		if(iClientStatus2)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus2,KErrNone);
		}
	}

	static TInt TimeOut(TAny* aThis)
	{
		CTestAPNManagerChangingTelephonyWrapper* This = static_cast<CTestAPNManagerChangingTelephonyWrapper*>(aThis);
		return This->TimeOut();
	}
	
	virtual TInt TimeOut()
	{
		iCount++;
		delete iTimer;
		iTimer = NULL;
		if(iCount> 10)
		{
			RDebug::Print(_L("Stoping scheduler"));
			CActiveScheduler::Stop();
		}
		else
		{
			CTelephony::TNetworkInfoV1 info;
			CTelephony::TNetworkInfoV1Pckg infoPckg(info);
			
			info.iCountryCode.Copy(KCountry);
			info.iNetworkId.Copy(KNetwork);
			info.iLongName.Copy(_L("T-Mobile"));
			info.iStatus = CTelephony::ENetworkStatusCurrent;
			iClientBuffer->Copy(infoPckg);
			
			User::RequestComplete(iClientStatus,KErrNone);
		}
		return KErrNone;
	}
	mutable CPeriodic* iTimer;
	mutable TDes8* iClientBuffer;
	mutable TInt iCount;
	mutable TRequestStatus* iClientStatus2;

};

class CTestAPNManagerNetworkLossTelephonyWrapper : public CTestAPNManagerChangingTelephonyWrapper
{
public:
	virtual TInt TimeOut()
	{
		iCount++;
		delete iTimer;
		iTimer = NULL;
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		if(iCount < 10)
		{
			info.iCountryCode.Copy(KCountry);
			info.iNetworkId.Copy(KNetwork);
			info.iLongName.Copy(_L("T-Mobile"));
			info.iStatus = CTelephony::ENetworkStatusCurrent;
			iClientBuffer->Copy(infoPckg);
			User::RequestComplete(iClientStatus,KErrNone);
		}
		else if(iCount < 20)
		{
			info.iCountryCode.Copy(KNullDesC);
			info.iNetworkId.Copy(KNullDesC);
			info.iLongName.Copy(KNullDesC);
			info.iStatus = CTelephony::ENetworkStatusUnknown;
			iClientBuffer->Copy(infoPckg);
			User::RequestComplete(iClientStatus,KErrNone);
		}
		else
		{
			CActiveScheduler::Stop();
		}
		return KErrNone;
	}

};

class CTestAPNManagerCommDBWrapper : public CBase,public MCommDBWrapper
{
public:
	CTestAPNManagerCommDBWrapper()
	{}
	virtual ~CTestAPNManagerCommDBWrapper()
	{
		TInt count = iDatabase.Count();
		for(TInt i=0;i<count;i++)
		{
			iDatabase[i].Reset();
		}
		iDatabase.Reset();
	}

	void AddIAPTableL()
	{
		TTable IAPTable(TPtrC(IAP));
		CRow* service = new (ELeave) CRow;
		service->WriteTextL(TPtrC(COMMDB_NAME),_L("Winsock Service"));
		service->WriteUintL(TPtrC(COMMDB_ID),1);
		IAPTable.iData.Append(service);

		iDatabase.Append(IAPTable);
	}

	void AddNetworkTableL()
	{
		TTable bearerTable(TPtrC(NETWORK));
		iDatabase.Append(bearerTable);
	}
	
	void AddWAPAcessPointTableL()
	{
		TTable wapTable(TPtrC(WAP_ACCESS_POINT));
		iDatabase.Append(wapTable);
	}
	
	void AddWAPBearerTableL()
	{
		TTable wapTable(TPtrC(WAP_IP_BEARER));
		iDatabase.Append(wapTable);
	}
	
	void AddOutgoingGPRSTableL()
	{
		TTable wapTable(TPtrC(OUTGOING_GPRS));
		CRow* service = new (ELeave) CRow;
		service->WriteTextL(TPtrC(COMMDB_NAME),_L("Winsock Service"));
		service->WriteUintL(TPtrC(COMMDB_ID),1);
		wapTable.iData.Append(service);
		iDatabase.Append(wapTable);
	}
	
	void AddLANBearerTableL()
	{
		TTable LanbearerTable(TPtrC(LAN_BEARER));
		CRow* service = new (ELeave) CRow;
		service->WriteTextL(TPtrC(COMMDB_NAME),_L("Winsock"));
		service->WriteUintL(TPtrC(COMMDB_ID),1);
		LanbearerTable.iData.Append(service);
		iDatabase.Append(LanbearerTable);
		
		TTable modembearerTable(TPtrC(MODEM_BEARER));		
		service = new (ELeave) CRow;
		service->WriteTextL(TPtrC(COMMDB_NAME),_L("GPRS Modem"));
		service->WriteUintL(TPtrC(COMMDB_ID),2);
		modembearerTable.iData.Append(service);
		iDatabase.Append(modembearerTable);
	}
	
	void ConstructL()
	{
		AddIAPTableL();
		AddNetworkTableL();
		AddWAPAcessPointTableL();
		AddWAPBearerTableL();
		AddOutgoingGPRSTableL();
		AddLANBearerTableL();
	}
	TInt BeginTransaction()
	{
		return KErrNone;
	}
	
	TInt CommitTransaction()
	{
		return KErrNone;
	}
	
	void RollbackTransaction()
	{}
	
	static TBool CompareNames(const TTable& aLeft,const TTable& aRight)
	{
		if(aLeft.iName.Compare(aRight.iName) == 0)
		{
			return ETrue;
		}
		else
		{
			return EFalse;
		}
	}
	MCCommsDbTableViewWrapper* OpenTableL(const TDesC& aTableName)
	{
		CTestCommsDbTableViewWrapper* wrapper = NULL; 
		TIdentityRelation<TTable> relationship(CompareNames);
		const TTable lookfor(aTableName);
		TInt pos = iDatabase.Find(lookfor,relationship);
		if(pos >= 0)
		{
			wrapper = new (ELeave) CTestCommsDbTableViewWrapper(iDatabase[pos]);
		}
		else
		{
			User::Leave(pos);
		}
		return wrapper;
	}

	MUnknown * QueryInterfaceL( TInt aInterfaceId )
	{	
		if( KIID_MCommDBWrapper == aInterfaceId )
		{
			AddRef();
			return static_cast<MCommDBWrapper*>(this);
		}
	}
	void AddRef(){ iRef++;}
	void Release()
	{
		if(--iRef <= 0)
		{
			Dump();
			delete this;
		}
	}
	void Dump()
	{
		RDebug::Print(_L("***************************DATABASE*************************************"));
		TInt c = iDatabase.Count();
		for(TInt i=0;i<c;i++)
		{
			iDatabase[i].Dump();
		}
	}
	TBool Find(const TDesC& aTable,const TDesC& aField,const TDesC& aValue)
	{
		TInt c = iDatabase.Count();
		for(TInt i=0;i<c;i++)
		{
			if(iDatabase[i].iName.Compare(aTable) == 0)
			{
				return iDatabase[i].Find(aField,aValue);
			}
		}
		return EFalse;
	}
	TInt iRef;
	RArray<TTable> iDatabase;
};

MUnknown * CTestAPNManager::APNManagerTimedWrapperWithIAPCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MConnectionMonitorWrapper:
			gConnectionWrapper= new (ELeave) CTestAPNManagerConnectionMonitorWithIAPWrapper;
			ret = gConnectionWrapper;
			break;
		default:
			ret = APNManagerTimedWrapperCreate(aImplementationUid,aInterfaceUid,aConstructionParameters);
	}
	return ret;
	
}

MUnknown * CTestAPNManager::NetworkLossCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CTestAPNManagerNetworkLossTelephonyWrapper;
			break;
		default:
			ret = APNManagerTimedWrapperCreate(aImplementationUid,aInterfaceUid,aConstructionParameters);
	}	
	return ret;
}

MUnknown * CTestAPNManager::APNManagerTimedWrapperCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	RDebug::Print(_L("APNManagerWrapperCreate 0x%08x"),aImplementationUid);
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MCommDBWrapper:
			CTestAPNManagerCommDBWrapper* db = new (ELeave) CTestAPNManagerCommDBWrapper;
			db->ConstructL();
			ret = db;
			gCurrentDB = db;
			break;
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CTestAPNManagerChangingTelephonyWrapper;
			break;
		case KCID_MConnectionMonitorWrapper:
			ret = new (ELeave) CTestAPNManagerConnectionMonitorWrapper;
			break;
		default:
			User::Invariant();
	}
	return ret;
}

MUnknown * CTestAPNManager::APNManagerWrapperCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	RDebug::Print(_L("APNManagerWrapperCreate 0x%08x"),aImplementationUid);
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MCommDBWrapper:
			CTestAPNManagerCommDBWrapper* db = new (ELeave) CTestAPNManagerCommDBWrapper;
			db->ConstructL();
			ret = db;
			break;
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CTestAPNManagerTelephonyWrapper;
			break;
		case KCID_MConnectionMonitorWrapper:
			ret = new (ELeave) CTestAPNManagerConnectionMonitorWrapper;
			break;
		default:
			User::Invariant();
	}
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTestAPNManager::testCreateAPNManager()
{
	/*MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	CAPNManager* manager = CAPNManager::NewL(properties);
	TS_ASSERT(manager != NULL);
	delete manager;
	CleanupStack::PopAndDestroy(properties);
	*/
}

void CTestAPNManager::testStartSession()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);

	REComPlusSession::SetDelegate(APNManagerWrapperCreate);

	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	TS_ASSERT(session);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

void CTestAPNManager::testGetIAPFromSession()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(APNManagerTimedWrapperCreate);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(session);
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
	}

	TInt iap = session->GetNextIAP(0);
	RDebug::Print(_L("IAP %d"),iap);
	TS_ASSERT(iap > 0);
	session->ReportStatus(EFalse,iap,0);
	iap = session->GetNextIAP(0);
	RDebug::Print(_L("IAP %d"),iap);
	TS_ASSERT(iap == 0);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}
void CTestAPNManager::testGetIAPFromSessionNoProperties()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	REComPlusSession::SetDelegate(APNManagerTimedWrapperCreate);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	CActiveScheduler::Start();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	TInt iap = session->GetNextIAP(0);
	RDebug::Print(_L("IAP %d"),iap);
	TS_ASSERT(iap > 0);
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
	}
	session->ReportStatus(EFalse,iap,0);
	iap = session->GetNextIAP(0);
	RDebug::Print(_L("IAP %d"),iap);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

void CTestAPNManager::testLosingNetwork()
{
	TBool found = EFalse;
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(NetworkLossCreate);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetBeingTested();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	TInt iap = session->GetNextIAP(0);
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		found = gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
	}

	RDebug::Print(_L("IAP %d"),iap);
	TS_ASSERT(!found); // no network
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

void CTestAPNManager::testNetworkChange()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork2(),0);

	REComPlusSession::SetDelegate(NetworkLossCreate);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		TBool found = gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
		RDebug::Print(_L("Found %d"),found);
		TS_ASSERT(!found);
	}
	CleanupStack::PopAndDestroy(session);
	
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
	}

	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
	
}

void CTestAPNManager::testUserChooseAPN()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(APNManagerTimedWrapperWithIAPCreate);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
	}

	session->ReportStatus(ETrue,0,0);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}
