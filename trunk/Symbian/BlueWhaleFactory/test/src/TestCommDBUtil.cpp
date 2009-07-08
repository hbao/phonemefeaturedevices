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
#include "EcomPlus.h"
#include "TestCommDBUtil.h"
#include "CommDBUtil.h"
#include "DummyCommDb.h"

_LIT(KTestIAP,"My Test");
_LIT(KTestBearerType,"XXX");
_LIT(KTestBearer,"YYY");
_LIT(KTestIfName,"ZZZZ");
_LIT(KTestServiceType,"AAAA");
_LIT(KTestServiceName,"BBBB");
_LIT(KTestNetworkName,"MMMM");
_LIT(KBlueWhaleIAP,"BlueWhale!");
_LIT(KTestAPN,"BlueWhaleAPN");
_LIT(KTestUser,"user");
_LIT(KTestPass,"pass");

_LIT(KTestAPN2,"22222222222");
_LIT(KTestUser2,"222");
_LIT(KTestPass2,"333");

class CTestCommDBWrapper;

static CTestCommDBWrapper* gDatabase = NULL;

void CTestCommDBUtil::setUp()
{}

void CTestCommDBUtil::tearDown()
{
	REComPlusSession::SetDelegate(NULL);
}

void CTestCommDBUtil::testCreate()
{
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());		
	TS_ASSERT(commDB);
	delete commDB;
}

class CTestCommDBWrapper : public CBase,public MCommDBWrapper
{
public:
	CTestCommDBWrapper()
	{}
	virtual ~CTestCommDBWrapper()
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
		CRow* row1 = new CRow;
		row1->WriteTextL(TPtrC(COMMDB_NAME),KTestIAP());
		
		TPtrC key2(COMMDB_ID);
		TUint32 data2 = 1;
		row1->WriteUintL(key2,data2);
		
		TPtrC key3(IAP_NETWORK);
		TUint32 data3 = 1;
		row1->WriteUintL(key3,data3);
						
		IAPTable.iData.Append(row1);
		iDatabase.Append(IAPTable);
	}
	void AddBearerTableL()
	{
		CRow* row = new CRow;
		TTable bearerTable(KTestBearerType);
		
		TPtrC key(COMMDB_NAME);
		TBuf<32> data(KTestBearer);
		row->WriteTextL(key,data);

		TPtrC key2(COMMDB_ID);
		TUint32 data2 = 1;
		row->WriteUintL(key2,data2);

		TPtrC key3(IF_NAME);
		TBuf<32> data3(KTestIfName);
		row->WriteTextL(key3,data3);

		bearerTable.iData.Append(row);
		iDatabase.Append(bearerTable);
	}

	void AddServiceTableL()
	{
		CRow* row = new CRow;
		TTable bearerTable(KTestServiceType);
		
		TPtrC key(COMMDB_NAME);
		TBuf<32> data(KTestServiceName);
		row->WriteTextL(key,data);

		TPtrC key2(COMMDB_ID);
		TUint32 data2 = 1;
		row->WriteUintL(key2,data2);

		bearerTable.iData.Append(row);
		iDatabase.Append(bearerTable);
	}
	
	void AddNetworkTableL()
	{
		CRow* row = new CRow;
		TTable bearerTable(TPtrC(NETWORK));

		TPtrC key(COMMDB_NAME);
		TBuf<32> data(KTestNetworkName);
		row->WriteTextL(key,data);

		TPtrC key2(COMMDB_ID);
		TUint32 data2 = 1;
		row->WriteUintL(key2,data2);

		bearerTable.iData.Append(row);
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
		iDatabase.Append(wapTable);
	}
	
	void ConstructL()
	{
		AddIAPTableL();
		AddBearerTableL();
		AddServiceTableL();
		AddNetworkTableL();
		AddWAPAcessPointTableL();
		AddWAPBearerTableL();
		AddOutgoingGPRSTableL();
	}
	TInt BeginTransaction()
	{}
	
	TInt CommitTransaction()
	{}
	
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
			RDebug::Print(_L("Table %S"),&(iDatabase[i].iName));
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

MUnknown * CTestCommDBUtil::CommDBWrapperCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	switch(aImplementationUid.iUid)
	{
	case KCID_MCommDBWrapper:
	{
		CTestCommDBWrapper* ret = new (ELeave) CTestCommDBWrapper;
		gDatabase = ret;
		ret->ConstructL();
		return ret;
	}
	default:
		return NULL;
	}
}

void CTestCommDBUtil::testFindIAP()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = 0;
	TUint32 iap = commDB->FindIAPL(KTestIAP,network);
	TS_ASSERT(iap != 0);
	TS_ASSERT(network != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testFindBearer()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 bearer = commDB->FindBearerL(KTestBearerType,KTestBearer);
	TS_ASSERT(bearer != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testFindService()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 service = commDB->FindServiceL(KTestServiceType,KTestServiceName);
	TS_ASSERT(service != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testFindNetwork()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->FindNetworkL(KTestNetworkName);
	TS_ASSERT(network != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testCreateNewInternetAccessPoint()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 IAPService = 2;
	TUint32 IAPBearer = 3;
	TUint32 IAPNetwork = 4;
	TUint32 iap = commDB->CreateNewInternetAccessPointL(KBlueWhaleIAP(),IAPService,IAPBearer,TPtrC(MODEM_BEARER),IAPNetwork);
	TS_ASSERT(iap != 0);
	TUint32 foundNetwork=0;
	TUint32 foundIap = commDB->FindIAPL(KBlueWhaleIAP(),foundNetwork);
	TS_ASSERT(iap == foundIap);
	TS_ASSERT(IAPNetwork == foundNetwork);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testCreateNewWAPAccessPoint()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 iap = commDB->CreateNewWAPAccessPointL(KBlueWhaleIAP());
	TS_ASSERT(iap != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testCreateNewWAPBearer()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 bearer = commDB->CreateNewWAPBearerL(5,6);
	TS_ASSERT(bearer != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testCreateNewNetwork()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->CreateNewNetworkL(KBlueWhaleIAP());
	TS_ASSERT(network != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testCreateNewOutgoingGprs()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->CreateNewOutgoingGprsL(KBlueWhaleIAP(),KTestAPN(), KTestUser(), KTestPass());
	TS_ASSERT(network != 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testRemoveOutgoingGprs()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->CreateNewOutgoingGprsL(KBlueWhaleIAP(),KTestAPN(), KTestUser(), KTestPass());
	TUint32 out = commDB->RemoveOutgoingGprsL(KBlueWhaleIAP());
	TS_ASSERT(out != 0);
	out = commDB->RemoveOutgoingGprsL(KBlueWhaleIAP());
	TS_ASSERT(out == 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testUpdateOutgoingGprs()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->CreateNewOutgoingGprsL(KBlueWhaleIAP(),KTestAPN(), KTestUser(), KTestPass());
	if(gDatabase)
	{
		gDatabase->Dump();
	}
	TS_ASSERT(network != 0);
	TUint32 recId = commDB->UpdateOutgoingGprsL(KBlueWhaleIAP(),KTestAPN2(), KTestUser2(), KTestPass2());
	TS_ASSERT(network == recId);
	if(gDatabase)
	{
		gDatabase->Dump();
		TBool found = gDatabase->Find(_L("OutgoingGPRS"),TPtrC(GPRS_APN),KTestAPN2());
		TS_ASSERT(found);
	}
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testRemoveWAPAccessPoint()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 iap = commDB->CreateNewWAPAccessPointL(KBlueWhaleIAP());
	iap = commDB->RemoveWAPAccessPointL(KBlueWhaleIAP());
	TS_ASSERT(iap != 0);
	iap = commDB->RemoveWAPAccessPointL(KBlueWhaleIAP());
	TS_ASSERT(iap == 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testRemoveNetwork()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 network = commDB->CreateNewNetworkL(KBlueWhaleIAP());
	network = commDB->RemoveNetworkL(KBlueWhaleIAP());
	TS_ASSERT(network != 0);
	network = commDB->RemoveNetworkL(KBlueWhaleIAP());
	TS_ASSERT(network == 0);
	CleanupStack::PopAndDestroy(commDB);
	
}
void CTestCommDBUtil::testRemoveInternetAcessPoint()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 IAPService = 2;
	TUint32 IAPBearer = 3;
	TUint32 IAPNetwork = 4;
	TUint32 iap = commDB->CreateNewInternetAccessPointL(KBlueWhaleIAP(),IAPService,IAPBearer,TPtrC(MODEM_BEARER),IAPNetwork);
	iap = commDB->RemoveInternetAcessPointL(KBlueWhaleIAP());
	TS_ASSERT(iap != 0);
	iap = commDB->RemoveInternetAcessPointL(KBlueWhaleIAP());
	TS_ASSERT(iap == 0);
	CleanupStack::PopAndDestroy(commDB);
}

void CTestCommDBUtil::testRemoveWAPBearer()
{
	REComPlusSession::SetDelegate(CommDBWrapperCreate);
	MCCommDBUtil* commDB = static_cast<MCCommDBUtil*>(CCommDBUtil::NewL());
	CleanupReleasePushL(*commDB);
	TUint32 bearer = commDB->CreateNewWAPBearerL(5,6);
	bearer = commDB->RemoveWAPBearerL(5,6);
	TS_ASSERT(bearer != 0);
	bearer = commDB->RemoveWAPBearerL(5,6);
	TS_ASSERT(bearer == 0);
	CleanupStack::PopAndDestroy(commDB);
}
