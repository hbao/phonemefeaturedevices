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

#ifndef __DUMMYCOMMDB_H__
#define __DUMMYCOMMDB_H__

#include "commDBUtil.h"

class TIntData
{
public:
	TIntData(const TDesC& aKey,TUint32 aValue) : iValue(aValue)
	{
		iKey.Copy(aKey);
	}
	TBuf<32> iKey;
	TUint32 iValue;
};

class CStrData : public CBase
{
public:
	CStrData()
	{}
	
	CStrData(const TDesC& aKey,const TDesC& aValue)
	{
		iKey = aKey.Alloc();
		iValue = aValue.Alloc();
	}
	CStrData(CStrData& aCopy)
	{
		iKey = aCopy.iKey->Alloc();
		iValue = aCopy.iValue->Alloc();
	}
	virtual ~CStrData()
	{
		delete iKey;
		delete iValue;
	}
	HBufC* iKey;
	HBufC* iValue;
};

class CRow : public CBase
{
public:
	CRow()
	{}
	CRow(CRow& aObject)
	{
		TInt count = aObject.iIntegers.Count();
		for(TInt i=0;i<count;i++)
		{
			iIntegers.Append(aObject.iIntegers[i]);
		}
		count = aObject.iStrings.Count();
		for(TInt i=0;i<count;i++)
		{
			iStrings.Append(new CStrData(*(aObject.iStrings[i])));
		}
	}
	
	virtual ~CRow()
	{
		iIntegers.Reset();
		TInt count = iStrings.Count();
		for(TInt i=0;i<count;i++)
		{
			delete iStrings[i];
		}
		iStrings.Reset();
	}
	void WriteUintL(const TDesC& aKey,TUint32 aValue)
	{
		TIntData entry(aKey,aValue);
		User::LeaveIfError(iIntegers.Append(entry));
	}
	void WriteTextL(const TDesC& aColumn,const TDesC& aValue)
	{
		TIdentityRelation<CStrData> relationship(CompareKeys);
		CStrData lookfor(aColumn,KNullDesC());
		TInt pos = iStrings.Find(&lookfor,relationship);
		if(pos != KErrNotFound)
		{
			delete iStrings[pos]->iValue;
			iStrings[pos]->iValue = aValue.Alloc();
		}
		else
		{
			CStrData* entry = new CStrData(aColumn,aValue);
			User::LeaveIfError(iStrings.Append(entry));
		}
	}
			
	static TBool CompareKeys(const CStrData & aLeft,const CStrData& aRight)
	{
		if(aLeft.iKey->Des().Compare(aRight.iKey->Des()) == 0)
		{
			return ETrue;
		}
		else
		{
			return EFalse;
		}
	}
		
	void ReadText(const TDesC& aColumn, TDes16& aValue)
	{
		TIdentityRelation<CStrData> relationship(CompareKeys);
		CStrData lookfor(aColumn,KNullDesC());
		TInt pos = iStrings.Find(&lookfor,relationship);
		if(pos != KErrNotFound)
		{
			aValue.Copy(iStrings[pos]->iValue->Des());
		}
	}

	static TBool CompareIntKeys(const TIntData& aLeft,const TIntData& aRight)
	{
		if(aLeft.iKey.Compare(aRight.iKey) == 0)
		{
			return ETrue;
		}
		else
		{
			return EFalse;
		}
	}
	
	void ReadUintL(const TDesC& aColumn, TUint32& aValue)
	{
		TBuf<32> col(aColumn);
		TUint32 val=0;
		TIntData lookfor(col,val);
		TIdentityRelation<TIntData> relationship(CompareIntKeys);
		TInt pos = iIntegers.Find(lookfor,relationship);
		if(pos != KErrNotFound)
		{
			aValue = iIntegers[pos].iValue;
		}
	}
	void Dump()
	{
		RDebug::Print(_L("-----------------------ROW------------------------------"));
		TInt c = iStrings.Count();
		for(TInt i=0;i<c;i++)
		{
			HBufC* key = iStrings[i]->iKey;
			HBufC* value = iStrings[i]->iValue;
			RDebug::Print(_L("%S %S"),key,value);
		}
		c = iIntegers.Count();
		for(TInt i=0;i<c;i++)
		{
			RDebug::Print(_L("%S %d"),&(iIntegers[i].iKey),iIntegers[i].iValue);
		}
	}
	TBool Find(const TDesC & aKey,const TDesC& aValue)
	{
		TInt c = iStrings.Count();
		for(TInt i=0;i<c;i++)
		{
			RDebug::Print( _L("Row %d %S") , i , iStrings[i]->iKey );
			if(iStrings[i]->iKey->Des().Compare(aKey) == 0 && iStrings[i]->iValue->Des().Compare(aValue) == 0)
			{
				return ETrue;
			}
		}
		c = iIntegers.Count();
		for(TInt i=0;i<c;i++)
		{
			if(iIntegers[i].iKey.Compare(aKey) == 0)
			{
				return ETrue;
			}
		}
		return EFalse;
	}
protected:
	RArray<TIntData> iIntegers;
	RPointerArray<CStrData> iStrings;
};

class TTable
{
public:
	TTable(const TDesC& aName): iName(aName)
	{
	}
	TTable(const TTable& aCopy)
	{
	    iName.Copy(aCopy.iName);
	    TInt c = aCopy.iData.Count();
	    for(TInt i=0;i<c;i++)
	    {
	        CRow* row = aCopy.iData[i];
	        iData.Append(new CRow(*row));
	    }
	}
	
	virtual ~TTable()
	{
	}
	void Reset()
	{
		TInt c = iData.Count();
		for(TInt i=0;i<c;i++)
		{
			delete iData[i];
		}
		iData.Reset();
	}
	void Dump()
	{
		RDebug::Print(_L("========================TABLE============================="));
		RDebug::Print(_L("%S"),&iName);
		TInt c = iData.Count();
		for(TInt i=0;i<c;i++)
		{
			iData[i]->Dump();
		}
	}
	TBool Find(const TDesC& aField,const TDesC& aValue)
	{
		TInt c = iData.Count();
		for(TInt i=0;i<c;i++)
		{
			if(iData[i]->Find(aField,aValue))
			{
				return ETrue;
			}
		}
		return EFalse;
	}
	TBuf<32> iName; 
	RPointerArray<CRow> iData;
};
class CTestCommsDbTableViewWrapper : public MCCommsDbTableViewWrapper
{
public:
	CTestCommsDbTableViewWrapper(TTable& aTable) : iTable(aTable)
	{
	}
		
	virtual ~CTestCommsDbTableViewWrapper()
	{
	}
	
	void Release()
	{
		delete this;
	}
	TInt GotoFirstRecord()
	{
		iReadyToModify = EFalse;
					
		if(iTable.iData.Count() > 0)
		{
			iIndex = 0;
			return KErrNone;
		}
		else
		{
			return KErrEof;
		}
	}
	TInt GotoNextRecord()
	{
		iReadyToModify = EFalse;
		if(++iIndex < iTable.iData.Count())
		{
			return KErrNone;
		}
		else
		{
			return KErrEof;
		}
	}
	TInt InsertRecord(TUint32& aId)
	{
		CRow* blankRow = new CRow;
		TInt ret = iTable.iData.Append(blankRow);
		if(ret == KErrNone)
		{
			aId = iTable.iData.Count();
			blankRow->WriteUintL(TPtrC(COMMDB_ID),aId);
			iIndex = aId - 1;
		}
		else
		{
			delete blankRow;
		}
		iReadyToModify = ETrue;
		RDebug::Print(_L("InsertRecord %d"),aId);
		return ret;
	}
	TInt UpdateRecord()
	{
		iReadyToModify = ETrue;
		return KErrNone;
	}
	void ReadTextL(const TDesC& aColumn, TDes16& aValue)
	{
		iTable.iData[iIndex]->ReadText(aColumn,aValue);
	}
	void ReadUintL(const TDesC& aColumn, TUint32& aValue)
	{
		iTable.iData[iIndex]->ReadUintL(aColumn,aValue);
	}
	
	void WriteTextL(const TDesC& aColumn, const TDesC16& aValue)
	{
		if(!iReadyToModify)
		{
			User::Panic(_L("CommsaDbServer"),12);
		}
		if(aColumn.Compare(TPtrC(COMMDB_NAME)) == 0)
		{
			RDebug::Print(_L("Rec name %S"),&aValue);
		}
		iTable.iData[iIndex]->WriteTextL(aColumn,aValue);
	}

	void WriteLongTextL(const TDesC& aColumn, const TDesC& aValue)
	{
		if(!iReadyToModify)
		{
			User::Panic(_L("CommsaDbServer"),12);
		}
		iTable.iData[iIndex]->WriteTextL(aColumn,aValue);
	}
	
	void WriteUintL(const TDesC& aColumn, const TUint32& aValue)
	{
		if(!iReadyToModify)
		{
			User::Panic(_L("CommsaDbServer"),12);
		}
		iTable.iData[iIndex]->WriteUintL(aColumn,aValue);
	}
	
	void WriteBoolL(const TDesC& aColumn, const TBool& aValue)
	{
		if(!iReadyToModify)
		{
			User::Panic(_L("CommsaDbServer"),12);
		}
		iTable.iData[iIndex]->WriteUintL(aColumn,aValue);
	}
	
	TInt PutRecordChanges(TBool /*aHidden*/, TBool /*aReadOnly*/)
	{
		return KErrNone;
	}
	
	virtual TInt DeleteRecord()
	{
		delete iTable.iData[iIndex];
		iTable.iData.Remove(iIndex);
		return KErrNone;
	}
	
	TTable& iTable;
	TInt iIndex;
	TInt iRef;
	TBool iReadyToModify;

};


#endif 
