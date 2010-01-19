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


#ifndef __ECOMPLUS_REF_COUNTED_TIMER_H__
#define __ECOMPLUS_REF_COUNTED_TIMER_H__

#include "TracksOwnDestructorKey.h"
#include "EcomPlus.h"

/**
 * An OPTIONAL inline base class which derives from CTimer and which
 * implements MUnknown, MTracksOwnDestructorKey and reference counting.
 */
class CEComPlusRefCountedTimer : public CTimer, public MTracksOwnDestructorKey
{
public:
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId )
	{
		if( KIID_MTracksOwnDestructorKey == aInterfaceId )
		{
			AddRef(); // Remember to do this in your subclasses for every QI.
			return static_cast<MTracksOwnDestructorKey*>( this );
		}
		else if( KIID_MUnknown == aInterfaceId )
		{
			AddRef(); // Remember to do this in your subclasses for every QI.
			return static_cast<MUnknown*>( this );
		}
		else
		{
			User::Leave(KErrNotSupported);
			return 0;
		}
	}

	virtual void AddRef() 
	{
		++iRefCount;
	}

	virtual void Release()
	{
		--iRefCount;
		if( 0 == iRefCount )
		{
			delete this;
		}
	}

	virtual void SetDestructorKey( TUid aDtorIDKey /* IN */ )
	{
		iDtorIDKey = aDtorIDKey;
	}

protected:
	// TODO: Think about how we'd like to pass in aConstructionParameters
	// using some standard T-class so that we can pass TPriority in here.
	CEComPlusRefCountedTimer( TPriority aPriority = EPriorityStandard )
		: CTimer( aPriority )
	{
	}

	void ConstructL()
	{
		CTimer::ConstructL();
	}

	virtual ~CEComPlusRefCountedTimer() 
	{
		Cancel();
		REComPlusSession::DestroyedImplementation(iDtorIDKey);
	}

private:
	TInt iRefCount;
	TUid iDtorIDKey;	
};


#endif /* __ECOMPLUS_REF_COUNTED_TIMER_H__ */

