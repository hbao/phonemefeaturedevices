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
#include <ApUtils.h>
#include "APNManager.h"
#include "Properties.h"
#include "CommDBUtil.h"

#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif


// some useful debugging and logging macros
//#define __APN_LOGGING__
//#define __DONT_USE_ALL_IAP__
//#define __DONT_USE_WLAN__
//#define __FORCE_ASK_USER__

#ifdef __APN_LOGGING__
#define __ENABLE_LOG__
_LIT(KLogFileName,"APNManager.txt");
#endif
#include "FileLogger.h"

EXPORT_DECL CAPNManager* CAPNManager::NewL(MProperties* aProperties)
{
	CAPNManager* self = new (ELeave) CAPNManager(aProperties);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CAPNManager::CAPNManager()
{}

CAPNManager::CAPNManager(MProperties* aProperties):iProperties(aProperties)
{
	iProperties->AddRef();
}

CAPNManager::~CAPNManager()
{
	delete iNetWorkInfo;
	delete iIAPInfo;
	iDatabase.Close();
	iCurrentIAPList.Close();
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	if(iCommDBUtil)
	{
		iCommDBUtil->Release();
		iCommDBUtil = NULL;
	}
}

void CAPNManager::ConstructL()
{
	DEBUGMSG(_L("ConstructL"));
#ifdef __WINSCW__
	iBeingTested = EFalse;
#endif

	TRAPD(ignore,iRuntimeDatabase.LoadL(iProperties));
	iAPNCreationEnabled = EFalse;
	
	iCommDBUtil = CCommDBUtil::NewL();
	
	iNetWorkInfo = CNetworkInfoManager::NewL();
	iNetWorkInfo->SetObserver(this);
	iNetWorkInfo->StartL();
	iDatabase.LoadDatabaseL();
	iIAPInfo = CIAPInfo::NewL();
	iIAPInfo->ScanL();
}

void CAPNManager::LoadPropertiesL(TDes& aCountry,TDes& aNetwork)
{
	const TDesC* countryCode = &(iProperties->GetStringL(PROPERTY_STRING_COUNTRY_CODE));
	const TDesC* networkId = &(iProperties->GetStringL(PROPERTY_STRING_NETWORK_ID));
	aCountry.Copy(*countryCode);
	aNetwork.Copy(*networkId);
	DEBUGMSG2(_L("LoadPropertiesL %S %S"),&aCountry,&aNetwork);
}

MIAPSession* CAPNManager::StartIAPSession()
{
	MIAPSession* ret = NULL;
	TRAPD(err,ret = DoStartIAPSessionL());
	return ret;
}

MIAPSession* CAPNManager::DoStartIAPSessionL()
{
	TRAPD(err,BuildCurrentValidIAPsL());
	DEBUGMSG1(_L("BuildCurrentValidIAPsL err %d"),err);
	CMIAPSessionImpl* ret = CMIAPSessionImpl::NewL(this);
	DEBUGMSG1(_L("New IAP session 0x%08x"),ret);
	return ret;
}

void CAPNManager::BuildCurrentValidIAPsL()
{
	DEBUGMSG(_L("BuildCurrentValidIAPsL->"));
	iCurrentIAPList.Reset();
	RArray<TUint>& iaps = iIAPInfo->IAPs();
	CDesC16Array& wlans = iIAPInfo->WlanNames();
	// find wlans with IAPS
	TUint32 network = 0;
#if __WINSCW__
	if(!iBeingTested)
	{
		TInt iap = iCommDBUtil->FindIAPL(_L("Winsock"),network);
		if(iap != 0)
		{
			iCurrentIAPList.AppendL(TIAPWithPort(iap,0,KMaxTInt32));
		}
	}
#endif
	iWlanCount = 0;
	TInt wCount = wlans.Count();
	DEBUGMSG1(_L("WLAN count %d"),wCount);
	for(TInt i=0;i<wCount;i++)
	{
		TUint32 iapCheck  = iCommDBUtil->FindIAPL(wlans[i],network);
		DEBUGMSG3(_L("IAP %S %d net %d"),&wlans[i],iapCheck,network);
		if(iapCheck != 0) // wlan has an IAP
		{
			TInt index = iaps.Find(iapCheck);
			if(index != KErrNotFound)
			{
	#ifndef __DONT_USE_WLAN__
				DEBUGMSG1(_L("Adding wlan %d"),iapCheck);
				iCurrentIAPList.AppendL(TIAPWithPort(iapCheck,0,KWlanPriority));
	#endif
				iaps.Remove(index);
				iWlanCount++;
			}
		}
	}
	// add bluewhale iap after the wlans
	if(iAPNCreationEnabled && iNetWorkInfo->IsHomeNetwork())
	{
		DEBUGMSG(_L("Auto APN creation enabled"));
		TUint32 blueWhaleIAP = iCommDBUtil->FindIAPL(KBlueWhaleAPN,network);
		DEBUGMSG1(_L("BWM IAP %d"),blueWhaleIAP);
		if(blueWhaleIAP == 0)
		{
			blueWhaleIAP = CreateBlueWhaleIAPL();
		}
		if(blueWhaleIAP != 0)
		{
			DEBUGMSG1(_L("Adding BWM %d"),blueWhaleIAP);
			iCurrentIAPList.AppendL(TIAPWithPort(blueWhaleIAP,0,KBlueWhalePriority));
		}
	}

	// Add the known good IAPs
	const RArray<TIAPWithPort>& current(iRuntimeDatabase.WorkingSet());
	DEBUGMSG1(_L("Workingset count %d"),current.Count());
	for(TInt i = 0;i<current.Count();i++)
	{
		DEBUGMSG1(_L("Adding known %d"),current[i].iIAP);
		iCurrentIAPList.AppendL(current[i]);
	}
	
	// add the rest of the iaps
#ifndef __DONT_USE_ALL_IAP__
	TInt iapCount = iaps.Count();
	DEBUGMSG1(_L("IAP count %d"),iapCount);
	for(TInt i=0;i<iapCount;i++)
	{
		DEBUGMSG1(_L("Adding rest %d"),iaps[i]);
		iCurrentIAPList.AppendL(TIAPWithPort(iaps[i],0,KTherestPriority));
	}
#endif
	// if all else fails, ask the user
	// iap of zero prompts the user
	// only do this if there are no stored APNs
	if(current.Count() == 0)
	{
		iCurrentIAPList.AppendL(TIAPWithPort(0,0,KUserPickPriority));
	}
	DEBUGMSG(_L("BuildCurrentValidIAPsL<-"));
		
}

TInt CAPNManager::GetIAP(TInt aIndex,TInt aPort)
{
#ifndef __FORCE_ASK_USER__
	TInt iap = KErrNotFound;
	TBuf<50> iapName;
	TIAPWithPort runTime;
	TIAPWithPort fromList;
#if 0
	if(aPort != 0)
	{
		DEBUGMSG1(_L("checking runtime for %d"),aPort);
		TIAPWithPort iapFind(0,aPort,0);
		DEBUGMSG1(_L("runtime count %d"),iRuntimeDatabase.Count());
		TInt index = iRuntimeDatabase.FindByPort(iapFind);
		if(index != KErrNotFound)
		{
			runTime = iRuntimeDatabase.WorkingSet()[index];
			iap =  runTime.iIAP;
		}
		DEBUGMSG2(_L("Found %d at %d"),iap,index);
	}
#endif
	if(iNetWorkInfo->IsCallOngoing())
	{
		DEBUGMSG(_L("Ongoing call"));
		return iap;
	}
	if( ( iNetWorkInfo->IsRegisteredOnNetwork() || iWlanCount > 0  
#ifdef __WINSCW__
		||	!iBeingTested
#endif
		) && iCurrentIAPList.Count() > 0)
	{
		if(aIndex < iCurrentIAPList.Count())
		{
			fromList = iCurrentIAPList[aIndex];
			DEBUGMSG2(_L("pri %d %d"),fromList.iPriority,runTime.iPriority);
			iap = fromList.iIAP;
		}
	}
	DEBUGMSG1(_L("Trying iap %d"),iap);
	return iap;
#else
	return 0;
#endif
}

EXPORT_DECL void CAPNManager::SetAutoAPN()
{
	iAPNCreationEnabled = ETrue;
}
void CAPNManager::Reset()
{
	TRAPD(ignore,iRuntimeDatabase.ResetL(iProperties));
}
void CAPNManager::UdateBlueWhaleIAPL(const TDesC& aCountryCode,const TDesC& aNetworkId)
{
	DEBUGMSG2(_L("UdateBlueWhaleIAPL %S %S"),&aCountryCode,&aNetworkId);
			
	User::LeaveIfError(iCommDBUtil->BeginTransaction());
	TInt networkIndex = iDatabase.GetEntry(aCountryCode,aNetworkId);
	if(networkIndex != KErrNotFound)
	{
		TOperatorAPN& APNInfo = iDatabase.GetEntry(networkIndex);
		DEBUGMSG2(_L("Updating %d %S"),networkIndex,&APNInfo.iAPN);
		iCommDBUtil->UpdateOutgoingGprsL(KBlueWhaleAPN(),APNInfo.iAPN,APNInfo.iUser,APNInfo.iPasswd);
	}
	iCommDBUtil->CommitTransaction();
}

#ifndef __DONT_USE_OWNAPN__
void CAPNManager::RemoveBlueWhaleIAPL()
{
	TUint32 ret = 0;
	
	User::LeaveIfError(iCommDBUtil->BeginTransaction());
	iCommDBUtil->RemoveOutgoingGprsL(KBlueWhaleAPN());
			
	TUint32 WAPId = iCommDBUtil->RemoveWAPAccessPointL(KBlueWhaleAPN());
	iCommDBUtil->RemoveNetworkL(KBlueWhaleAPN());
	TUint32 IAPId = iCommDBUtil->RemoveInternetAcessPointL(KBlueWhaleAPN());
	iCommDBUtil->RemoveWAPBearerL(WAPId,IAPId);
	
	iCommDBUtil->CommitTransaction();
	
	iRuntimeDatabase.RemoveIAP(IAPId);
	
}
#endif

TUint32 CAPNManager::CreateBlueWhaleIAPL()
{
	DEBUGMSG(_L("CreateBlueWhaleIAPL"));
	// create the BlueWhaleAPN
	TUint32 ret = 0;
	User::LeaveIfError(iCommDBUtil->BeginTransaction());
	TUint32 service = 0;
	TUint32 bearer = 0;
#ifdef __USE_WINSOCK__
	const TPtrC bearerType(TPtrC(LAN_BEARER));
	service = iCommDBUtil->FindServiceL(TPtrC(OUTGOING_GPRS),_L("Winsock Service"));
	bearer = iCommDBUtil->FindBearerL(bearerType,_L("Winsock"));
#else
	const TPtrC bearerType(TPtrC(MODEM_BEARER));
	TInt networkIndex = KErrNotFound;
	TBuf<32> countryCode;
	TBuf<23> networkId;
	TRAPD(loadError,LoadPropertiesL(countryCode,networkId));
	if(loadError != KErrNone)
	{
		networkId.Copy(iNetWorkInfo->CurrentNetworkID());
		countryCode.Copy(iNetWorkInfo->CurrentCountryCode());
	}

	networkIndex = iDatabase.GetEntry(countryCode,networkId);
	if(networkIndex != KErrNotFound)
	{
		DEBUGMSG1(_L("Getting entry %d"),networkIndex);
		TOperatorAPN& APNInfo = iDatabase.GetEntry(networkIndex);
		service = iCommDBUtil->CreateNewOutgoingGprsL(KBlueWhaleAPN(),APNInfo.iAPN,APNInfo.iUser,APNInfo.iPasswd);
		bearer = iCommDBUtil->FindBearerL(bearerType,_L("GPRS Modem"));
	}
#endif
	if(service !=0 && bearer != 0)
	{
		TUint32 wap_id = iCommDBUtil->CreateNewWAPAccessPointL(KBlueWhaleAPN());
		TUint32 network = iCommDBUtil->CreateNewNetworkL(KBlueWhaleAPN());
		ret = iCommDBUtil->CreateNewInternetAccessPointL(KBlueWhaleAPN,service,bearer,bearerType,network);
		iCommDBUtil->CreateNewWAPBearerL(wap_id,ret);
		User::LeaveIfError(iCommDBUtil->CommitTransaction());
	}
	else
	{
		iCommDBUtil->RollbackTransaction();
	}
	return ret;
}

void CAPNManager::NetworkChanged(const TDesC& aCountryCode,const TDesC& aNetworkId)
{
	DEBUGMSG2(_L("NetworkChanged %S %S"),&aCountryCode,&aNetworkId);
	TRAPD(ignore,DoNetworkChangedL(aCountryCode,aNetworkId));
}

void CAPNManager::DoNetworkChangedL(const TDesC& aCountryCode,const TDesC& aNetworkId)
{
	TBuf<32> countryCode;
	TBuf<32> networkId;
	TRAPD(loadErr,LoadPropertiesL(countryCode,networkId));
	if(loadErr == KErrNone)
	{
		DEBUGMSG2(_L("current network %S %S"),&countryCode,&networkId);
		if(aCountryCode.Length() != 0 && aNetworkId.Length() != 0
				&& (countryCode.Compare(aCountryCode) != 0 
				|| networkId.Compare(aNetworkId) != 0))
		{
			//the network is different to the one we have setup for
			DEBUGMSG2(_L("New network %S %S"),&aCountryCode,&aNetworkId);
			SaveSettingsL(aCountryCode,aNetworkId);
			UdateBlueWhaleIAPL(aCountryCode,aNetworkId);
		}
	}
	else
	{
		if(aCountryCode.Length() != 0 && aNetworkId.Length() != 0)
		{
			SaveSettingsL(aCountryCode,aNetworkId);
		}		
	}
}

void CAPNManager::SaveSettingsL(const TDesC& aCountryCode,const TDesC& aNetworkId)
{
	iProperties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	iProperties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	iProperties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,aCountryCode,0);
	iProperties->SetStringL(PROPERTY_STRING_NETWORK_ID,aNetworkId,0);
}

void CAPNManager::IAPReport(TBool aSuccess,TInt aIAP,TInt aPort)
{
	TRAPD(ignore, IAPReportL(aSuccess, aIAP, aPort));
}

void CAPNManager::IAPReportL(TBool aSuccess,TInt aIAP,TInt aPort)
{
	DEBUGMSG3(_L("IAPReport iap %d port %d success %d"),aIAP,aPort,aSuccess);
	if(aSuccess)
	{
		if(aIAP == 0)
		{
			TBuf<50> iapName;
			iIAPInfo->ActiveIAP(iapName);
			if(iapName.Length() > 0)
			{
				if(iapName.Compare(KBlueWhaleAPN) != 0)
				{
					// user chose APN that wasn't ours
					TUint32 network;
					TInt iap = iCommDBUtil->FindIAPL(iapName,network);
					iRuntimeDatabase.AddWorkingIAP(TIAPWithPort(iap,aPort,KTherestPriority));
					iRuntimeDatabase.SaveL(iProperties);
				}
				else
				{
					TUint32 network;
					TInt iap = iCommDBUtil->FindIAPL(KBlueWhaleAPN,network);
					iRuntimeDatabase.AddWorkingIAP(TIAPWithPort(iap,aPort,KBlueWhalePriority));
					iRuntimeDatabase.SaveL(iProperties);
				}
			}
		}
		else
		{
			TInt found = iCurrentIAPList.Find(TIAPWithPort(aIAP,aPort,0));
			if(found != KErrNotFound)
			{
				TInt iap = iCurrentIAPList[found].iIAP;
				TInt port = iCurrentIAPList[found].iPort;
				TInt priority = iCurrentIAPList[found].iPriority;
				iRuntimeDatabase.AddWorkingIAP(TIAPWithPort(iap,port,priority));
				iRuntimeDatabase.SaveL(iProperties);
			}
		}
	}
	else
	{
		iRuntimeDatabase.RemoveIAP(TIAPWithPort(aIAP,aPort,0));
		iRuntimeDatabase.SaveL(iProperties);
	}
	BuildCurrentValidIAPsL();
}
////////////////////////////////////////////////////////////////////////////////////////
CMIAPSessionImpl::CMIAPSessionImpl()
{}

CMIAPSessionImpl::CMIAPSessionImpl(CAPNManager* aParent) :iParent(aParent)
{}

CMIAPSessionImpl::~CMIAPSessionImpl()
{}

CMIAPSessionImpl* CMIAPSessionImpl::NewL(CAPNManager* aParent)
{
	CMIAPSessionImpl* self = new (ELeave) CMIAPSessionImpl(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CMIAPSessionImpl::ConstructL()
{
	iIndex = 0;
}

TInt CMIAPSessionImpl::GetNextIAP(TInt aPort)
{
	return iParent->GetIAP(iIndex,aPort);
}

void CMIAPSessionImpl::ReportStatus(TBool aSuccess,TInt aIAP,TInt aPort)
{
	iParent->IAPReport(aSuccess,aIAP,aPort);
	if(!aSuccess) // IAP didn't work
	{
		iIndex++;
	}
}

void CMIAPSessionImpl::Release()
{
	delete this;
}
