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


#ifndef __PROPERTIES_IMPL_H__
#define __PROPERTIES_IMPL_H__

#include <EcomPlusRefCountedBase.h>
#include "Properties.h"

static const TUint8 KCPropertiesVersion = 1;

class CProperties : public CEComPlusRefCountedBase, public MProperties, public MComparable, public MExternalizable
{
public:
	static MProperties * NewL( TAny * aConstructionParameters );

public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	virtual void AddRef() {CEComPlusRefCountedBase::AddRef();}
	virtual void Release() {CEComPlusRefCountedBase::Release();}

public: // MExternalizable
	virtual TUid GetECOMPlusComponentId() const;
	virtual TUint8 GetVersion() const { return KCPropertiesVersion;}
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);
	virtual const TTime & GetModified() const;
	virtual void SetModified(const TTime & aModifiedTime );


public: // MComparable
	virtual TInt Compare( MUnknown * aOther ) const;

public: // MProperties implementation.
	virtual TInt GetIntL(const TIntUniqueKey & aKey, TInt aOrdinal) const;
	virtual void SetIntL(const TIntUniqueKey & aKey, TInt aIntegerValue, TInt aOrdinal);
	virtual TBool DeleteInt(const TIntUniqueKey & aKey, TInt aOrdinal);

	virtual TInt64 GetInt64L(const TInt64UniqueKey & aKey, TInt aOrdinal) const;
	virtual void SetInt64L(const TInt64UniqueKey & aKey, TInt64 aIntegerValue, TInt aOrdinal);
	virtual TBool DeleteInt64(const TInt64UniqueKey & aKey, TInt aOrdinal);

	virtual const TDesC8 & GetString8L(const TString8UniqueKey & aKey, TInt aOrdinal) const;
	virtual HBufC8 * GetStringBuffer8L(const TString8UniqueKey & aKey, TInt aOrdinal) const;
	virtual void SetString8L(const TString8UniqueKey & aKey, HBufC8 * aBuffer, TInt aOrdinal);
	virtual void SetString8L(const TString8UniqueKey & aKey, const TDesC8 & aStringValue, TInt aOrdinal);
	virtual TBool DeleteString8(const TString8UniqueKey & aKey, TInt aOrdinal);

	virtual const TDesC & GetStringL(const TStringUniqueKey & aKey, TInt aOrdinal) const;
	virtual HBufC * GetStringBufferL(const TStringUniqueKey & aKey, TInt aOrdinal) const;
	virtual void SetStringL(const TStringUniqueKey & aKey, HBufC * aBuffer, TInt aOrdinal);
	virtual void SetStringL(const TStringUniqueKey & aKey, const TDesC & aStringValue, TInt aOrdinal);
	virtual TBool DeleteString(const TStringUniqueKey & aKey, TInt aOrdinal);

	virtual void SetObjectL(const TObjectUniqueKey & aKey, MUnknown * aUnknown /* IN */, TInt aOrdinal);
	virtual MUnknown * GetObjectL(const TObjectUniqueKey & aKey, TInt aInterfaceId, TInt aOrdinal );
	virtual TBool DeleteObject(const TObjectUniqueKey & aKey, TInt aOrdinal);

	virtual TBool Dirty() const;
	virtual void SetDirty(TBool aDirty = ETrue);
	virtual void Reset();
	
	virtual void DebugPrintL(TInt aNestingLevel) const;


protected:
	CProperties( TAny * aConstructionParameters );
	void ConstructL();
	virtual ~CProperties();
	
protected:

	class TIntKeyInt
	{
	public:
		TIntKeyInt(TIntUniqueKey aKey):iKey(aKey){}
		static TBool Match(const TIntKeyInt& aFirst, const TIntKeyInt& aSecond)
		{
			if(aFirst.iKey == aSecond.iKey)
			{
				return ETrue;
			}
			else
			{
				return EFalse;
			}
		}

		TIntUniqueKey iKey;
		RArray<TInt> iData;
	};

	class TIntKeyInt64
	{
	public:
		TIntKeyInt64(TInt64UniqueKey aKey){iKey = aKey;}
		static TBool Match(const TIntKeyInt64& aFirst, const TIntKeyInt64& aSecond)
		{
			if(aFirst.iKey == aSecond.iKey)
			{
				return ETrue;
			}
			else
			{
				return EFalse;
			}
		}


		TInt64UniqueKey iKey;
		RArray<TInt64> iData;
	};

	class TIntKeyStr8
	{
	public:
		TIntKeyStr8(TString8UniqueKey aKey){iKey = aKey;}
		static TBool Match(const TIntKeyStr8& aFirst, const TIntKeyStr8& aSecond)
		{
			if(aFirst.iKey == aSecond.iKey)
			{
				return ETrue;
			}
			else
			{
				return EFalse;
			}
		}


		TString8UniqueKey iKey;
		RPointerArray<HBufC8> iData;
	};

	class TIntKeyStr
	{
	public:
		TIntKeyStr(TStringUniqueKey aKey){iKey = aKey;}
		static TBool Match(const TIntKeyStr& aFirst, const TIntKeyStr& aSecond)
		{
			if(aFirst.iKey == aSecond.iKey)
			{
				return ETrue;
			}
			else
			{
				return EFalse;
			}
		}


		TStringUniqueKey iKey;
		RPointerArray<HBufC> iData;
	};

	class TIntKeyObj
	{
	public:
		TIntKeyObj(TObjectUniqueKey aKey){iKey = aKey;}
		static TBool Match(const TIntKeyObj& aFirst, const TIntKeyObj& aSecond)
		{
			if(aFirst.iKey == aSecond.iKey)
			{
				return ETrue;
			}
			else
			{
				return EFalse;
			}
		}

		TObjectUniqueKey iKey;
		RPointerArray<MUnknown> iData;
	};
	
	RArray<TIntKeyInt> 		iIntegers;
	RArray<TIntKeyInt64>	iIntegers64;
	RArray<TIntKeyStr8>		iStrings8;
	RArray<TIntKeyStr>		iStrings;
	RArray<TIntKeyObj>		iObjects;
	
	TTime iModified;
	TBool iDirty;

};
#endif // __PROPERTIES_IMPL_H__ 
