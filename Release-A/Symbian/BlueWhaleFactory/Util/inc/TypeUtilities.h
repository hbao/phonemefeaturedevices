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


#ifndef __TYPE_UTILITIES_H__
#define __TYPE_UTILITIES_H__

#include <s32strm.h>
#include <OSVersion.h>

const TInt KHugePositiveInteger = KMaxTInt32;
const TInt KHugeNegativeInteger = KMinTInt32;


const TInt KHugeHBufCReadMaxNumber = KHugePositiveInteger;

/**
 * In various places, we wish to allow for the extensible 
 * addition of new and unique constants.  In order
 * to minimize the possibility of collisions between
 * different 3rd party implementors who may extend 
 * our framework, we use 64-bit quanties which 
 * consist of 32-bit high word usually drawn from a 
 * component or interface id (which has its uniquess
 * guaranteed by the usual Symbian UID allocation scheme)
 * and a 32-but low word which can be defined by the implementor.
 *
 * TInt64 does not serve our needs, because it has a constructor
 * and thus it is not possible to declare one const
 * without getting ARM unitialized data errors.
 *
 * To allow us to declare const 64-bit values without
 * getting ARM uninitialized data errors, we need
 * a 64-bit int class which has no constructor.
 * 
 * Then we can declare constants of the class using
 * the format:
 * 
 *		const TConstructorlessInt64<some unique dummy number for type safety> KSome64BitConstant = { 0x12312312, 0x92342313 };
 *
 * without getting any uninitialized data warnings.
 *
 * We have templated the class with an otherwise unused integer
 * so that different users of this class can obtain type safety.
 * For instance, TConstructorlessInt64<10>
 * is a different class than TConstructorlessInt64<20>
 * so that you can't accidentally pass one when you mean the other.
 *
 */
template<TInt TYPE_DIFFERENTIATOR_UNIQUE_DUMMY_NUMBER>


#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

// i.e. S60v3 and all new SDK's moving forward.
// TInt64 is a base language type.
#define MY64HIGH( aInt64 )	I64HIGH(aInt64)
#define MY64LOW( aInt64 )	I64LOW(aInt64)

#else // older

// TInt64 was a C++ class.
#define MY64HIGH( aInt64 )	((aInt64).High())
#define MY64LOW( aInt64 )	((aInt64).Low())

#endif // (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION__ >= __UIQ_V3_FP0_VERSION_NUMBER__)


struct TConstructorlessInt64
{
	void Set( TInt32 aUniqueComponentIdentifier, TInt32 aStateUniqueToComponent)
	{
		iHigh = aUniqueComponentIdentifier;
		iLow = aStateUniqueToComponent;
	}
	void Set( const TInt64 & aValue)
	{
		iHigh = MY64HIGH(aValue);
		iLow =  MY64LOW(aValue);
	}
	TInt32 High() const
	{
		return iHigh;
	}
	TInt32 Low() const
	{
		return iLow;
	}
	TInt64 Int64() const
	{
	#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__)  || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		return MAKE_TINT64(iHigh, iLow);
	#else // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt64 temp(iHigh, iLow);
		return temp;
	#endif // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	}
	TInt operator!=(const TConstructorlessInt64 & aOther) const
	{
		return Int64() != aOther.Int64();
	}
	TInt operator==(const TConstructorlessInt64 & aOther) const
	{
		return Int64() == aOther.Int64();
	}
	TInt operator<(const TConstructorlessInt64 & aOther) const
	{
		return Int64() < aOther.Int64();
	}

	void ExternalizeL( RWriteStream & aStream ) const
	{
		aStream << iHigh;
		aStream << iLow;
	}
	void InternalizeL( RReadStream & aStream )
	{
		aStream >> iHigh;
		aStream >> iLow;
	}
	TInt32 iHigh;
	TInt32 iLow;
};


void inline TRArrayTIntCleanupOperation(TAny* aPointer)
{
	RArray<TInt> * array = reinterpret_cast< RArray<TInt> * >(aPointer);
	array->Reset();
	delete array;
}


#endif /* __TYPE_UTILITIES_H__ */
