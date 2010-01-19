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


#ifndef __PAIR_H__
#define __PAIR_H__

#include "eikmenup.h"


/**
 * A comparison function which can be used on TPair (or other types)
 * and is needed by the THashtable class.
 *
 * Should return 0 when items are equal.
 */
template <class DATA> class TComparator
{
public:
	/**
	 * Used for ordering, should return:
	 *
	 *		aOne <  aOther	==>		-1
	 *		aOne == aOther	==>		 0
	 *		aOne >  aOther	==>		 1
	 *
	 * This implementation doesn't do much -- implement a template specialization 
	 * of this method for your own DATA type to implement the correct comparison.
	 */
	static TInt Compare( DATA aOne, DATA aOther )
	{
		if( aOne < aOther )
		{
			return -1;
		}
		else if( aOne == aOther )
		{
			return 0;
		}
		else /* aOne > aOther */
		{
			return 1;
		}
	}
};

#ifdef __GCCE__
template class TComparator<HBufC8 *>;
template class TComparator<HBufC *>;
template class TComparator<MUnknown *>;
template class TComparator<CEikMenuPaneItem *>;
#endif


TInt TComparator<CEikMenuPaneItem *>::Compare( CEikMenuPaneItem * aOne, CEikMenuPaneItem * aOther )
{
	TInt firstCommandId = aOne->iData.iCommandId;
	TInt secondCommandId = aOther->iData.iCommandId;
	if( firstCommandId > secondCommandId )
	{
		return 1;
	}
	else if( firstCommandId == secondCommandId )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
/**
 * We've chose to use folded (i.e. case insensitive) comparison.
 */
TInt TComparator<HBufC8 *>::Compare( HBufC8 * aOne, HBufC8 * aOther)
{
	if( aOne && aOther)
	{
		return( (*aOne).CompareF( *aOther) );
	}
	else 
	{
		if( aOne )
		{
			// aOne is non-null so we'll say it's "greater" than aOther.
			return 1;
		}
		else 
		{
			// aOne is null but aOther isn't so we'll say aOne is "less" than aOther.
			return -1;
		}
	}
}


/**
 * We've chose to use folded (i.e. case insensitive) comparison.
 */
TInt TComparator<HBufC *>::Compare( HBufC * aOne, HBufC * aOther)
{
	if( aOne && aOther)
	{
		return( (*aOne).CompareF( *aOther) );
	}
	else 
	{
		if( aOne )
		{
			// aOne is non-null so we'll say it's "greater" than aOther.
			return 1;
		}
		else 
		{
			// aOne is null but aOther isn't so we'll say aOne is "less" than aOther.
			return -1;
		}
	}
}

template <class KEY, class DATA> class TPair
{
public:
	/**
	 * Note: these are not const & here because we actually mean
	 * to make copies of the data passed in to this constructor.
	 */
	TPair( KEY aKey, DATA aData )
		: iKey(aKey), iData(aData)
	{
	}

	/**
	 * Add a template specialization here if you don't like 
	 * the default comparison, for example if your DATA is
	 * a pointer class, the implementation below will be pointer 
	 * comparison only.  You may want to do something like e.g.
	 * compare strings the pointers point to.
	 */
	TInt operator==(const TPair & aOther) const
	{
		return( (0 == TComparator<KEY>::Compare(iKey,aOther.iKey)) && ( 0 == TComparator<DATA>::Compare(iData,aOther.iData) ) );
	}

	TPair & operator=(const TPair & aOther)
	{
		iKey = aOther.iKey;
		iData = aOther.iData;
		return( *this );
	}
	KEY Key() const
	{
		return iKey;
	}
	DATA Data() const
	{
		return iData;
	}
protected:
	KEY iKey;
	DATA iData;
};



#endif /* __PAIR_H__ */
