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


#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#include "Externalizable.h"
#include "Comparable.h"
#include "TypeUtilities.h"






// Interface ID.
const TInt KIID_MProperties = 0x100000F;

// Component ID for a default implementation of the interface.
const TInt KCID_MProperties = 0x10206D87;


const TInt KCID_MMailAccount = 0xA0003E26;

/**
 * These typedefs represent type-safe unique keys for different types of properties.
 *
 * To avoid collisions, the high word of the key
 * should be something chosen to be unique for a component
 * (e.g. if the component is an ECOM component,
 * the ECOM component ID should be used).
 *
 * The low word of the key can be used as needed.
 * 
 */
typedef TConstructorlessInt64<KIID_MProperties + 0x4510>	TIntUniqueKey;
typedef TConstructorlessInt64<KIID_MProperties + 0x4520>	TInt64UniqueKey;
typedef TConstructorlessInt64<KIID_MProperties + 0x4530>	TStringUniqueKey;
typedef TConstructorlessInt64<KIID_MProperties + 0x4540>	TString8UniqueKey;
typedef TConstructorlessInt64<KIID_MProperties + 0x4550>	TObjectUniqueKey;





/**
 * A generic property bag interface for setting and getting
 * different kinds of data.
 *
 * TODO: Should think seriously about changing name to not be plural for a singular object -- try MPropertyBag
 *
 * This property bag interface supports setting duplicate entries for items.
 * Setting on an existing aOrdinal replaces it.  Setting with an ordinal
 * higher than the existing number of items for a given key appends
 * (Use aOrdinal = KHugePositiveInteger to append).
 *
 */
class MProperties : public MUnknown
{
public:
	virtual TInt GetIntL(const TIntUniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	virtual void SetIntL(const TIntUniqueKey & aKey, TInt aIntegerValue, TInt aOrdinal = 0) = 0;
	virtual TBool DeleteInt(const TIntUniqueKey & aKey, TInt aOrdinal = 0) = 0;

	virtual TInt64 GetInt64L(const TInt64UniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	virtual void SetInt64L(const TInt64UniqueKey & aKey, TInt64 aIntegerValue, TInt aOrdinal = 0) = 0;
	virtual TBool DeleteInt64(const TInt64UniqueKey & aKey, TInt aOrdinal = 0) = 0;

	/**
	 * Returns a reference to the data without copying.
	 */
	virtual const TDesC8 & GetString8L(const TString8UniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	/**
	 * Returns the buffer pointed to by the string, but retains ownership.
	 */
	virtual HBufC8 * GetStringBuffer8L(const TString8UniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	/**
	 * Takes ownership of existing *aBuffer pointer.
	 */
	virtual void SetString8L(const TString8UniqueKey & aKey, HBufC8 * aBuffer, TInt aOrdinal = 0) = 0;
	/**
	 * Allocates a new copy of the data.
	 */
	virtual void SetString8L(const TString8UniqueKey & aKey, const TDesC8 & aStringValue, TInt aOrdinal = 0) = 0;
	virtual TBool DeleteString8(const TString8UniqueKey & aKey, TInt aOrdinal = 0) = 0;



	/**
	 * Returns a reference to the data without copying.
	 */
	virtual const TDesC & GetStringL(const TStringUniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	/**
	 * Returns the buffer pointed to by the string, but retains ownership.
	 */
	virtual HBufC * GetStringBufferL(const TStringUniqueKey & aKey, TInt aOrdinal = 0) const = 0;
	/**
	 * Takes ownership of existing *aBuffer pointer.
	 */
	virtual void SetStringL(const TStringUniqueKey & aKey, HBufC * aBuffer, TInt aOrdinal = 0) = 0;
	/**
	 * Allocates a new copy of the data.
	 */
	virtual void SetStringL(const TStringUniqueKey & aKey, const TDesC & aStringValue, TInt aOrdinal = 0) = 0;
	virtual TBool DeleteString(const TStringUniqueKey & aKey, TInt aOrdinal = 0) = 0;
	



	/**
	 * Retrieves an object from this MProperties object casted to the required interface.
	 *
	 * Leaves if the object is not found or if it doesn't support the specified interface.
	 *
	 * If you want to pay the cost of retrieving the object only once but
	 * want to be able to QueryInterfaceL at your leisure, then specify KIID_MUnknown 
	 * which should always succeed if the object exists.
	 *
	 * AddRef is always called on the object before it is handed out.
	 */
	virtual MUnknown * GetObjectL(const TObjectUniqueKey & aKey, TInt aInterfaceId, TInt aOrdinal = 0 ) = 0;

	/**
	 * Adds an object to this MProperties object.
	 *
	 * AddRef will be called on the object.
	 */
	virtual void SetObjectL(const TObjectUniqueKey & aKey, MUnknown * aUnknown /* IN */, TInt aOrdinal = 0 ) = 0;
	virtual TBool DeleteObject(const TObjectUniqueKey & aKey, TInt aOrdinal = 0) = 0;


	virtual void Reset() = 0;

	// For debugging.
	virtual void DebugPrintL(TInt aNestingLevel = 0) const = 0;


protected:
	virtual ~MProperties() {}
};

/**
 * Macros can often be evil, but the following macro is entirely optional
 * and provides for type safety as well more concise code.
 *
 * It works provided you follow the convention of naming your interface Id's
 * as KIID_MSomeInterface, where MSomeInterface is the name of your interface.
 *
 * e.g.
 *		MSomeInterface * someInterface = QiL( unknown, MSomeInterface );
 */
#define GoL( PROPERTIES_OBJECT, KEY, INTERFACE )	reinterpret_cast<INTERFACE*>( (PROPERTIES_OBJECT)->GetObjectL( KEY, KIID_##INTERFACE ) );



#endif  /* __PROPERTIES_H__ */
