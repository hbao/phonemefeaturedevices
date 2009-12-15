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

#ifndef __TESTAPNMANAGERTELEPHONYWRAPPER_H__
#define __TESTAPNMANAGERTELEPHONYWRAPPER_H__

_LIT(KCountry,"234"); // UK 
_LIT(KNetwork,"30"); // T-Mobile

class CBaseTelephonyWrapper : public CBase, public MTelephonyWrapper
{
public:
	CBaseTelephonyWrapper()
	{}
public: // MTelephonyWrapper
	virtual void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const
	{
		//TRequestStatus* status = &aReqStatus; 
		//User::RequestComplete(status,KErrNotSupported);
		CTelephony::TNetworkRegistrationV1 info;
		CTelephony::TNetworkRegistrationV1Pckg infoPckg(info);
		
		info.iRegStatus = CTelephony::ERegisteredOnHomeNetwork;
		aStatus.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
	}		

	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KCountry);
		info.iNetworkId.Copy(KNetwork);
		info.iLongName.Copy(_L("T-Mobile"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		aNetworkInfo.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
	}
	
	virtual TInt GetLineStatus(const CTelephony::TPhoneLine & /*aLine*/, TDes8 &aStatus) const
	{
		CTelephony::TCallStatusV1 info;
		CTelephony::TCallStatusV1Pckg infoPckg(info);
		info.iStatus = CTelephony::EStatusIdle;
		aStatus.Copy(infoPckg);
		return KErrNone;
	}
	
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& /*aDes*/) const
	{
		aReqStatus = KRequestPending;
		if(aEvent == CTelephony::ECurrentNetworkInfoChange)
		{
			iClientStatus = (TRequestStatus*)&aReqStatus;
		}
		else
		{
			iClientStatus2 = (TRequestStatus*)&aReqStatus;
		}
	}
	
	virtual TInt CancelAsync(CTelephony::TCancellationRequest /*aCancel*/) const
	{
		if(iClientStatus)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus,KErrNone);
		}
		if(iClientStatus2)
		{
			User::RequestComplete((TRequestStatus*&)iClientStatus2,KErrNone);
		}
		return KErrNone;
	}
public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt /*aInterfaceId*/ )
	{
		return NULL;
	}
	void AddRef() {}
	void Release() 
	{
		delete this;
	}

	mutable TRequestStatus* iClientStatus;
	mutable TRequestStatus* iClientStatus2;
};

class CChangingTelephonyWrapper : public CBaseTelephonyWrapper
{
public:
	CChangingTelephonyWrapper()
	{}
	virtual ~CChangingTelephonyWrapper()
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
	static TInt TimeOut(TAny* aThis)
	{
		CChangingTelephonyWrapper* This = static_cast<CChangingTelephonyWrapper*>(aThis);
		return This->TimeOut();
	}
	TInt TimeOut()
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KCountry);
		info.iNetworkId.Copy(KNetwork);
		info.iLongName.Copy(_L("T-Mobile"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		iClientBuffer->Copy(infoPckg);
		User::RequestComplete(iClientStatus,KErrNone);
		return KErrNone;
	}
	
	mutable CPeriodic* iTimer;
	mutable TDes8* iClientBuffer;
};

_LIT(KUnknownCountry,"XXX");
_LIT(KUnknownNetwork,"YY");

class CUnknownTelephonyWrapper : public CBaseTelephonyWrapper
{
public:
	CUnknownTelephonyWrapper()
	{}
	virtual ~CUnknownTelephonyWrapper()
	{
		delete iTimer;
	}
	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KUnknownCountry);
		info.iNetworkId.Copy(KUnknownNetwork);
		info.iLongName.Copy(_L("Unknown"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		aNetworkInfo.Copy(infoPckg);
		TRequestStatus* status = &aReqStatus; 
		User::RequestComplete(status,KErrNone);
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
	static TInt TimeOut(TAny* aThis)
	{
		CUnknownTelephonyWrapper* This = static_cast<CUnknownTelephonyWrapper*>(aThis);
		return This->TimeOut();
	}
	TInt TimeOut()
	{
		CTelephony::TNetworkInfoV1 info;
		CTelephony::TNetworkInfoV1Pckg infoPckg(info);
		
		info.iCountryCode.Copy(KUnknownCountry);
		info.iNetworkId.Copy(KUnknownNetwork);
		info.iLongName.Copy(_L("Unknown"));
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		iClientBuffer->Copy(infoPckg);
		User::RequestComplete(iClientStatus,KErrNone);
		return KErrNone;
	}
	
	mutable CPeriodic* iTimer;
	mutable TDes8* iClientBuffer;
};


#endif
