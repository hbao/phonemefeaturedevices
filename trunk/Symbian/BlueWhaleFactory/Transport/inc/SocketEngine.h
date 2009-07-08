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


#ifndef __SOCKET_ENGINE_H__
#define __SOCKET_ENGINE_H__

#include "WritableConnection.h"

const TInt KIID_MResolvedConnection = 0x100003E;

class MResolvedConnection : public MUnknown
{
	public:
	    /**
     *  Get the IP address of the resolved connection
     */
	virtual TUint32 ResolvedAddress() const = 0;

	protected:
		virtual ~MResolvedConnection(){};
};

/**
 * ECOMPLUS Component Id for our default MWritableConnection implementation.
 */
const TInt KCID_MSocketEngine = 0x10206D8A;



/**
 * Keys required to be set at time of connect on the aInitializationParameters
 * passed to this object on InitializeL.
 */
const TIntUniqueKey		KPropertyIntSocketPort				= {KIID_MConnection,65324};
const TStringUniqueKey	KPropertyStringSocketServer			= {KIID_MConnection,5643};
const TIntUniqueKey		KPropertyIntSocketConnectionIap		= {KIID_MConnection, 0x94ef50 };





/**
 * States supported by KCID_MSocketEngine.
 */

// KStateInitial (defined with MStateMachine) means totally disconnected.


const MStateMachine::TState KStateSocketEngineRaisingConnection						= {KCID_MSocketEngine, 0x300};
const MStateMachine::TState KStateSocketEngineRaisingConnectionNoOpen				= {KCID_MSocketEngine, 0x305};
const MStateMachine::TState KStateSocketEngineErrorRaisingConnection				= {KCID_MSocketEngine, 0x320};
const MStateMachine::TState KStateSocketEngineCancellingRaisingConnection			= {KCID_MSocketEngine, 0x340};
const MStateMachine::TState KStateSocketEngineConnectionRaised						= {KCID_MSocketEngine, 0x360};

const MStateMachine::TState KStateSocketEngineResolvingServerName					= {KCID_MSocketEngine, 0x400};
const MStateMachine::TState KStateSocketEngineResolvingServerNameNoConnect			= {KCID_MSocketEngine, 0x405};
const MStateMachine::TState KStateSocketEngineErrorResolvingServerName				= {KCID_MSocketEngine, 0x420};
const MStateMachine::TState KStateSocketEngineCancellingResolvingServerName			= {KCID_MSocketEngine, 0x440};
const MStateMachine::TState KStateSocketEngineServerNameResolved					= {KCID_MSocketEngine, 0x460};

const MStateMachine::TState KStateSocketEngineConnectingSocket						= {KCID_MSocketEngine, 0x500};
const MStateMachine::TState KStateSocketEngineErrorConnectingSocket					= {KCID_MSocketEngine, 0x520};
const MStateMachine::TState KStateSocketEngineCancellingConnectingSocket			= {KCID_MSocketEngine, 0x540};
const MStateMachine::TState KStateSocketEngineSocketConnectedButNotYetSecure 		= {KCID_MSocketEngine, 0x560};

const MStateMachine::TState KStateSocketEngineSecuring								= {KCID_MSocketEngine, 0x600};
const MStateMachine::TState KStateSocketEngineErrorSecuring							= {KCID_MSocketEngine, 0x620};
const MStateMachine::TState KStateSocketEngineCancellingSecuring					= {KCID_MSocketEngine, 0x640};

const MStateMachine::TState KStateSocketEngineRaisingAcceptConnection				= {KCID_MSocketEngine, 0x650};
const MStateMachine::TState KStateSocketEngineConnectedAcceptConnection				= {KCID_MSocketEngine, 0x655};

// KStateDone (defined with MStateMachine) means a secure connection has been established.




#endif /* __SOCKET_ENGINE_H__ */
