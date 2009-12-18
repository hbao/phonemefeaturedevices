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


#include "BaseStateMachine.h"


template <class REF_COUNTED_BASE_CLASS>
CBaseStateMachine<REF_COUNTED_BASE_CLASS>::CBaseStateMachine( TAny * aConstructionParameters )
: REF_COUNTED_BASE_CLASS(aConstructionParameters)
, iState(KStateUndefined) // Put the state machined into undefined state until someone does Reset properly.
{
	DEBUG_CONSTRUCTOR
}

template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::ConstructL()
{
	REF_COUNTED_BASE_CLASS::ConstructL();

	Reset();
}

template <class REF_COUNTED_BASE_CLASS>
CBaseStateMachine<REF_COUNTED_BASE_CLASS>::~CBaseStateMachine()
{
	DEBUG_DESTRUCTOR
	Reset();
	// Don't try to release iProperties as it is not owned by us.
}


template <class REF_COUNTED_BASE_CLASS>
MUnknown * CBaseStateMachine<REF_COUNTED_BASE_CLASS>::QueryInterfaceL(TInt aInterfaceId)
{
	if( KIID_MStateMachine == aInterfaceId )
	{
		AddRef();
		return static_cast<MStateMachine*>(this);
	}
	else if( KIID_MInitialize == aInterfaceId )
	{
		AddRef();
		return static_cast<MInitialize*>(this);
	}
	else
	{
		return REF_COUNTED_BASE_CLASS::QueryInterfaceL(aInterfaceId);
	}
}

template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::AddRef()
{
	REF_COUNTED_BASE_CLASS::AddRef();
}

template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::Release()
{
	REF_COUNTED_BASE_CLASS::Release();
}


template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::InitializeL(MProperties * aInitializationProperties)
{
	// This must be done first, as no PushL will be done by MProperties creators
	// so that our cleanup code can work properly in all circumstances.
	iProperties = aInitializationProperties;

	// Not owned -- we shouldn't AddRef as we may be added to this 
	// MProperties by our owner. 
	// The same situation as for callback interfaces in general.
	//iProperties->AddRef();


	iCallback = GoL( iProperties, KPropertyObjectStateMachineCallback, MStateMachineCallback );

	// Not owned and we just incremented refcount so decrement here.
	iCallback->Release();
}


template <class REF_COUNTED_BASE_CLASS>
TBool CBaseStateMachine<REF_COUNTED_BASE_CLASS>::AcceptCommandL( MStateMachine::TCommand /* aCommand */, MProperties * /* aCommandProperties */)
{
	return EFalse;
}

template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::Reset()
{
	SetState(KStateInitial);
}

template <class REF_COUNTED_BASE_CLASS>
const MStateMachine::TState CBaseStateMachine<REF_COUNTED_BASE_CLASS>::State() const
{
	return iState;
}

template <class REF_COUNTED_BASE_CLASS>
void CBaseStateMachine<REF_COUNTED_BASE_CLASS>::SetState(const MStateMachine::TState & aNewState)
{
	//LOG_DEBUG( "CBaseStateMachine", aNewState.Low(), aNewState.High());
	iState = aNewState; 
}

