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

#ifndef __IAPINFO_H__
#define __IAPINFO_H__

#include <rconnmon.h>
#include <badesca.h>
#include <EcomPlusRefCountedBase.h>

const TInt KCID_MConnectionMonitorWrapper = 0xA0003F5A;
const TInt KIID_MConnectionMonitorWrapper = 0xA0003F5B;

class MConnectionMonitorWrapper : public MUnknown
{
public:
	virtual void Close() =0;
	virtual TInt ConnectL() =0;
	virtual TInt NotifyEventL( MConnectionMonitorObserver& aObserver ) =0;
	virtual TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const =0;
	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const =0;
	virtual void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const =0;
	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus ) =0;
	virtual void CancelAsyncRequest( TInt aReqToCancel ) =0;
protected:
	virtual ~MConnectionMonitorWrapper(){}
};

class CConnectionMonitorWrapper : public CEComPlusRefCountedBase, public MConnectionMonitorWrapper
{
public:
	static MUnknown * NewL( TAny * aConstructionParameters );

public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() {CEComPlusRefCountedBase::AddRef();}
	void Release() {CEComPlusRefCountedBase::Release();}

protected:
	// MConnectionMonitorWrapper
	void Close();
	TInt ConnectL();
	TInt NotifyEventL( MConnectionMonitorObserver& aObserver );
	TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const;
	void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const;
	void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const;
	void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus );
	void CancelAsyncRequest( TInt aReqToCancel );
private:
	CConnectionMonitorWrapper();
	CConnectionMonitorWrapper(TAny * aConstructionParameters );
	virtual ~CConnectionMonitorWrapper();
	void ConstructL();
	
protected:
	RConnectionMonitor iConnectionMonitor;
};

class MIAPInfoObserver
{
public:
	virtual void IAPAvailabilityChange() = 0;
	virtual void WLANAvailabilityChange() = 0;
	virtual void ConnectionCountChange() = 0;
};

class CIAPInfo : public CActive, public MConnectionMonitorObserver
{
public:
	static CIAPInfo* NewL();
	virtual ~CIAPInfo();
	void ScanL();
	CDesC16Array& WlanNames() { return iWlans;}
	RArray<TUint>& IAPs() {return iIAPs;}
	void ActiveIAP(TDes& aName);
	inline void SetObserver(MIAPInfoObserver* aObserver){iObserver = aObserver;}
private:
	CIAPInfo();
	void ConstructL();
	void GetWlanNetworks();
	void GetAvailableIAPs();
	void GetConnectionCount();
	// CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	// MConnectionMonitorObserver
	void EventL(const CConnMonEventBase &aConnMonEvent);
private:
	RArray<TUint> iRequests;
	TUint iCurrentRequest;
	MConnectionMonitorWrapper* iConnectionMonitor;
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
	TConnMonIapInfoBuf iIAPInfoBuf;
#endif
	TConnMonNetworkNamesBuf iNetworkInfoBuf;
	CDesC16ArrayFlat iWlans;
	RArray<TUint> iIAPs;
	TUint iConnectionCount;
	enum TIAPInfo
	{
		KBWMConnectionCount = 500
	};
	MIAPInfoObserver* iObserver;
};

#endif /*__IAPINFO_H__*/
