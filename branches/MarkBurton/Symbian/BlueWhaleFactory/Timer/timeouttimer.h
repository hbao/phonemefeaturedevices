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

 
#ifndef __TIME_OUT_TIMER_H__
#define __TIME_OUT_TIMER_H__

#include <unknown.h>


const TInt KCID_MTimeOutTimer = 0x10208068;


const TInt KIID_MTimeOutTimer = 0x01000021;




class MTimeOutTimerCallback
{
public:
	/**
	 * aCookie is the cookie which was set in SetCallback.
	 * Its use allows a client to use more than one MTimeOutTimer
	 * at once and be able to tell which one Notify'ed.
	 *
	 * Note: Not marked Leaving by design -- TRAP issues yourself.
	 * 
	 */
	virtual void TimeOutTimerNotify( TAny * aCookie ) = 0;
protected:
	virtual ~MTimeOutTimerCallback() {}
};


const TInt KTimeIntervalSecondsOneMinute		= 60;


class MTimeOutTimer : public MUnknown
{
public:
	/**
	 * The optional aCookie will be returned in the TimeOutTimerNotify call.
	 */
	virtual void SetCallback( MTimeOutTimerCallback * aTimeOutCallback, TAny * aCookie = 0, CActive::TPriority aPriority = CActive::EPriorityStandard ) = 0;
	
	virtual void At(const TTime& aTime) = 0;

	/**
	 * Out version of After takes seconds and leaves if the number of
	 * seconds is greater than the RTimer::After can handle, which is
	 * bounded by the limits of TTimeIntervalMicroSeconds32, i.e.
	 * 35 minutes, 47 seconds (2147 seconds) according to the Symbian 
	 * documentation.
	 */
	virtual void AfterL(TTimeIntervalSeconds anInterval) = 0;

	virtual void Inactivity(TTimeIntervalSeconds anInterval ) = 0;

	virtual void Reset() = 0;
	
protected:
	virtual ~MTimeOutTimer() {}

};

#endif /* __TIME_OUT_TIMER_H__ */

