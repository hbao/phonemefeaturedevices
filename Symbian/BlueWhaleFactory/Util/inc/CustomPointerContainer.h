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


#ifndef __CUSTOM_POINTER_CONTAINER_H__
#define __CUSTOM_POINTER_CONTAINER_H__


#include "RefCountedBase.h"
#include "DestructorFunctor.h"
#include "DebugUtilities.h"

/**
 * This is a special interface pointer which some objects may support
 * to return a (TAny *) cast of their this pointer.
 *
 * It is used in some cases like MComparable implemetations where we
 * which to get a pointer the object itself.
 *
 * NOT for general use.  Be sure you understand the risks of using this.
 */
const TInt KIID_CInternalTAnyPointer = 0x66643666;



/**
 * Sometimes we want to add plain old lightweight C++ object pointers
 * to things like MProperties, without wanting to pay the price of 
 * adding a v-table, QueryInterfaceL, AddRef and Release to those objects.
 *
 * We can use this templated class to do so.  To external callers it will
 * look like a normal, well-behaved MUnknown implementation.
 *
 * Internally it maintains a correctly-typed pointer the required object.
 *
 * To retrieve values, QueryInterfaceL for the KIID_CInternalTAnyPointer
 * pointer and cast to the type you MUST KNOW the pointer will be.
 */
template <class MYCLASS>
class CCustomPointerContainer : public CRefCountedBase
{
public:
	CCustomPointerContainer(MYCLASS * aMyClass = 0)
		: CRefCountedBase(0)
		, iMyClass(aMyClass)
	{
		DEBUG_CONSTRUCTOR
		AddRef();
	}
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId )
	{
		if( KIID_CInternalTAnyPointer == aInterfaceId )
		{
			AddRef();
			return reinterpret_cast<MUnknown*>((TAny*)this);
		}
		else
		{
			return CRefCountedBase::QueryInterfaceL(aInterfaceId);
		}
	}
	void AddRef()
	{
		CRefCountedBase::AddRef();
	}
	void Release()
	{
		CRefCountedBase::Release();
	}
	MYCLASS * GetPointer() const
	{
		return iMyClass;
	}
	void SetPointer(MYCLASS * aPointer)
	{
		// Don't Reset -- allow clients to do this themselves using the Reset method.
		// Reset();


		iMyClass = aPointer;
	}
	void Reset()
	{
		if( iMyClass )
		{
			TDestructorFunctor<MYCLASS *>::Destroy( iMyClass );
			iMyClass = 0;
		}
	}

protected:
	virtual ~CCustomPointerContainer()
	{
		DEBUG_DESTRUCTOR
		Reset();
	}

	MYCLASS * iMyClass;
};

#endif /* __CUSTOM_POINTER_CONTAINER_H__ */
