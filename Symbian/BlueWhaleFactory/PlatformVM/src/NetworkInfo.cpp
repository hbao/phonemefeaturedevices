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
#include <OSVersion.h>
#include "NetworkInfo.h"

#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif

//#define __NETWORKINFO_LOGGING__

#ifdef __NETWORKINFO_LOGGING__
#define __ENABLE_LOG__
_LIT(KLogFileName,"NetworkInfo.txt");
#endif
#include "FileLogger.h"

MUnknown * CTelephonyWrapper::NewL( TAny * aConstructionParameters )
{
	CTelephonyWrapper* self = new (ELeave) CTelephonyWrapper(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;
}

CTelephonyWrapper::CTelephonyWrapper(TAny * aConstructionParameters ): CEComPlusRefCountedBase(aConstructionParameters)
{}

CTelephonyWrapper::~CTelephonyWrapper()
{
	delete iTelephony;
}

void CTelephonyWrapper::ConstructL()
{
	iTelephony = CTelephony::NewL();
}

MUnknown * CTelephonyWrapper::QueryInterfaceL( TInt aInterfaceId )
{	
	if( KIID_MTelephonyWrapper == aInterfaceId )
	{
		AddRef();
		return static_cast<MTelephonyWrapper*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}
	
void CTelephonyWrapper::GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
{
	iTelephony->GetCurrentNetworkInfo(aReqStatus,aNetworkInfo);
}
void CTelephonyWrapper::GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const
{
	iTelephony->GetNetworkRegistrationStatus(aReqStatus,aStatus);
}

void CTelephonyWrapper::NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const
{
	iTelephony->NotifyChange(aReqStatus,aEvent,aDes);
}

TInt CTelephonyWrapper::CancelAsync(CTelephony::TCancellationRequest aCancel) const
{
	return iTelephony->CancelAsync(aCancel);
}

	
//////////////////////////////////////////////////////////////////////////////
EXPORT_DECL CNetworkInfoManager* CNetworkInfoManager::NewL()
{
	CNetworkInfoManager* self = new (ELeave) CNetworkInfoManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CNetworkInfoManager::~CNetworkInfoManager()
{
	delete iRegistered;
	delete iCurrent;
	if(iTelephony)
	{
		iTelephony->Release();
		iTelephony = NULL;
	}
}
	
CNetworkInfoManager::CNetworkInfoManager() 
{}

void CNetworkInfoManager::ConstructL()
{
	iTelephony = static_cast<MTelephonyWrapper*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MTelephonyWrapper), TUid::Uid(KIID_MTelephonyWrapper),NULL));
	iCurrent = CCurrentNetworkInfo::NewL(*iTelephony);
	iRegistered = CRegisteredNetworkInfo::NewL(*iTelephony);
}

EXPORT_DECL void CNetworkInfoManager::StartL()
{	
	iCurrent->StartL();
	iRegistered->StartL();
}

EXPORT_DECL void CNetworkInfoManager::Stop()
{	
	iCurrent->Stop();
	iRegistered->Stop();
}

EXPORT_DECL const TDesC& CNetworkInfoManager::CurrentNetworkID()
{
	return iCurrent->CurrentNetworkID();
}

EXPORT_DECL const TDesC& CNetworkInfoManager::CurrentCountryCode()
{
	return iCurrent->CurrentCountryCode();
}

EXPORT_DECL TBool CNetworkInfoManager::IsRegisteredOnNetwork()
{
	return iCurrent->IsRegisteredOnNetwork();
}

EXPORT_DECL TBool CNetworkInfoManager::IsHomeNetwork()
{
	return iRegistered->IsHomeNetwork();
}

EXPORT_DECL void CNetworkInfoManager::SetObserver(MNetWorkInfoObserver* aObserver)
{
	iObserver = aObserver;
	iCurrent->SetObserver(aObserver);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
CCurrentNetworkInfo* CCurrentNetworkInfo::NewL(MTelephonyWrapper& aTelephony)
{
	CCurrentNetworkInfo* self = new (ELeave) CCurrentNetworkInfo(aTelephony);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CCurrentNetworkInfo::CCurrentNetworkInfo(MTelephonyWrapper& aTelephony) : CActive(CActive::EPriorityStandard),iTelephony(aTelephony),iInfoPckg(iInfo)
{
	CActiveScheduler::Add(this);
}

CCurrentNetworkInfo::~CCurrentNetworkInfo()
{
	if(IsActive())
	{
		Cancel();
	}
}

void CCurrentNetworkInfo::ConstructL()
{
	iCountryCode.Copy(KNullDesC);
	iNetworkId.Copy(KNullDesC);
}

void CCurrentNetworkInfo::StartL()
{	
	iTelephony.GetCurrentNetworkInfo(iStatus, iInfoPckg);
	SetActive();
}
void CCurrentNetworkInfo::Stop()
{
	if(IsActive())
	{
		Cancel();
	}
}

void CCurrentNetworkInfo::RunL()
{
	DEBUGMSG4(_L("Info: Country %S Id %S Name %S %d"), &(iInfo.iCountryCode),&(iInfo.iNetworkId),&(iInfo.iLongName),iInfo.iStatus);
	if(iCountryCode.Compare(iInfo.iCountryCode) != 0 || iNetworkId.Compare(iInfo.iNetworkId) != 0
		|| iRegistered != (iInfo.iStatus == CTelephony::ENetworkStatusCurrent) )
	{
		DEBUGMSG(_L("Info changed"));
		iCountryCode.Copy(iInfo.iCountryCode);
		iNetworkId.Copy(iInfo.iNetworkId);
		iRegistered = (iInfo.iStatus == CTelephony::ENetworkStatusCurrent);
		if(iObserver)
		{
			DEBUGMSG(_L("->NetworkChanged"));
			iObserver->NetworkChanged(iCountryCode,iNetworkId);
			DEBUGMSG(_L("<-NetworkChanged"));
		}
	}
	
	iTelephony.NotifyChange(iStatus, CTelephony::ECurrentNetworkInfoChange,iInfoPckg);
	iValid = ETrue;
	SetActive();
}

void CCurrentNetworkInfo::DoCancel()
{
	if(iValid)
	{
		iTelephony.CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
	}
	else
	{
		iTelephony.CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	}
}

TInt CCurrentNetworkInfo::RunError(TInt /*aError*/)
{
	return KErrNone;
}

const TDesC& CCurrentNetworkInfo::CurrentNetworkID()
{
	if(iValid)
	{
		return iNetworkId;
	}
	else
	{
		return KNullDesC();
	}
}
const TDesC& CCurrentNetworkInfo::CurrentCountryCode()
{
	if(iValid)
	{
		return iCountryCode;
	}
	else
	{
		return KNullDesC();
	}
}

TBool CCurrentNetworkInfo::IsRegisteredOnNetwork()
{
	return iRegistered;
}

void CCurrentNetworkInfo::SetObserver(MNetWorkInfoObserver* aObserver)
{
	iObserver = aObserver;
}

////////////////////////////////////////////////////////////////////////////////////////////////
CRegisteredNetworkInfo* CRegisteredNetworkInfo::NewL(MTelephonyWrapper& aTelephony)
{
	CRegisteredNetworkInfo* self = new (ELeave) CRegisteredNetworkInfo(aTelephony);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CRegisteredNetworkInfo::CRegisteredNetworkInfo(MTelephonyWrapper& aTelephony) : CActive(CActive::EPriorityStandard),iTelephony(aTelephony),iInfoPckg(iInfo)
{
	CActiveScheduler::Add(this);
}

CRegisteredNetworkInfo::~CRegisteredNetworkInfo()
{
	Stop();
}

void CRegisteredNetworkInfo::ConstructL()
{
}

void CRegisteredNetworkInfo::StartL()
{	
	iTelephony.GetNetworkRegistrationStatus(iStatus, iInfoPckg);
	SetActive();
}

void CRegisteredNetworkInfo::Stop()
{
	if(IsActive())
	{
		Cancel();
	}
}

TBool CRegisteredNetworkInfo::IsHomeNetwork()
{
	if(iValid)
	{
		if(iInfo.iRegStatus == CTelephony::ERegisteredOnHomeNetwork)
		{
			return ETrue;
		}
	}
	return EFalse;
}

void CRegisteredNetworkInfo::RunL()
{
	DEBUGMSG1(_L("Home network info %d"), iInfo.iRegStatus);
	iTelephony.NotifyChange(iStatus, CTelephony::ENetworkRegistrationStatusChange,iInfoPckg);
	iValid = ETrue;
	SetActive();
}

void CRegisteredNetworkInfo::DoCancel()
{
	if(!iValid)
	{
		iTelephony.CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
	}
	else
	{
		iTelephony.CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
	}
}

TInt CRegisteredNetworkInfo::RunError(TInt /*aError*/)
{
	return KErrNone;
}

void CRegisteredNetworkInfo::SetObserver(MNetWorkInfoObserver* aObserver)
{
	iObserver = aObserver;
}
