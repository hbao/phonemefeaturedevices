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



#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include <e32base.h> // TDesC8
#include "Initialize.h"






const TInt KIID_MStateMachine = 0x100000C;


/**
 * Base interface for all state machines.
 *
 * State machines will expect that on InitializeL, key KPropertyObjectStateMachineCallback
 * will point to an instance of MStateMachineCallback (both defined below).
 */
class MStateMachine : public MInitialize
{
public:


	/**
	 * TState represents a state for a state machine.
	 *
	 * This state is slightly richer than a simple TInt.
	 * This is because we wish to allow new states to be 
	 * defined at a later time by independent 3rd parties.
	 *
	 * To avoid collisions, the high word of the state
	 * should be something chosen to be unique for a component
	 * (e.g. if the component is an ECOM component,
	 * the ECOM component ID should be used).
	 *
	 * The low word of the state should be unique to that component.
	 * 
	 */
	typedef TConstructorlessInt64<KIID_MStateMachine + 10> TState;

	// TODO: Soon.
	//virtual TState QueryState() = 0;





	/**
	 * TCommand represents a command for a state machine.
	 *
	 * This command is slightly richer than a simple TInt.
	 * This is because we wish to allow new commands to be 
	 * defined at a later time by independent 3rd parties.
	 *
	 * To avoid collisions, the high word of the command
	 * should be something chosen to be unique for a component
	 * (e.g. if the component is an ECOM component,
	 * the ECOM component ID should be used).
	 *
	 * The low word of the command should be unique to that component.
	 * 
	 */
	typedef TConstructorlessInt64<KIID_MStateMachine + 20> TCommand;

	/**
	 * Call this method to command the state machine to do something.
	 *
	 * Optional supplemental information needed for this command can
	 * be passed in using aCommandProperties.
	 */
	virtual TBool AcceptCommandL( TCommand aCommand, MProperties * aCommandProperties = 0 ) = 0;



	/**
	 * Call this method to reset a state machine to the initial (ready) state.
	 */
	virtual void Reset() = 0;

protected:
	virtual ~MStateMachine() {}
};




const MStateMachine::TState KStateInitial	= { KIID_MStateMachine, 0x10000 };
const MStateMachine::TState KStateDone		= { KIID_MStateMachine, 0x10001 };
const MStateMachine::TState KStateUndefined	= { KIID_MStateMachine, 0x10002 };


const TInt KErrBadState=(-10000);
const TInt KErrBadFormat=(-10001);



/**
 * Many state machines process a buffer of information and change their state accordingly.
 *
 * State machines which accept this command will use information stored in
 * the KPropertyString8ConnectionReadBuffer and KPropertyIntCurrentReadBufferPosition
 * keys of the MProperties * aCommandProperties property bag they receive in AcceptCommandL.
 */
const MStateMachine::TCommand KCommandStateMachineAcceptReadBuffer = {KIID_MStateMachine, 0x9238ab};

/**
 * The following property keys are used as convention for state machines
 * which accept the KCommandStateMachineAcceptReadBuffer command.
 * 
 *
 * Implementation discussion: These have been chosen to be a small number on purpose, 
 * to speed their retrieval from our hashtable implementation.
 */
const TString8UniqueKey KPropertyString8ConnectionReadBuffer = {1,1};

/**
 * The state machine accepting KCommandStateMachineAcceptReadBuffer should
 * read this value, start at this offset into the KPropertyString8ConnectionReadBuffer
 * and then set its value on completion to:
 *
 *	1) The offset at which processing stopped (inclusive) 
 *	
 *	OR 
 *
 *  2) The KCommandStateMachineAcceptReadBuffer (size + 1) if the 
 *		state machine is not finished and requires more data
 *		to complete processing.
 *
 */
const TIntUniqueKey KPropertyIntCurrentReadBufferPosition = {1,2};




const MStateMachine::TCommand KCommandStateMachineFinishedNegotiatingSecureConnection = { KIID_MStateMachine, 0x98234 };
const MStateMachine::TCommand KCommandStateMachineFinishedNegotiatingConnection = { KIID_MStateMachine, 0x98235 };













const TInt KIID_MStateMachineCallback = 0x100000B;


/**
 * This is the key value at which the MStateMachineCallback will normally 
 * be found in the MProperties interface passed in on the call
 * to MStateMachine::Initialize().
 */
const TObjectUniqueKey KPropertyObjectStateMachineCallback = {KIID_MStateMachineCallback, 0x6234ab45};


/**
 * An interface passed to state machines as an interface added
 * under the KPropertyObjectStateMachineCallback key to
 * the MProperties passed in InitializeL()
 * so that they can perform operations and respond 
 * to content they accept.
 *
 * In general, as the MStateMachineCallback interface is usually an interface
 * which an owned object uses to call back to its owner, you should 
 * be sure never to increase the refcount on this interface.
 * For example, if by some call such as QueryInterfaceL or GetObjectL
 * you raise the refcount, be sure to lower it.  
 * Otherwise a refcount cycle will result and objects will never get 
 * cleaned up.
 */
class MStateMachineCallback : public MUnknown
{
public:
	/**
	 * A state machine calls this method to request something of its owner.
	 *
	 * There is no MProperties here intentionally -- the state machine
	 * would have been initialized with an MProperties object, 
	 * and any communication of data can be used by setting values on that object.
	 * Otherwise we'd end up with MProperties instances all over the place.
	 */
	virtual void CallbackCommandL( MStateMachine::TCommand aCommand ) = 0;


	/**
	 * A state machine calls this to report a state change.
	 *
	 * @param aComponentId should be some meaningful value which disambiguates 
	 * which MStateMachine is reporting the state changed, in case
	 * several MStateMachines have been provided the same MStateMachineCallback
	 * interface.
	 */
	virtual void ReportStateChanged( TInt aComponentId, MStateMachine::TState aState ) = 0;

protected:
	virtual ~MStateMachineCallback() {}
};





#endif /* __STATE_MACHINE_H__ */
