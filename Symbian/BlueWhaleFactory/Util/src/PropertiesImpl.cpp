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


#include "DebugUtilities.h"
#include "PropertiesImpl.h"
#include "EcomPlus.h"
#include "CustomPointerContainer.h"
#include "DataAccessConnectionImpl.h"

const TInt32 KPropertiesPersistCookie = 0x58008666;
const TInt32 KPropertiesPersistVersion = 0x00000001;

CProperties::CProperties(TAny * aConstructionParameters)
: CEComPlusRefCountedBase(aConstructionParameters)
{
	DEBUG_CONSTRUCTOR
}

void CProperties::ConstructL()
{
	CEComPlusRefCountedBase::ConstructL();

}

CProperties::~CProperties()
{
	DEBUG_DESTRUCTOR
	Reset();
	
}

#ifdef __WINS__
EXPORT_C
#endif
MProperties * CProperties::NewL(TAny * aConstructionParameters)
{
	CProperties * self = new (ELeave) CProperties(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	MProperties * selfInterface = QiL( self, MProperties );
	
	CleanupStack::Pop(self);
	return selfInterface;
}

MUnknown * CProperties::QueryInterfaceL( TInt aInterfaceId)
{
	if( KIID_MProperties == aInterfaceId )
	{
		AddRef();
		return static_cast<MProperties*>(this);
	}
	else if( KIID_MExternalizable == aInterfaceId )
	{
		AddRef();
		return static_cast<MExternalizable*>(this);
	}
	else if( KIID_MComparable == aInterfaceId )
	{
		AddRef();
		return static_cast<MComparable*>(this);
	}
	else if( KIID_CInternalTAnyPointer == aInterfaceId )
	{
		AddRef();
		return reinterpret_cast<MUnknown*>((TAny*)this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}


TInt CProperties::GetIntL(const TIntUniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyInt hunter(aKey);

	TInt index = iIntegers.Find(hunter,TIntKeyInt::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return iIntegers[index].iData[aOrdinal];
}

void CProperties::SetIntL(const TIntUniqueKey & aKey, TInt aIntegerValue, TInt aOrdinal)
{
	TIntKeyInt hunter(aKey);
	TInt index = iIntegers.Find(hunter,TIntKeyInt::Match);
	if(index >= 0)
	{
		if(aOrdinal < iIntegers[index].iData.Count())
		{
			iIntegers[index].iData[aOrdinal] = aIntegerValue;
		}
		else
		{
			iIntegers[index].iData.Append(aIntegerValue);
		}
	}
	else
	{
		hunter.iData.Append(aIntegerValue);
		iIntegers.Append(hunter);
	}
	iDirty = ETrue;
}

TBool CProperties::DeleteInt(const TIntUniqueKey & aKey, TInt aOrdinal)
{
	TBool returnValue = EFalse;
	TIntKeyInt hunter(aKey);
	TInt index = iIntegers.Find(hunter,TIntKeyInt::Match);
	if(index >= 0)
	{
		if(aOrdinal < iIntegers[index].iData.Count())
		{
			iIntegers[index].iData.Remove(aOrdinal);
			if(iIntegers[index].iData.Count() == 0)
			{
				iIntegers.Remove(index);
			}
			iDirty = ETrue;
			returnValue = ETrue;
		}
	}
	return returnValue;
}


TInt64 CProperties::GetInt64L(const TInt64UniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyInt64 hunter(aKey);

	TInt index = iIntegers64.Find(hunter,TIntKeyInt64::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return iIntegers64[index].iData[aOrdinal];

}

void CProperties::SetInt64L(const TInt64UniqueKey & aKey,  TInt64 aIntegerValue, TInt aOrdinal)
{
	TIntKeyInt64 hunter(aKey);
	TInt index = iIntegers64.Find(hunter,TIntKeyInt64::Match);
	if(index >= 0)
	{
		if(aOrdinal < iIntegers64[index].iData.Count())
		{
			iIntegers64[index].iData[aOrdinal] = aIntegerValue;
		}
		else
		{
			iIntegers64[index].iData.Append(aIntegerValue);
		}
	}
	else
	{
hunter.iData.Append(aIntegerValue);
		iIntegers64.Append(hunter);
	}
	iDirty = ETrue;
}

TBool CProperties::DeleteInt64(const TInt64UniqueKey & aKey, TInt aOrdinal)
{
	TBool returnValue = EFalse;
	TIntKeyInt64 hunter(aKey);
	TInt index = iIntegers64.Find(hunter,TIntKeyInt64::Match);
	if(index >= 0)
	{
		if(aOrdinal < iIntegers64[index].iData.Count())
		{
			iIntegers64[index].iData.Remove(aOrdinal);
			if(iIntegers64[index].iData.Count() == 0)
			{
				iIntegers64.Remove(index);
			}
			iDirty = ETrue;
			returnValue = ETrue;
		}
	}
	return returnValue;
}


const TDesC8 & CProperties::GetString8L(const TString8UniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyStr8 hunter(aKey);

	TInt index = iStrings8.Find(hunter,TIntKeyStr8::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return *(iStrings8[index].iData[aOrdinal]);
}

HBufC8 * CProperties::GetStringBuffer8L(const TString8UniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyStr8 hunter(aKey);

	TInt index = iStrings8.Find(hunter,TIntKeyStr8::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return iStrings8[index].iData[aOrdinal];
}


void CProperties::SetString8L(const TString8UniqueKey & aKey, HBufC8 * aBuffer, TInt aOrdinal)
{
	TIntKeyStr8 hunter(aKey);
	TInt index = iStrings8.Find(hunter,TIntKeyStr8::Match);
	if(index >= 0)
	{
		if(aOrdinal < iStrings8[index].iData.Count())
		{
			iStrings8[index].iData[aOrdinal] = aBuffer;
		}
		else
		{
			User::LeaveIfError(iStrings8[index].iData.Append(aBuffer));
		}
	}
	else
	{
		User::LeaveIfError(hunter.iData.Append(aBuffer));
		User::LeaveIfError(iStrings8.Append(hunter));
	}
	iDirty = ETrue;
}

void CProperties::SetString8L(const TString8UniqueKey & aKey, const TDesC8 & aStringValue, TInt aOrdinal)
{
	HBufC8 * buffer = aStringValue.AllocLC();

	SetString8L(aKey,buffer,aOrdinal);
	
	CleanupStack::Pop(buffer);
	iDirty = ETrue;
}

TBool CProperties::DeleteString8(const TString8UniqueKey & aKey, TInt aOrdinal)
{
	TBool returnValue = EFalse;
	TIntKeyStr8 hunter(aKey);
	TInt index = iStrings8.Find(hunter,TIntKeyStr8::Match);
	if(index >= 0)
	{
		if(aOrdinal < iStrings8[index].iData.Count())
		{
			delete iStrings8[index].iData[aOrdinal];
			iStrings8[index].iData.Remove(aOrdinal);

			if(iStrings8[index].iData.Count() == 0)
			{
				iStrings8[index].iData.Reset();
				iStrings8.Remove(index);
			}
			iDirty = ETrue;
			returnValue = ETrue;
		}
	}
	return returnValue;
}

const TDesC & CProperties::GetStringL(const TStringUniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyStr hunter(aKey);

	TInt index = iStrings.Find(hunter,TIntKeyStr::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return *(iStrings[index].iData[aOrdinal]);
}

HBufC * CProperties::GetStringBufferL(const TStringUniqueKey & aKey, TInt aOrdinal) const
{
	TIntKeyStr hunter(aKey);

	TInt index = iStrings.Find(hunter,TIntKeyStr::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	return iStrings[index].iData[aOrdinal];
}


void CProperties::SetStringL(const TStringUniqueKey & aKey, HBufC * aBuffer, TInt aOrdinal)
{
	TIntKeyStr hunter(aKey);
	TInt index = iStrings.Find(hunter,TIntKeyStr::Match);
	if(index >= 0)
	{
		if(aOrdinal < iStrings[index].iData.Count())
		{
			iStrings[index].iData[aOrdinal] = aBuffer;
		}
		else
		{
			iStrings[index].iData.Append(aBuffer);
		}
	}
	else
	{
		hunter.iData.Append(aBuffer);
		iStrings.Append(hunter);
	}
	iDirty = ETrue;
}

void CProperties::SetStringL(const TStringUniqueKey & aKey, const TDesC & aStringValue, TInt aOrdinal)
{
	HBufC * buffer = aStringValue.AllocL();
	CleanupStack::PushL(buffer);
	
	SetStringL(aKey,buffer,aOrdinal);
	
	CleanupStack::Pop(buffer);
	iDirty = ETrue;
}

TBool CProperties::DeleteString(const TStringUniqueKey & aKey, TInt aOrdinal)
{
	TBool returnValue = EFalse;
	TIntKeyStr hunter(aKey);
	TInt index = iStrings.Find(hunter,TIntKeyStr::Match);
	if(index >= 0)
	{
		if(aOrdinal < iStrings[index].iData.Count())
		{
			delete iStrings[index].iData[aOrdinal];
			iStrings[index].iData.Remove(aOrdinal);

			if(iStrings[index].iData.Count() == 0)
			{
				iStrings[index].iData.Reset();
				iStrings.Remove(index);
			}
			iDirty = ETrue;
			returnValue = ETrue;
		}
	}
	return returnValue;
}

void CProperties::SetObjectL(const TObjectUniqueKey & aKey, MUnknown * aUnknown /* IN */, TInt aOrdinal )
{
	TIntKeyObj hunter(aKey);
	TInt index = iObjects.Find(hunter,TIntKeyObj::Match);
	if(index >= 0)
	{
		if(aOrdinal < iObjects[index].iData.Count())
		{
			iObjects[index].iData[aOrdinal] = aUnknown;
		}
		else
		{
			iObjects[index].iData.Append(aUnknown);
		}
	}
	else
	{
		hunter.iData.Append(aUnknown);
		iObjects.Append(hunter);
	}
	// Call AddRef so object stays around even if caller who added it goes away.
	// We call Release in our destructor.
	aUnknown->AddRef();
	iDirty = ETrue;
}

MUnknown * CProperties::GetObjectL(const TObjectUniqueKey & aKey, TInt aInterfaceId, TInt aOrdinal )
{
	MUnknown * unknown = NULL;

	TIntKeyObj hunter(aKey);

	TInt index = iObjects.Find(hunter,TIntKeyObj::Match);
	if(index < 0)
	{
		User::Leave(KErrNotFound);
	}
	unknown = iObjects[index].iData[aOrdinal];

	return unknown->QueryInterfaceL( aInterfaceId );
}

TBool CProperties::DeleteObject(const TObjectUniqueKey & aKey, TInt aOrdinal)
{
	TBool returnValue = EFalse;
	TIntKeyObj hunter(aKey);
	TInt index = iObjects.Find(hunter,TIntKeyObj::Match);
	if(index >= 0)
	{
		if(aOrdinal < iObjects[index].iData.Count())
		{
			iObjects[index].iData[aOrdinal]->Release();
			iObjects[index].iData.Remove(aOrdinal);

			if(iObjects[index].iData.Count() == 0)
			{
				iObjects.Remove(index);
			}
			iDirty = ETrue;
			returnValue = ETrue;
		}
	}
	return returnValue;
}

TUid CProperties::GetECOMPlusComponentId() const
{
	return TUid::Uid(KCID_MProperties);
}

void CProperties::ExternalizeL(RWriteStream& aStream) const
{
	TInt count = iIntegers.Count();
	aStream.WriteUint32L(count);
	for(TInt i=0;i<count;i++)
	{
		aStream.WriteUint32L(iIntegers[i].iKey.High());
		aStream.WriteUint32L(iIntegers[i].iKey.Low());
		TInt subCount = iIntegers[i].iData.Count();
		aStream.WriteUint32L(subCount);
		for(TInt j=0;j<subCount;j++)
		{
			aStream.WriteInt32L(iIntegers[i].iData[j]);
		}
	}
	count = iIntegers64.Count();
	aStream.WriteUint32L(count);
	for(TInt i=0;i<count;i++)
	{
		aStream.WriteUint32L(iIntegers64[i].iKey.High());
		aStream.WriteUint32L(iIntegers64[i].iKey.Low());
		TInt subCount = iIntegers64[i].iData.Count();
		aStream.WriteUint32L(subCount);
		for(TInt j=0;j<subCount;j++)
		{
			aStream.WriteUint32L(MY64HIGH(iIntegers64[i].iData[j]));
			aStream.WriteUint32L(MY64LOW(iIntegers64[i].iData[j]));
		}
	}
	count = iStrings8.Count();
	aStream.WriteUint32L(count);
	for(TInt i=0;i<count;i++)
	{
		aStream.WriteUint32L(iStrings8[i].iKey.High());
		aStream.WriteUint32L(iStrings8[i].iKey.Low());
		TInt subCount = iStrings8[i].iData.Count();
		aStream.WriteUint32L(subCount);
		for(TInt j=0;j<subCount;j++)
		{
			aStream << *iStrings8[i].iData[j];
		}
	}
	count = iStrings.Count();
	aStream.WriteUint32L(count);
	for(TInt i=0;i<count;i++)
	{
		aStream.WriteUint32L(iStrings[i].iKey.High());
		aStream.WriteUint32L(iStrings[i].iKey.Low());
		TInt subCount = iStrings[i].iData.Count();
		aStream.WriteUint32L(subCount);
		for(TInt j=0;j<subCount;j++)
		{
			aStream << *iStrings[i].iData[j];
		}
	}
	RArray<RArray<TInt> > saveable;
	TInt saveCount = 0;
	MExternalizable* ext;
	count = iObjects.Count();
	for(TInt i=0;i<count;i++)
	{
		TInt subCount = iObjects[i].iData.Count();
		RArray<TInt> subObjects;
		for(TInt j=0;j<subCount;j++)
		{
			TRAPD(extError,ext = QiL(iObjects[i].iData[j],MExternalizable));
			if(extError == KErrNone)
			{
				subObjects.Append(j);
				ext->Release();
			}
		}
		saveable.Append(subObjects);
		subObjects.Reset();
	}
	for(TInt i=0;i<count;i++)
	{
		if(saveable[i].Count() > 0)
		{
			saveCount++;
		}
	}
	aStream.WriteUint32L(saveCount);
	for(TInt i=0;i<count;i++)
	{
		TInt subCount = saveable[i].Count();
		if(subCount > 0)
		{
			aStream.WriteUint32L(iObjects[i].iKey.High());
			aStream.WriteUint32L(iObjects[i].iKey.Low());
			aStream.WriteUint32L(subCount);
			for(TInt j=0;j<subCount;j++)
			{
				TRAPD(extError,ext = QiL(iObjects[i].iData[j],MExternalizable));
				if(extError == KErrNone)
				{
					CleanupReleasePushL(*ext);
					aStream.WriteUint32L(ext->GetECOMPlusComponentId().iUid);
					ext->ExternalizeL(aStream);
					CleanupStack::PopAndDestroy(ext);
				}	
			}
		}
	}
	saveable.Reset();
		
}

void CProperties::InternalizeL(RReadStream& aStream)
{
	Reset();
	TInt count = aStream.ReadUint32L();
	for(TInt i=0;i<count;i++)
	{
		TUint32 high = aStream.ReadUint32L();
		TUint32 low = aStream.ReadUint32L();
		TIntUniqueKey integerKey = {high,low};
		TIntKeyInt newInteger(integerKey);
		TInt subcount = aStream.ReadUint32L();
		for(TInt j=0;j<subcount;j++)
		{
			TInt data = aStream.ReadInt32L();
			newInteger.iData.Append(data);
		}
		iIntegers.Append(newInteger);
	}
	count = aStream.ReadUint32L();
	for(TInt i=0;i<count;i++)
	{
		TUint32 high = aStream.ReadUint32L();
		TUint32 low = aStream.ReadUint32L();
		TInt64UniqueKey integerKey64 = {high,low};
		TIntKeyInt64 newInteger64(integerKey64);
		TInt subcount = aStream.ReadUint32L();
		for(TInt j=0;j<subcount;j++)
		{
			TInt dataHigh = aStream.ReadInt32L();
			TInt dataLow = aStream.ReadInt32L();
		#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
			TInt64 data = MAKE_TINT64(dataHigh, dataLow);
		#else // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
			TInt64 data(dataHigh, dataLow);
		#endif // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	
			newInteger64.iData.Append(data);
		}
		iIntegers64.Append(newInteger64);
	}
	
	count = aStream.ReadUint32L();
	for(TInt i=0;i<count;i++)
	{
		TUint32 high = aStream.ReadUint32L();
		TUint32 low = aStream.ReadUint32L();
		TString8UniqueKey stringKey = {high,low};
		TIntKeyStr8 newString(stringKey);
		TInt subcount = aStream.ReadUint32L();
		for(TInt j=0;j<subcount;j++)
		{
			HBufC8* str = HBufC8::NewL(aStream,256);
			newString.iData.Append(str);
		}
		iStrings8.Append(newString);
	}
	count = aStream.ReadUint32L();
	for(TInt i=0;i<count;i++)
	{
		TUint32 high = aStream.ReadUint32L();
		TUint32 low = aStream.ReadUint32L();
		TStringUniqueKey stringKey = {high,low};
		TIntKeyStr newString(stringKey);
		TInt subcount = aStream.ReadUint32L();
		for(TInt j=0;j<subcount;j++)
		{
			HBufC* str = HBufC::NewL(aStream,256);
			newString.iData.Append(str);
		}
		iStrings.Append(newString);
	}
	count = aStream.ReadUint32L();
	for(TInt i=0;i<count;i++)
	{
		TUint32 high = aStream.ReadUint32L();
		TUint32 low = aStream.ReadUint32L();
		TObjectUniqueKey objectKey = {high,low};
		TIntKeyObj newObject(objectKey);
		TInt subcount = aStream.ReadUint32L();
		for(TInt j=0;j<subcount;j++)
		{
			TUint objectId = aStream.ReadUint32L();
			MExternalizable* ext = static_cast<MExternalizable*> ( REComPlusSession::CreateImplementationL(TUid::Uid(objectId), TUid::Uid(KIID_MExternalizable), NULL) );
			CleanupReleasePushL(*ext);
			ext->InternalizeL(aStream);
			User::LeaveIfError(newObject.iData.Append(ext));
			CleanupStack::Pop(ext);
		}
		iObjects.Append(newObject);
	}
}


TInt operator==( HBufC8 & aBuffer1, HBufC8 & aBuffer2 )
{
	TPtr8 ptr1 = aBuffer1.Des();
	TPtr8 ptr2 = aBuffer2.Des();

	return !( ptr1.Compare( ptr2 ) );
}


TInt CProperties::Compare( MUnknown * aOther ) const
{
	// At the moment, the only way we can compare ourselves to another
	// component is if it is the same implementation as ourselves.
	// To establish this, check its ECOMPlusComponentId.
	MExternalizable * otherExternalizable = 0;
	TRAPD(queryError, otherExternalizable = QiL( aOther, MExternalizable ));
	if( KErrNone != queryError )
	{
		return 1;
	}

	// Note that we call our own virtual GetECOMPlusComponentId method
	// as well, so that if we are a base class implementation in some 
	// derived class, this will still work.
	TUid ourUid(GetECOMPlusComponentId());
	TUid otherUid(otherExternalizable->GetECOMPlusComponentId());

	otherExternalizable->Release();

	if( ourUid.iUid != otherUid.iUid )
	{
		// Different -- it's not us.
		return (ourUid.iUid < otherUid.iUid ? -1 : 1);
	}


	CProperties * other = 0;

	// The following is safe since we know the implementation is our own.
	TRAPD( ignoreError, other = reinterpret_cast<CProperties*>( aOther->QueryInterfaceL(KIID_CInternalTAnyPointer) ) );
	// Restore the object refcount to what it was before the previous line.
	// Don't worry, it isn't going anywhere.
	other->Release();
	
	TInt comparison = 0;
#if 0
	if( 0 != (comparison = iIntegers->Compare( other->iIntegers ) ) )
	{
		// Different.
		return comparison;
	}

	if( 0 != (comparison = iIntegers64->Compare( other->iIntegers64 ) ) )
	{
		// Different.
		return comparison;
	}

	if( 0 != (comparison = iStrings->Compare(  other->iStrings ) ) )
	{
		// Different.
		return comparison;
	}
	
	if( 0 != (comparison = iStrings8->Compare(  other->iStrings8 ) ) )
	{
		// Different.
		return comparison;
	}

	if( 0 != (comparison = iObjects->Compare(  other->iObjects ) ) )
	{
		// Different.
		return comparison;
	}
#endif
	// Same.
	return comparison;
}


void CProperties::Reset()
{
	// it's possible to het here with a partially created
	// found when low mem testing
	TInt count = iIntegers.Count();
	for(TInt i=0;i<count;i++)
	{
		iIntegers[i].iData.Reset();
	}
	iIntegers.Reset();

	count = iIntegers64.Count();
	for(TInt i=0;i<count;i++)
	{
		iIntegers64[i].iData.Reset();
	}
	iIntegers64.Reset();
	
	count = iStrings.Count();
	for(TInt i=0;i<count;i++)
	{
		TInt subcount = iStrings[i].iData.Count();
		for(TInt j=0;j<subcount;j++)
		{
			delete iStrings[i].iData[j];
		}
		iStrings[i].iData.Reset();
	}
	iStrings.Reset();
	
	count = iStrings8.Count();
	for(TInt i=0;i<count;i++)
	{
		TInt subcount = iStrings8[i].iData.Count();
		for(TInt j=0;j<subcount;j++)
		{
			delete iStrings8[i].iData[j];
		}
		iStrings8[i].iData.Reset();
	}
	iStrings8.Reset();
	
	count = iObjects.Count();
	for(TInt i=0;i<count;i++)
	{
		TInt subcount = iObjects[i].iData.Count();
		for(TInt j=0;j<subcount;j++)
		{
			iObjects[i].iData[j]->Release();
		}
		iObjects[i].iData.Reset();
	}

	iObjects.Reset();
	
	iDirty = EFalse; // Bit of a philosophical issue here.

}

void CProperties::DebugPrintL(TInt aNestingLevel) const
{
	TInt sizeNeeded = 2 * aNestingLevel; // Minimum needed for our text.
	HBufC * nesting = HBufC::NewLC(sizeNeeded);
	TPtr ptr = nesting->Des();
	for( TInt i = 0; i < aNestingLevel; ++i )
	{
		ptr.Append(_L("  "));
	}

	if( iDirty )
	{
		RDebug::Print(_L("%SProperties [%x]: (dirty)"), nesting, this );
	}
	else
	{
		RDebug::Print(_L("%SProperties [%x]: (not dirty)"), nesting, this );
	}

	RDebug::Print(_L("%S Integers (%d)"), nesting, iIntegers.Count());
/*	MIterator< TPair<TIntUniqueKey,TInt> > * intIterator = iIntegers->IteratorLC();
	while( intIterator->HasNext() )
	{
		TPair<TIntUniqueKey, TInt> intPair = intIterator->NextL();
		RDebug::Print(_L("%S Key[%d,%d]: %d"), nesting, intPair.Key().High(), intPair.Key().Low(), intPair.Data());
	}
	CleanupStack::PopAndDestroy(intIterator);
*/
	
	RDebug::Print(_L("%S Integers64 (%d)"), nesting, iIntegers64.Count());
/*	MIterator< TPair<TInt64UniqueKey,TInt64> > * int64Iterator = iIntegers64->IteratorLC();
	while( int64Iterator->HasNext() )
	{
		TPair<TInt64UniqueKey, TInt64> int64Pair = int64Iterator->NextL();
		RDebug::Print(_L("%S  Key[%d,%d]: %d,%d"), nesting,int64Pair.Key(), int64Pair.Key(), MY64HIGH(int64Pair.Data()), MY64LOW(int64Pair.Data()));
	}
	CleanupStack::PopAndDestroy(int64Iterator);
*/
	
	RDebug::Print(_L("%S Strings (%d)"), nesting, iStrings.Count() );
/*	MIterator< TPair<TStringUniqueKey,HBufC*> > * iterator = iStrings->IteratorLC();
	while( iterator->HasNext() )
	{
		TPair<TStringUniqueKey, HBufC *> pair = iterator->NextL(); 
		RDebug::Print(_L("%S  Key[%d,%d]: (%d)"), nesting, pair.Key().High(), pair.Key().Low(), (pair.Data())->Length());
		DebugPrint(aNestingLevel + 3, *(pair.Data()) );
	}
	CleanupStack::PopAndDestroy(iterator);
*/
	
	RDebug::Print(_L("%S Strings8 (%d)"), nesting, iStrings8.Count() );
/*	MIterator< TPair<TString8UniqueKey,HBufC8*> > * iterator8 = iStrings8->IteratorLC();
	while( iterator8->HasNext() )
	{
		TPair<TString8UniqueKey, HBufC8 *> pair8 = iterator8->NextL();
		RDebug::Print(_L("%S  Key[%d,%d]: (%d)"), nesting, pair8.Key().High(), pair8.Key().Low(), (pair8.Data())->Length());
		DebugPrint(aNestingLevel + 3, *(pair8.Data()) );
	}
	CleanupStack::PopAndDestroy(iterator8);
*/
	RDebug::Print(_L("%S Objects (%d)"), nesting, iObjects.Count() );
/*	MIterator< TPair<TObjectUniqueKey,MUnknown*> > * objectIterator = iObjects->IteratorLC();
	while( objectIterator->HasNext() )
	{
		TPair<TObjectUniqueKey, MUnknown *> pair = objectIterator->NextL();
		MUnknown * externalizable = pair.Data();
		MProperties * properties = 0;
		TRAPD(error, properties = QiL( externalizable, MProperties ) );
		if( KErrNone == error )
		{
			RDebug::Print(_L("%S  Key[%d,%d]:"), nesting, pair.Key().High(), pair.Key().Low());
			CleanupReleasePushL(*properties);
			properties->DebugPrintL(aNestingLevel + 2);
			CleanupStack::PopAndDestroy(properties);
		}
		else
		{
			RDebug::Print(_L("%S  Key[%d,%d]: Object could not be printed."), nesting, pair.Key().High(), pair.Key().Low());
		}
	}
	CleanupStack::PopAndDestroy(objectIterator);
*/
	CleanupStack::PopAndDestroy(nesting);
}


const TTime & CProperties::GetModified() const
{
	return iModified;
}

void CProperties::SetModified(const TTime & aModified )
{
	iModified = aModified;
}

TBool CProperties::Dirty() const
{
	return iDirty;
}

void CProperties::SetDirty(TBool aDirty)
{
	iDirty = aDirty;
}
