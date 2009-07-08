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

#ifndef TESTAPNMANAGER_H_
#define TESTAPNMANAGER_H_
#include <cxxtest/TestSuite.h>
#include "EcomPlus.h"
#include  "IAPInfo.h"

class CTestAPNManagerConnectionMonitorWrapper : public CBase, public MConnectionMonitorWrapper
{
public:
	CTestAPNManagerConnectionMonitorWrapper();
	virtual ~CTestAPNManagerConnectionMonitorWrapper();
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
	void AddRef() {iRef++;}
	virtual void Release() 
	{
		if(--iRef <= 0)
		{
			delete this;
		}
	}
	static TInt TimeOut(TAny* aThis);
	TInt TimeOut();
	mutable TRequestStatus* iClientStatus;
	mutable CPeriodic* iTimer;
	TInt iRef;
};

class CTestAPNManagerConnectionMonitorWithIAPWrapper : public CTestAPNManagerConnectionMonitorWrapper
{
public:
	CTestAPNManagerConnectionMonitorWithIAPWrapper() {}
	virtual ~CTestAPNManagerConnectionMonitorWithIAPWrapper(){}
	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const;
	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus );
};

class CTestAPNManager : public CxxTest::TestSuite
{	
public:
	CTestAPNManager(const TDesC8& aSuiteName):CxxTest::TestSuite(aSuiteName)
	{}
private:
	void setUp();
	void tearDown();
public:
	void testCreateAPNManager();
	void testStartSession();
	void testGetIAPFromSession();
	void testGetIAPFromSessionNoProperties();
	void testLosingNetwork();
	void testNetworkChange();
	void testUserChooseAPN();
private:
	static MUnknown * APNManagerWrapperCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
	static MUnknown * APNManagerTimedWrapperCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
	static MUnknown * NetworkLossCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
	static MUnknown * APNManagerTimedWrapperWithIAPCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters);
};

#endif 
