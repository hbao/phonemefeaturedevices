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
#include <Etel3rdParty.h>

#include "EcomPlus.h"
#include "TestAPNDatabase.h"
#include "TestNetworkInfo.h"
#include "NetworkInfo.h"
#include "TestAPNManagerTelephonyWrapper.h"

_LIT(KCountry2,"123");
_LIT(KNetwork2,"55");

_LIT(KCountry3,"456");
_LIT(KNetwork3,"88");

static CTestNetworkInfo* gTestCase = NULL;
static CNetworkInfoManager* gManager = NULL;

class CTelephonyWrapperGetCurrent : public CBaseTelephonyWrapper
{
public:
	CTelephonyWrapperGetCurrent()
	{}
public: // MTelephonyWrapper
	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KCountry2);
		info.iNetworkId.Copy(KNetwork2);
		info.iLongName.Copy(_L("Marks Test"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		aNetworkInfo.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
	}
	
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const
	{
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrCancel);
		if(gManager)
		{
			gManager->Stop();
		}
		CActiveScheduler::Stop();
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

	mutable TRequestStatus* iClientStatus;
};

#define KInterval 500 * 1000;

class CTestHomeNetworkTelephonyWrapper : public CBaseTelephonyWrapper
{
public:
	CTestHomeNetworkTelephonyWrapper() : CBaseTelephonyWrapper(){}
	virtual ~CTestHomeNetworkTelephonyWrapper()
	{}
	virtual void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const
	{
		CTelephony::TNetworkRegistrationV1 info;
		CTelephony::TNetworkRegistrationV1Pckg infoPckg(info);
		
		info.iRegStatus = CTelephony::ERegisteredOnHomeNetwork;
		aStatus.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
	}
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const
	{
		aReqStatus = KRequestPending;
		if(aEvent == CTelephony::ECurrentNetworkInfoChange)
		{
			iClientStatus = (TRequestStatus*)&aReqStatus;
		}
		else
		{
			iClientStatus2 = (TRequestStatus*)&aReqStatus;
			CActiveScheduler::Stop();
		}
	}
	virtual TInt CancelAsync(CTelephony::TCancellationRequest aCancel) const
	{
		CBaseTelephonyWrapper::CancelAsync(aCancel);
		if(iClientStatus2)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus2,KErrNone);
		}
		return KErrNone;
	}

};

class CTestChangingTelephonyWrapper : public CBaseTelephonyWrapper
{
public:
	CTestChangingTelephonyWrapper(CTestNetworkInfo* aParent) : CBaseTelephonyWrapper(),iParent(aParent){}
	virtual ~CTestChangingTelephonyWrapper()
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
		CBaseTelephonyWrapper::CancelAsync(aCancel);
		if(iClientStatus2)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus2,KErrNone);
		}
	}
	
	static TInt TimeOut(TAny* aThis)
	{
		CTestChangingTelephonyWrapper* This = static_cast<CTestChangingTelephonyWrapper*>(aThis);

		delete This->iTimer;
		This->iTimer = NULL;
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KCountry3);
		info.iNetworkId.Copy(KNetwork3);
		info.iLongName.Copy(_L("Marks Test"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		This->iClientBuffer->Copy(infoPckg);
		
		User::RequestComplete(This->iClientStatus,KErrNone);
		return KErrNone;
	}
	mutable CPeriodic* iTimer;
	mutable TDes8* iClientBuffer;
	CTestNetworkInfo* iParent;
	mutable TRequestStatus* iClientStatus2;

public:
	class CChangingObserver : public CBase, public MNetWorkInfoObserver
	{
	public:
		CChangingObserver(CTestNetworkInfo* aParent) : iParent(aParent) {}
		virtual void NetworkChanged(const TDesC& aCountryCode,const TDesC& aNetworkId)
		{
			RDebug::Print(_L("NetworkChanged %S %S"),&aCountryCode,&aNetworkId);
			if(aCountryCode.Compare(KCountry3) == 0 && aNetworkId.Compare(KNetwork3) == 0)
			{
				iParent->iPass = ETrue;
				CActiveScheduler::Stop();
			}
		}
		
	protected:	
		CTestNetworkInfo* iParent;
	};
};

MUnknown * CTestNetworkInfo::CreateHomeNetworkL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MTelephonyWrapper:
		return new CTestHomeNetworkTelephonyWrapper();
	default:
		return NULL;
	}
}

MUnknown * CTestNetworkInfo::CreateTimerL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MTelephonyWrapper:
		return new CTestChangingTelephonyWrapper(gTestCase);
	default:
		return NULL;
	}
}

MUnknown * CTestNetworkInfo::CreateL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MTelephonyWrapper:
		return new CTelephonyWrapperGetCurrent;
	default:
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTestNetworkInfo::setUp()
{
	gTestCase = this;
}

void CTestNetworkInfo::tearDown()
{
	gTestCase = NULL;
	REComPlusSession::SetDelegate(NULL);
}

void CTestNetworkInfo::testNetworkInfoCreate()
{
	REComPlusSession::SetDelegate(CTestNetworkInfo::CreateL);
	CNetworkInfoManager* info = CNetworkInfoManager::NewL();
	delete info;
}

void CTestNetworkInfo::testNetworkInfoStart()
{
	REComPlusSession::SetDelegate(CTestNetworkInfo::CreateL);
	CNetworkInfoManager* info = CNetworkInfoManager::NewL();
	CleanupStack::PushL(info);
	info->StartL();
	CleanupStack::PopAndDestroy(info);
}

void CTestNetworkInfo::testGetCurrentInfo()
{
	iTestStatus = NULL;
	iPass = EFalse;
	REComPlusSession::SetDelegate(CreateL);
	CNetworkInfoManager* info = CNetworkInfoManager::NewL();
	CleanupStack::PushL(info);
	gManager = info;
	info->StartL();
	CActiveScheduler::Start();
	TBuf<32> country = info->CurrentCountryCode();
	TBuf<32> network = info->CurrentNetworkID();
	CleanupStack::PopAndDestroy(info);
	TS_ASSERT_EQUALS_DESCRIPTOR(country,KCountry2());
	TS_ASSERT_EQUALS_DESCRIPTOR(network ,KNetwork2());
		
}

void CTestNetworkInfo::testObserver()
{
	iTestStatus = NULL;
	iPass = EFalse;
	REComPlusSession::SetDelegate(CreateTimerL);
	CNetworkInfoManager* info = CNetworkInfoManager::NewL();
	CleanupStack::PushL(info);
	CTestChangingTelephonyWrapper::CChangingObserver* observer = new (ELeave) CTestChangingTelephonyWrapper::CChangingObserver(this);
	CleanupStack::PushL(observer);
	info->SetObserver(observer);
	info->StartL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(info);
	TS_ASSERT(iPass);
}

void CTestNetworkInfo::testRegisteredNetwork()
{
	REComPlusSession::SetDelegate(CreateHomeNetworkL);
	CNetworkInfoManager* info = CNetworkInfoManager::NewL();
	CleanupStack::PushL(info);
	info->StartL();
	CActiveScheduler::Start();
	TBool ret = info->IsHomeNetwork();
	TS_ASSERT(ret);
	CleanupStack::PopAndDestroy(info);
}
