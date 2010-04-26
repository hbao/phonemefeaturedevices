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

#ifndef __TESTAPNMANAGERCOMMDBWRAPPER_H__
#define __TESTAPNMANAGERCOMMDBWRAPPER_H__
#include "DummyCommDB.h"

class CTestSimpleCommDBWrapper : public CBase,public MCommDBWrapper
{
public:
	CTestSimpleCommDBWrapper()
	{}
	CTestSimpleCommDBWrapper(CTestSimpleCommDBWrapper* aDest):iDest(aDest)
    {}
	
	void AddIAPTableL()
	{
		TTable IAPTable(TPtrC(IAP));
		CRow* service = new (ELeave) CRow;
		service->WriteTextL(TPtrC(COMMDB_NAME),_L("Winsock Service"));
		service->WriteUintL(TPtrC(COMMDB_ID),1);
		IAPTable.iData.Append(service);

		iDatabase.Append(IAPTable);

		TTable IAPTable2(TPtrC(IAP));
		CRow* service2 = new (ELeave) CRow;
		service2->WriteTextL(TPtrC(COMMDB_NAME),_L("Dummy Service"));
		service2->WriteUintL(TPtrC(COMMDB_ID),2);
		IAPTable2.iData.Append(service2);

		iDatabase.Append(IAPTable2);
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
		return NULL;
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
	void Duplicate()
	{
	    TInt c = iDatabase.Count();
	    for(TInt i=0;i<c;i++)
	    {
	        iDest->iDatabase.Append(TTable(iDatabase[i]));
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
private:
	   virtual ~CTestSimpleCommDBWrapper()
	    {
	       if(iDest)
	       {
	           Duplicate();
	       }
	        TInt count = iDatabase.Count();
	        for(TInt i=0;i<count;i++)
	        {
	            iDatabase[i].Reset();
	        }
	        iDatabase.Reset();
	    }

	   CTestSimpleCommDBWrapper* iDest;
};



#endif /*TESTAPNMANAGERCOMMDBWRAPPER_H_*/
