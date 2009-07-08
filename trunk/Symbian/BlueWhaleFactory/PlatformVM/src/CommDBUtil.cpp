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
#include <e32svr.h>
#include "CommDBUtil.h"
#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////
CCommsDbTableViewWrapper::CCommsDbTableViewWrapper(CCommsDbTableView* aView) : iView(aView)
{
	//RDebug::Print(_L("CCommsDbTableViewWrapper 0x%08x"),this);
}

CCommsDbTableViewWrapper::CCommsDbTableViewWrapper()
{}

CCommsDbTableViewWrapper::~CCommsDbTableViewWrapper()
{
	//RDebug::Print(_L("~CCommsDbTableViewWrapper 0x%08x"),this);
	delete iView;
}
void CCommsDbTableViewWrapper::Release()
{
	delete this;
}
TInt CCommsDbTableViewWrapper::GotoFirstRecord()
{
	return iView->GotoFirstRecord();
}

TInt CCommsDbTableViewWrapper::GotoNextRecord()
{
	return iView->GotoNextRecord();
}

TInt CCommsDbTableViewWrapper::InsertRecord(TUint32& aId)
{
	return iView->InsertRecord(aId);
}
TInt CCommsDbTableViewWrapper::UpdateRecord()
{
	return iView->UpdateRecord();
}
void CCommsDbTableViewWrapper::ReadTextL(const TDesC& aColumn, TDes16& aValue)
{
	iView->ReadTextL(aColumn,aValue);
}

void CCommsDbTableViewWrapper::ReadUintL(const TDesC& aColumn, TUint32& aValue)
{
	iView->ReadUintL(aColumn,aValue);
}

void CCommsDbTableViewWrapper::WriteTextL(const TDesC& aColumn, const TDesC16& aValue)
{
	iView->WriteTextL(aColumn,aValue);
}

void CCommsDbTableViewWrapper::WriteLongTextL(const TDesC& aColumn, const TDesC& aValue)
{
	iView->WriteLongTextL(aColumn,aValue);
}

void CCommsDbTableViewWrapper::WriteUintL(const TDesC& aColumn, const TUint32& aValue)
{
	iView->WriteUintL(aColumn,aValue);
}

void CCommsDbTableViewWrapper::WriteBoolL(const TDesC& aColumn, const TBool& aValue)
{
	iView->WriteBoolL(aColumn,aValue);
}

TInt CCommsDbTableViewWrapper::PutRecordChanges(TBool aHidden, TBool aReadOnly)
{
	return iView->PutRecordChanges(aHidden,aReadOnly);
}

TInt CCommsDbTableViewWrapper::DeleteRecord()
{
	return iView->DeleteRecord();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
MUnknown * CCommDBWrapper::NewL( TAny * aConstructionParameters )
{
	CCommDBWrapper* self = new (ELeave) CCommDBWrapper(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;
}
CCommDBWrapper::CCommDBWrapper( TAny * aConstructionParameters ): CEComPlusRefCountedBase(aConstructionParameters)
{}

CCommDBWrapper::CCommDBWrapper(): CEComPlusRefCountedBase(NULL)
{}

CCommDBWrapper::~CCommDBWrapper()
{
	delete iCommDB;
}

void CCommDBWrapper::ConstructL()
{
	iCommDB = CCommsDatabase::NewL();
}

MUnknown * CCommDBWrapper::QueryInterfaceL( TInt aInterfaceId )
{	
	if( KIID_MCommDBWrapper == aInterfaceId )
	{
		AddRef();
		return static_cast<MCommDBWrapper*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

MCCommsDbTableViewWrapper* CCommDBWrapper::OpenTableL(const TDesC& aTableName)
{
	CCommsDbTableView* view = iCommDB->OpenTableLC(aTableName);
	CCommsDbTableViewWrapper* wrapper = new (ELeave) CCommsDbTableViewWrapper(view);
	CleanupStack::Pop(view);
	return wrapper;
}
TInt CCommDBWrapper::BeginTransaction()
{
	return iCommDB->BeginTransaction();
}

TInt CCommDBWrapper::CommitTransaction()
{
	return iCommDB->CommitTransaction();
}

void CCommDBWrapper::RollbackTransaction()
{
	iCommDB->RollbackTransaction();
}

////////////////////////////////////////////////////////////////////////////
	
EXPORT_DECL CCommDBUtil* CCommDBUtil::NewL()
{
	CCommDBUtil* self = new (ELeave)CCommDBUtil;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CCommDBUtil::CCommDBUtil()
{}

CCommDBUtil::~CCommDBUtil()
{
	if(iCommDb)
	{
		iCommDb->Release();
		iCommDb = NULL;
	}
}

void CCommDBUtil::ConstructL()
{
	iCommDb = static_cast<MCommDBWrapper*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MCommDBWrapper), TUid::Uid(KIID_MCommDBWrapper),NULL));
}
void CCommDBUtil::Release()
{
	delete this;
}
TInt CCommDBUtil::BeginTransaction()
{
	return iCommDb->BeginTransaction();
}
TInt CCommDBUtil::CommitTransaction()
{
	return iCommDb->CommitTransaction();	
}

void CCommDBUtil::RollbackTransaction()
{
	iCommDb->RollbackTransaction();
}

TUint32 CCommDBUtil::FindIAPL(const TDesC& aName,TUint32& aNetwork)
{
	TUint32 ret =0;
	MCCommsDbTableViewWrapper* commsAp = iCommDb->OpenTableL(TPtrC(IAP));
	CleanupReleasePushL(*commsAp);
	TInt err = commsAp->GotoFirstRecord();
	TBuf<64> name;
	TBuf<64> serviceType;
	TBuf<64> bearerType;
	TUint32 IAPID;
	while(err == KErrNone)
	{
		commsAp->ReadTextL(TPtrC(COMMDB_NAME),name);
		commsAp->ReadUintL(TPtrC(COMMDB_ID),IAPID);
		commsAp->ReadUintL(TPtrC(IAP_NETWORK),aNetwork);
		    
		if(name.Compare(aName) == 0)
		{
			ret = IAPID;
			break;
		}
		err = commsAp->GotoNextRecord();
	}
	
	CleanupStack::PopAndDestroy(commsAp);
	return ret;
}

TUint32 CCommDBUtil::FindNetworkL(const TDesC& aName)
{
	TUint32 ret = 0;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(NETWORK));
	CleanupReleasePushL(*view);
	TBuf<64> name;
	TUint32 networkID = 0; 
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		view->ReadUintL(TPtrC(COMMDB_ID),networkID);
		if(name.Compare(aName) == 0)
		{
			ret = networkID;
			break;
		}
		err = view->GotoNextRecord();
	}			
	CleanupStack::PopAndDestroy(view);
		
	return ret;
}


TUint32 CCommDBUtil::FindServiceL(const TDesC& aServiceType,const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<64> name;
	TUint32 serviceID = 0; 
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(aServiceType);
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		view->ReadUintL(TPtrC(COMMDB_ID),serviceID);
		if(name.Compare(aName) == 0)
		{
			ret = serviceID;
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);
	return ret;
}

TUint32 CCommDBUtil::FindBearerL(const TDesC& aBearerType,const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<64> name;
	TBuf<64> ifName;
	TUint32 bearerID = 0; 
	MCCommsDbTableViewWrapper* commsAp = iCommDb->OpenTableL(aBearerType);
	CleanupReleasePushL(*commsAp);
	TInt err = commsAp->GotoFirstRecord();
	while(err == KErrNone)
	{
		commsAp->ReadTextL(TPtrC(COMMDB_NAME),name);
		commsAp->ReadUintL(TPtrC(COMMDB_ID),bearerID);
		commsAp->ReadTextL(TPtrC(IF_NAME),ifName);
		if(name.Compare(aName) == 0)
		{
			ret = bearerID;
			break;
		}
		err = commsAp->GotoNextRecord();
	}			
	CleanupStack::PopAndDestroy(commsAp);

	return ret;
}

TUint32 CCommDBUtil::CreateNewOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass)
{
	TUint32 id;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(OUTGOING_GPRS));
	CleanupReleasePushL(*view);
	
	User::LeaveIfError(view->InsertRecord(id));
	
	view->WriteTextL(TPtrC(COMMDB_NAME), aName);
	
	view->WriteLongTextL(TPtrC(GPRS_APN), aAPN);
	view->WriteLongTextL(TPtrC(GPRS_IF_AUTH_NAME), aUser);
	view->WriteLongTextL(TPtrC(GPRS_IF_AUTH_PASS), aPass);

	view->WriteUintL( TPtrC(GPRS_PDP_TYPE), 0);

	view->WriteUintL( TPtrC(GPRS_REQ_PRECEDENCE), 0);

	view->WriteUintL( TPtrC(GPRS_REQ_DELAY), 0);

	view->WriteUintL( TPtrC(GPRS_REQ_RELIABILITY), 0);

	view->WriteUintL( TPtrC(GPRS_REQ_PEAK_THROUGHPUT), 0);

	view->WriteUintL( TPtrC(GPRS_REQ_MEAN_THROUGHPUT), 0);

	view->WriteUintL( TPtrC(GPRS_MIN_PRECEDENCE), 0);

	view->WriteUintL( TPtrC(GPRS_MIN_DELAY), 0);

	view->WriteUintL( TPtrC(GPRS_MIN_RELIABILITY), 0);

	view->WriteUintL( TPtrC(GPRS_MIN_PEAK_THROUGHPUT), 0);

	view->WriteUintL( TPtrC(GPRS_MIN_MEAN_THROUGHPUT), 0);

	view->WriteBoolL( TPtrC(GPRS_DATA_COMPRESSION), EFalse);

	view->WriteBoolL( TPtrC(GPRS_HEADER_COMPRESSION), EFalse);

	view->WriteBoolL( TPtrC(GPRS_ANONYMOUS_ACCESS), EFalse);

	view->WriteTextL( TPtrC(GPRS_IF_NETWORKS), _L("ip"));

	view->WriteBoolL( TPtrC(GPRS_IF_PROMPT_FOR_AUTH), EFalse);

	view->WriteBoolL( TPtrC(GPRS_IP_DNS_ADDR_FROM_SERVER), ETrue);
	
	view->WriteTextL( TPtrC(GPRS_IP_NAME_SERVER1), _L("0.0.0.0"));
	view->WriteTextL( TPtrC(GPRS_IP_NAME_SERVER2), _L("0.0.0.0"));
	
	view->WriteBoolL( TPtrC(GPRS_ENABLE_LCP_EXTENSIONS), EFalse);

	view->WriteBoolL( TPtrC(GPRS_DISABLE_PLAIN_TEXT_AUTH), EFalse);

	view->WriteBoolL( TPtrC(GPRS_IP_ADDR_FROM_SERVER), ETrue);
	view->WriteUintL( TPtrC(GPRS_AP_TYPE), 2);

	view->WriteUintL( TPtrC(GPRS_QOS_WARNING_TIMEOUT), 0xffffffff);

	view->PutRecordChanges();
	CleanupStack::PopAndDestroy(view);

	return id;
	}

TUint32 CCommDBUtil::RemoveOutgoingGprsL(const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<128>  name;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(OUTGOING_GPRS));
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		if(name.Compare(aName) == 0)
		{
			view->ReadUintL(TPtrC(COMMDB_ID),ret);
			view->DeleteRecord();
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}

TUint32 CCommDBUtil::UpdateOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass)
{
	TUint32 ret = 0;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(OUTGOING_GPRS));
	CleanupReleasePushL(*view);
	TBuf<32> name;
	TUint32 id; 
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		view->ReadUintL(TPtrC(COMMDB_ID),id);
		if(name.Compare(aName) == 0)
		{
			view->UpdateRecord();
			view->WriteLongTextL(TPtrC(GPRS_APN), aAPN);
			view->WriteLongTextL(TPtrC(GPRS_IF_AUTH_NAME), aUser);
			view->WriteLongTextL(TPtrC(GPRS_IF_AUTH_PASS), aPass);
			view->PutRecordChanges();
			ret = id;
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}


TUint32 CCommDBUtil::CreateNewNetworkL(const TDesC& aName)
{
	TUint32 id;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(NETWORK));
	CleanupReleasePushL(*view);
	view->InsertRecord(id);
	view->WriteTextL(TPtrC(COMMDB_NAME), aName);
	view->PutRecordChanges();
	CleanupStack::PopAndDestroy(view);
	return id;
}

TUint32 CCommDBUtil::RemoveNetworkL(const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<128>  name;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(NETWORK));
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		if(name.Compare(aName) == 0)
		{
			view->ReadUintL(TPtrC(COMMDB_ID),ret);
			view->DeleteRecord();
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}

TUint32 CCommDBUtil::CreateNewWAPAccessPointL(const TDesC& aName)
   {
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(WAP_ACCESS_POINT));
	CleanupReleasePushL(*view);
	TUint32 id;
	User::LeaveIfError(view->InsertRecord(id));

	view->WriteTextL(TPtrC(COMMDB_NAME), aName);
	view->WriteTextL(TPtrC(WAP_CURRENT_BEARER),  TPtrC(WAP_IP_BEARER));
	
	view->PutRecordChanges();
	CleanupStack::PopAndDestroy(view);
	return id;
}

TUint32 CCommDBUtil::RemoveWAPAccessPointL(const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<128>  name;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(WAP_ACCESS_POINT));
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		if(name.Compare(aName) == 0)
		{
			view->ReadUintL(TPtrC(COMMDB_ID),ret);
			view->DeleteRecord();
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}


TUint32 CCommDBUtil::CreateNewWAPBearerL(TUint32 aWAPID,TUint32 aIAPID)
{
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(WAP_IP_BEARER));
	CleanupReleasePushL(*view);

	TUint32 id;
	User::LeaveIfError(view->InsertRecord(id));

	view->WriteUintL(TPtrC(WAP_ACCESS_POINT_ID), aWAPID);

	_LIT(wap_gw_address, "0.0.0.0");
	view->WriteTextL(TPtrC(WAP_GATEWAY_ADDRESS), wap_gw_address);

	view->WriteUintL( TPtrC(WAP_IAP), aIAPID);

	view->WriteUintL( TPtrC(WAP_WSP_OPTION), EWapWspOptionConnectionOriented);
    
	view->WriteBoolL( TPtrC(WAP_SECURITY), EFalse);
	view->WriteUintL( TPtrC(WAP_PROXY_PORT), 0 );

	view->PutRecordChanges();
	CleanupStack::PopAndDestroy(view);
	return id;
}

TUint32 CCommDBUtil::RemoveWAPBearerL(TUint32 aWAPId,TUint32 aIAPId)
{
	TUint32 ret = 0;
	TUint32 wapId;
	TUint32 iapId;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(WAP_IP_BEARER));
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadUintL(TPtrC(WAP_ACCESS_POINT_ID),wapId);
		view->ReadUintL(TPtrC(WAP_IAP),iapId);
		if(aWAPId == wapId && aIAPId == iapId)
		{
			view->ReadUintL(TPtrC(COMMDB_ID),ret);
			view->DeleteRecord();
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}


TUint32 CCommDBUtil::CreateNewInternetAccessPointL(const TDesC& aName,TUint32 aIAPService,TUint32 aIAPBearer,const TDesC& aBearerType,TUint32 aIAPNetwork)
{
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(IAP));
	CleanupReleasePushL(*view);
	TUint32 id;
	User::LeaveIfError(view->InsertRecord(id));
	
    view->WriteTextL(TPtrC(COMMDB_NAME), aName);
	
    view->WriteUintL(TPtrC(IAP_SERVICE), aIAPService);
	view->WriteTextL(TPtrC(IAP_SERVICE_TYPE), TPtrC(OUTGOING_GPRS));

	view->WriteTextL(TPtrC(IAP_BEARER_TYPE), aBearerType);
	view->WriteUintL( TPtrC(IAP_BEARER), aIAPBearer);
    
    view->WriteUintL( TPtrC(IAP_NETWORK), aIAPNetwork);
    view->WriteUintL( TPtrC(IAP_NETWORK_WEIGHTING), 0 );
    
    view->WriteUintL( TPtrC(IAP_LOCATION), 2);

    view->PutRecordChanges();
	CleanupStack::PopAndDestroy(view);
	return id;
}
TUint32 CCommDBUtil::RemoveInternetAcessPointL(const TDesC& aName)
{
	TUint32 ret = 0;
	TBuf<128>  name;
	MCCommsDbTableViewWrapper* view = iCommDb->OpenTableL(TPtrC(IAP));
	CleanupReleasePushL(*view);
	TInt err = view->GotoFirstRecord();
	while(err == KErrNone)
	{
		view->ReadTextL(TPtrC(COMMDB_NAME),name);
		if(name.Compare(aName) == 0)
		{
			view->ReadUintL(TPtrC(COMMDB_ID),ret);
			view->DeleteRecord();
			break;
		}
		err = view->GotoNextRecord();
	}
	CleanupStack::PopAndDestroy(view);	
	return ret;
}
