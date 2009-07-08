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


#include <e32std.h>
#include <e32svr.h>
#include "unknown.h"
#include "BlankObject.h"

MUnknown * CBlankObject::NewL( TAny * aConstructionParameters )
{
	CBlankObject* self = new (ELeave) CBlankObject(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;
}

CBlankObject::CBlankObject(TAny * aConstructionParameters) : CEComPlusRefCountedBase(aConstructionParameters)
{
	RDebug::Print(_L("CBlankObject"));
}

CBlankObject::~CBlankObject()
{}

MUnknown * CBlankObject::QueryInterfaceL( TInt aInterfaceId )
{	
	RDebug::Print(_L("CBlankObject::QueryInterfaceL 0x%08x"),aInterfaceId);
	if( KIID_MBlankObject == aInterfaceId )
	{
		AddRef();
		return static_cast<MBlankObject*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}
