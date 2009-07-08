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

#ifndef __COMMDBUTIL_H__
#define __COMMDBUTIL_H__
#include <Commdb.h>
#include <EcomPlusRefCountedBase.h>

const TInt KCID_MCommDBWrapper = 0xA0003F5C;
const TInt KIID_MCommDBWrapper = 0xA000B18B;

class MCCommsDbTableViewWrapper
{
public:
	virtual void Release() = 0;
	virtual TInt GotoFirstRecord() = 0;
	virtual TInt GotoNextRecord() = 0;
	virtual TInt InsertRecord(TUint32& aId) = 0;
	virtual TInt UpdateRecord() = 0;
	virtual void ReadTextL(const TDesC& aColumn, TDes16& aValue) = 0;
	virtual void ReadUintL(const TDesC& aColumn, TUint32& aValue) = 0;
	virtual void WriteTextL(const TDesC& aColumn, const TDesC16& aValue) = 0;
	virtual void WriteLongTextL(const TDesC& aColumn, const TDesC& aValue) = 0;
	virtual void WriteUintL(const TDesC& aColumn, const TUint32& aValue) = 0;
	virtual void WriteBoolL(const TDesC& aColumn, const TBool& aValue) = 0;
	virtual TInt PutRecordChanges(TBool aHidden = EFalse, TBool aReadOnly = EFalse) = 0;
	virtual TInt DeleteRecord() = 0;
protected:
	virtual ~MCCommsDbTableViewWrapper(){}
};

class CCommsDbTableViewWrapper: public MCCommsDbTableViewWrapper
{
public:
	CCommsDbTableViewWrapper(CCommsDbTableView* aView);
private:
	virtual ~CCommsDbTableViewWrapper();
	CCommsDbTableViewWrapper();
	// MCCommsDbTableViewWrapper
	virtual void Release();
	virtual TInt GotoFirstRecord();
	virtual TInt GotoNextRecord();
	virtual TInt InsertRecord(TUint32& aId);
	virtual TInt UpdateRecord();
	virtual void ReadTextL(const TDesC& aColumn, TDes16& aValue);
	virtual void ReadUintL(const TDesC& aColumn, TUint32& aValue);
	virtual void WriteTextL(const TDesC& aColumn, const TDesC16& aValue);
	virtual void WriteLongTextL(const TDesC& aColumn, const TDesC& aValue);
	virtual void WriteUintL(const TDesC& aColumn, const TUint32& aValue);
	virtual void WriteBoolL(const TDesC& aColumn, const TBool& aValue);
	virtual TInt PutRecordChanges(TBool aHidden = EFalse, TBool aReadOnly = EFalse);
	virtual TInt DeleteRecord();

private:
	CCommsDbTableView* iView;
};

class MCommDBWrapper : public MUnknown
{
public:
	virtual TInt BeginTransaction() = 0;
	virtual TInt CommitTransaction() = 0;
	virtual void RollbackTransaction() = 0;
	virtual MCCommsDbTableViewWrapper* OpenTableL(const TDesC& aTableName) = 0;
protected:
	virtual ~MCommDBWrapper(){}
};

class CCommDBWrapper : public CEComPlusRefCountedBase, public MCommDBWrapper
{
public:
	static MUnknown * NewL( TAny * aConstructionParameters );

protected: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() {CEComPlusRefCountedBase::AddRef();}
	void Release() {CEComPlusRefCountedBase::Release();}

protected:// MCommDBWrapper
	virtual TInt BeginTransaction();
	virtual TInt CommitTransaction();
	virtual void RollbackTransaction();
	virtual MCCommsDbTableViewWrapper* OpenTableL(const TDesC& aTableName);

private: 
	CCommDBWrapper( TAny * aConstructionParameters );
	CCommDBWrapper();
	~CCommDBWrapper();
	void ConstructL();
private:
	CCommsDatabase* iCommDB;
};
class MCCommDBUtil
{
public:
	virtual ~MCCommDBUtil(){}
	virtual void Release() = 0;
	virtual TInt BeginTransaction() = 0;
	virtual TInt CommitTransaction() = 0;
	virtual void RollbackTransaction() = 0;
	virtual TUint32 FindIAPL(const TDesC& aName,TUint32& aNetwork) = 0;
	virtual TUint32 FindBearerL(const TDesC& aBearerType,const TDesC& aName) = 0;
	virtual TUint32 FindServiceL(const TDesC& aServiceType,const TDesC& aName) = 0;
	virtual TUint32 FindNetworkL(const TDesC& aName) = 0;
	
	virtual TUint32 CreateNewInternetAccessPointL(const TDesC& aName,TUint32 aIAPService,TUint32 aIAPBearer,const TDesC& aBearerType,TUint32 aIAPNetwork) = 0;
	virtual TUint32 CreateNewWAPAccessPointL(const TDesC& aName) = 0;
	virtual TUint32 CreateNewWAPBearerL(TUint32 aWAPID,TUint32 aIAPID) = 0;
	virtual TUint32 CreateNewNetworkL(const TDesC& aName) = 0;
	virtual TUint32 CreateNewOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass) = 0;
	virtual TUint32 RemoveOutgoingGprsL(const TDesC& aName) = 0;
	virtual TUint32 RemoveWAPAccessPointL(const TDesC& aName) = 0;
	virtual TUint32 RemoveNetworkL(const TDesC& aName) = 0;
	virtual TUint32 RemoveInternetAcessPointL(const TDesC& aName) = 0;
	virtual TUint32 RemoveWAPBearerL(TUint32 aWAPId,TUint32 aIAPId) = 0;
	virtual TUint32 UpdateOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass) = 0;
	
	//virtual TUint32 DuplicateIAPL(TUint32 aIAP) = 0;
	//virtual void SetIAPName(TUint32 aIAP,const TDesC& aName) = 0;

};
class CCommDBUtil : public CBase, public MCCommDBUtil
{
public:
	static CCommDBUtil* NewL();
	virtual void Release();
	TInt BeginTransaction();
	TInt CommitTransaction();
	void RollbackTransaction();
	TUint32 FindIAPL(const TDesC& aName,TUint32& aNetwork);
	TUint32 FindBearerL(const TDesC& aBearerType,const TDesC& aName);
	TUint32 FindServiceL(const TDesC& aServiceType,const TDesC& aName);
	TUint32 FindNetworkL(const TDesC& aName);
	
	TUint32 CreateNewInternetAccessPointL(const TDesC& aName,TUint32 aIAPService,TUint32 aIAPBearer,const TDesC& aBearerType,TUint32 aIAPNetwork);
	TUint32 CreateNewWAPAccessPointL(const TDesC& aName);
	TUint32 CreateNewWAPBearerL(TUint32 aWAPID,TUint32 aIAPID);
	TUint32 CreateNewNetworkL(const TDesC& aName);
	TUint32 CreateNewOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass);

	TUint32 RemoveOutgoingGprsL(const TDesC& aName);
	TUint32 RemoveWAPAccessPointL(const TDesC& aName);
	TUint32 RemoveNetworkL(const TDesC& aName);
	TUint32 RemoveInternetAcessPointL(const TDesC& aName);
	TUint32 RemoveWAPBearerL(TUint32 aWAPId,TUint32 aIAPId);
	virtual TUint32 UpdateOutgoingGprsL(const TDesC& aName, const TDesC& aAPN, const TDesC& aUser, const TDesC& aPass);

	//TUint32 DuplicateIAPL(TUint32 aIAP);
	//void SetIAPName(TUint32 aIAP,const TDesC& aName);
protected:
	CCommDBUtil();
	virtual ~CCommDBUtil();
	void ConstructL();
private:
	MCommDBWrapper* iCommDb;
};


#endif /*__COMMDBUTIL_H__*/
