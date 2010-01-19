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

#ifndef __NETWORKINFO_H__
#define __NETWORKINFO_H__

#include <Etel3rdParty.h>
#include <EcomPlusRefCountedBase.h>

const TInt KCID_MTelephonyWrapper = 0xA0003F58;
const TInt KIID_MTelephonyWrapper = 0xA0003F59;

class MTelephonyWrapper : public MUnknown
{
public:
	virtual void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const = 0;
	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const = 0;
	virtual TInt GetLineStatus(const CTelephony::TPhoneLine &aLine, TDes8 &aStatus) const = 0;
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const = 0 ;
	virtual TInt CancelAsync(CTelephony::TCancellationRequest aCancel) const = 0;
protected:
	virtual ~MTelephonyWrapper(){}
};

class CTelephonyWrapper : public CEComPlusRefCountedBase, public MTelephonyWrapper
{
public:
	static MUnknown * NewL( TAny * aConstructionParameters );

public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() {CEComPlusRefCountedBase::AddRef();}
	void Release() {CEComPlusRefCountedBase::Release();}

public: // MTelephonyWrapper
	virtual void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const;
	virtual void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const;
	virtual TInt GetLineStatus(const CTelephony::TPhoneLine &aLine, TDes8 &aStatus) const;
	virtual void NotifyChange(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes) const;
	virtual TInt CancelAsync(CTelephony::TCancellationRequest aCancel) const;

private:
	CTelephonyWrapper(TAny * aConstructionParameters );
	~CTelephonyWrapper();
	void ConstructL();
private:
	CTelephony* iTelephony;
};



class MNetWorkInfoObserver
{
public:
	virtual void NetworkChanged(const TDesC& aCountryCode,const TDesC& aNetworkId) = 0;
protected:
	virtual ~MNetWorkInfoObserver(){}
};

class CCurrentNetworkInfo : public CActive
{
public:
	static CCurrentNetworkInfo* NewL(MTelephonyWrapper& aTelephony);
	virtual ~CCurrentNetworkInfo();
	void StartL();
	void Stop();
	const TDesC& CurrentNetworkID();
	const TDesC& CurrentCountryCode();
	TBool IsRegisteredOnNetwork();
	void SetObserver(MNetWorkInfoObserver* aObserver);
private:
	CCurrentNetworkInfo(MTelephonyWrapper& aTelephony);
	void ConstructL();
private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	MTelephonyWrapper& iTelephony;
	CTelephony::TNetworkInfoV1 iInfo;
	CTelephony::TNetworkInfoV1Pckg iInfoPckg;
	TBuf<CTelephony::KNetworkCountryCodeSize> 	iCountryCode;
	TBuf<CTelephony::KNetworkIdentitySize> 		iNetworkId;
	TBool iRegistered;
	TBool iValid;
	MNetWorkInfoObserver* iObserver;
};

class CRegisteredNetworkInfo : public CActive
{
public:
	static CRegisteredNetworkInfo* NewL(MTelephonyWrapper& aTelephony);
	virtual ~CRegisteredNetworkInfo();
	void StartL();
	void Stop();
	void SetObserver(MNetWorkInfoObserver* aObserver);
	TBool IsHomeNetwork();
private:
	CRegisteredNetworkInfo(MTelephonyWrapper& aTelephony);
	void ConstructL();
private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	MTelephonyWrapper& iTelephony;
	CTelephony::TNetworkRegistrationV1 iInfo;
	CTelephony::TNetworkRegistrationV1Pckg iInfoPckg;
	TBool iValid;
	MNetWorkInfoObserver* iObserver;
};

class CCallStatusInfo : public CBase
{
public:
	static CCallStatusInfo* NewL(MTelephonyWrapper& aTelephony);
	virtual ~CCallStatusInfo();
	TBool IsCallOngoing();
private: 
	CCallStatusInfo(MTelephonyWrapper& aTelephony);
	void ConstructL();

private:
	MTelephonyWrapper& iTelephony;
};

class CNetworkInfoManager : public CBase
{
public:
	static CNetworkInfoManager* NewL();
	virtual ~CNetworkInfoManager();
	void StartL();
	void Stop();
	void SetObserver(MNetWorkInfoObserver* aObserver); 
	const TDesC& CurrentNetworkID();
	const TDesC& CurrentCountryCode();
	TBool IsRegisteredOnNetwork();
	TBool IsHomeNetwork();
	TBool IsCallOngoing();
	
private:
	CNetworkInfoManager();
	void ConstructL();
private:
	MTelephonyWrapper* iTelephony;
	CCurrentNetworkInfo* iCurrent;
	CRegisteredNetworkInfo* iRegistered;
	CCallStatusInfo* iCallStatus;
	TBool iListening;
	MNetWorkInfoObserver* iObserver;
};

#endif /*__NETWORKINFO_H__*/
