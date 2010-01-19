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

 

#include "TimeOutTimerImpl.h"
#include "DebugUtilities.h"



MUnknown * CTimeOutTimer::NewL(TAny * /* aConstructionParameters */)
{
	CTimeOutTimer* self = new (ELeave) CTimeOutTimer(EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;
}

CTimeOutTimer::CTimeOutTimer(TPriority aPriority)
: CEComPlusRefCountedTimer(aPriority)
{
	DEBUG_CONSTRUCTOR
}

void CTimeOutTimer::ConstructL()
{
	CEComPlusRefCountedTimer::ConstructL();
	CActiveScheduler::Add(this);
}


CTimeOutTimer::~CTimeOutTimer()
{
	DEBUG_DESTRUCTOR

	Reset();
}


MUnknown * CTimeOutTimer::QueryInterfaceL( TInt aInterfaceId )
{
	if( KIID_MTimeOutTimer == aInterfaceId )
	{
		AddRef();
		return static_cast<MTimeOutTimer*>(this);
	}
	else
	{
		return CEComPlusRefCountedTimer::QueryInterfaceL(aInterfaceId);
	}
}

void CTimeOutTimer::SetCallback( MTimeOutTimerCallback * aTimeOutCallback, TAny * aCookie, CActive::TPriority aPriority )
{
	Cancel();
	iTimeOutCallback = aTimeOutCallback;
	iCookie = aCookie;
	SetPriority(aPriority);
}


void CTimeOutTimer::At(const TTime& aTime)
{
	Cancel();
	CTimer::At(aTime);
}


void CTimeOutTimer::AfterL(TTimeIntervalSeconds anInterval)
{
	if( (anInterval.Int() > 0 ) && (anInterval.Int() <= 2147) )
	{
		Cancel();
		CTimer::After(TTimeIntervalMicroSeconds32( anInterval.Int() * 1000000) );
	}
	else
	{
		User::Leave(KErrArgument);
	}
}

void CTimeOutTimer::Inactivity(TTimeIntervalSeconds anInterval )
{
	Cancel();
	CTimer::Inactivity(anInterval);
}


void CTimeOutTimer::Reset()
{
	Cancel();
	CTimer::Cancel();
}


void CTimeOutTimer::RunL()
{
	if (iStatus == KErrNone)
    {
		iTimeOutCallback->TimeOutTimerNotify(iCookie);
    }
}

void CTimeOutTimer::DoCancel()
{
	CTimer::DoCancel();
}
