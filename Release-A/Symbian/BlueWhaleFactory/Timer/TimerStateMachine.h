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

 
#ifndef __TIMER_STATE_MACHINE_H__
#define __TIMER_STATE_MACHINE_H__

#include "StateMachine.h"

/**
 * ECOMPLUS Component Id for our default timer/system management implementation.
 */
const TInt KCID_MTimerStateMachine = 0x10208067;
const TInt KCID_MThreadedTimerStateMachine = 0xA0003E24;
const TInt KCID_MVMTimerStateMachine = 0xA0003E25;

const TInt KIID_MTimerStateMachine = 0x01000020;


const MStateMachine::TCommand KCommandOpen 		= {KCID_MVMTimerStateMachine, 0x10 };
const MStateMachine::TCommand KCommandOnline	= {KCID_MVMTimerStateMachine, 0x20 };
const MStateMachine::TCommand KCommandOffline	= {KCID_MVMTimerStateMachine, 0x30 };
const MStateMachine::TCommand KCommandClose		= {KCID_MVMTimerStateMachine, 0x40 };
const MStateMachine::TCommand KCommandKill		= {KCID_MVMTimerStateMachine, 0x50 };

class MDebugApplication;

/**
 * An interface which defines the basic properties of a connection.
 */
class MTimerStateMachine : public MStateMachine
{
public: 
	virtual TBool Online() const = 0;
	virtual TBool Offline() const = 0;

#if _DEBUGTERMINAL_
public: // for debug terminal
	virtual MDebugApplication* Application() = 0 ;
#endif

protected:
	virtual ~MTimerStateMachine() {}
};

#endif /* __TIMER_STATE_MACHINE_H__ */

