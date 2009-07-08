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

#ifndef __TESTIAPINFO_H__
#define __TESTIAPINFO_H__

#include <cxxtest/TestSuite.h>
#include "unknown.h"
#include "IAPInfo.h"

class CIAPAvailabilityTestMonitor : public CBase, public MConnectionMonitorWrapper
{
public:
	CIAPAvailabilityTestMonitor();
	virtual ~CIAPAvailabilityTestMonitor();
protected:
	// MConnectionMonitorWrapper
	virtual void Close();
	virtual TInt ConnectL();
	virtual TInt NotifyEventL( MConnectionMonitorObserver& aObserver );
	virtual TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const;
	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const;
	virtual void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const;
	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus );
	virtual void CancelAsyncRequest(TInt aReqToCancel);
	
	// MUnknown implementation.
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

class CNetworkTestMonitor : public CBase, public MConnectionMonitorWrapper
{
public:
	CNetworkTestMonitor();
	virtual ~CNetworkTestMonitor();
protected:
	// MConnectionMonitorWrapper
	virtual void Close();
	virtual TInt ConnectL();
	virtual TInt NotifyEventL( MConnectionMonitorObserver& aObserver );
	virtual TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const;
	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const;
	virtual void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const;
	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus );
	virtual void CancelAsyncRequest(TInt aReqToCancel);
	
	// MUnknown implementation.
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

class CConnectionCountMonitor : public CBase, public MConnectionMonitorWrapper
{
public:
	CConnectionCountMonitor();
	virtual ~CConnectionCountMonitor();
protected:
	// MConnectionMonitorWrapper
	virtual void Close();
	virtual TInt ConnectL();
	virtual TInt NotifyEventL( MConnectionMonitorObserver& aObserver );
	virtual TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const;
	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const;
	virtual void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const;
	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus );
	virtual void CancelAsyncRequest(TInt aReqToCancel);
	
	// MUnknown implementation.
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

class CScanTestObserver : public CBase, public MIAPInfoObserver
{
public:
	CScanTestObserver();
protected:
	void IAPAvailabilityChange();
	void WLANAvailabilityChange();
	void ConnectionCountChange();
private:
	TInt iStep;
};

class CNetworkTestObserver : public CBase, public MIAPInfoObserver
{
public:
	CNetworkTestObserver(){}
protected:
	void IAPAvailabilityChange(){}
	void WLANAvailabilityChange()
	{
		CActiveScheduler::Stop();
	}
	void ConnectionCountChange(){}
};

class CConnectionCountTestObserver : public CBase, public MIAPInfoObserver
{
public:
	CConnectionCountTestObserver(){}
protected:
	void IAPAvailabilityChange(){}
	void WLANAvailabilityChange() {}
	void ConnectionCountChange()
	{
		CActiveScheduler::Stop();
	}
	
};

class CTestIAPInfo : public CxxTest::TestSuite
{	
public:
	CTestIAPInfo(const TDesC8& aSuiteName):CxxTest::TestSuite(aSuiteName)
	{}
private:
	void setUp();
	void tearDown();
	static MUnknown * CreateIAPAvailabilitySimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
	static MUnknown * CreateNetworkChangSimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
	static MUnknown * CreateConnectionCountSimL(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
public:
	void testCreateObject();
	void testScan();
	void testIAPAvailabilityChange();
	void testNetworkChange();
	void testConnectionCount();
};

#endif 
