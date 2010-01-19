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



#ifndef __BASE_STATE_MACHINE_H__
#define __BASE_STATE_MACHINE_H__

#include <e32base.h>
#include <EcomPlusRefCountedBase.h>
#include "StateMachine.h"
#include "DebugUtilities.h"

/**
 * The CBaseStateMachine class is a base for many of our components.
 *
 * Some of these components will be Active Objects, while others simply
 * derive from CBase.  For this reason, we have provided a templated
 * CBaseStateMachine class.
 *
 * We currently envision two primary instantiations:
 *
 *	class MyStateMachine : public CBaseStateMachine<CEComPlusRefCountedBase>
 * 
 *	and:
 * 
 *	class MyOtherStateMachine : public CBaseStateMachine<CEComPlusRefCountedActive>
 * 
 */
template <class REF_COUNTED_BASE_CLASS>
class CBaseStateMachine : public REF_COUNTED_BASE_CLASS, public MStateMachine
{
public: // MUnknown.
	virtual MUnknown * QueryInterfaceL(TInt aInterfaceId);
	virtual void AddRef();
	virtual void Release();

public: // MInitialize
	virtual void InitializeL(MProperties * aInitializationProperties);

public: // MStateMachine
	virtual TBool AcceptCommandL( MStateMachine::TCommand aCommand, MProperties * aCommandProperties );
	virtual void Reset();

protected:

	CBaseStateMachine( TAny * aConstructionParameters );
	void ConstructL();
	virtual ~CBaseStateMachine();

	/**
	 * Even derived classes must use these accessors
	 * to facilitate debugging.
	 */ 
	virtual const MStateMachine::TState State() const;
	virtual void SetState( const MStateMachine::TState & aNewState );

	/**
	 * The MProperties property bag with which we were initialized
	 * in our 3rd stage constructor InitializeL()
	 * 
	 * Not owned -- we shouldn't AddRef as we may be added to this 
	 * MProperties by our owner. 
	 * The same situation as for callback interfaces in general.
	 */
	MProperties * iProperties;


	/**
	 * Pointer retrieved from KPropertyObjectStateMachineCallback key on 
	 * aInitializationProperties during InitializeL call.
	 *
	 * Not owned.
	 */
	MStateMachineCallback * iCallback;

	// This should only be set to ETrue once InitializeL has completed.
	TBool iInitializationCompleted;


private:
	/**
	 * Private so that even derived classes must use accessors.
	 */
	MStateMachine::TState iState;
	
};

#include "BaseStateMachine.inl"

#endif /* __BASE_STATE_MACHINE_H__ */
