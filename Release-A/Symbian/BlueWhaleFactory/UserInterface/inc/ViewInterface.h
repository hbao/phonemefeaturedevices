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


#ifndef __VIEW_INTERFACE_H__
#define __VIEW_INTERFACE_H__

#include <OSVersion.h>
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <aknview.h>
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <qikviewbase.h>
#endif
#include <unknown.h>
#include "properties.h"

class MViewAppUiCallback;


const TInt KIID_MView = 0x1000018;

/**
 * We've found it handy to be able to create views using ECOMPLUS
 * and then add them to CAknViewAppUi.
 *
 * This interface represents what is needed for an ECOMPLUS
 * component to be available to use as a CAknView.
 * 
 */
class MView : public MUnknown
{
public:

	/**
	 * Ask the view to add itself to the CAknViewAppUi.
	 *
	 * The MView needs to do this itself because it
	 * isn't exposing its own CAknView implementation
	 * to the world.
	 */
	virtual void AddViewL( MViewAppUiCallback * aViewAppUiCallback, TBool aMakeDefault = EFalse ) = 0;

	/**
	 * Sometimes we may retrieve an already existing view but from a different list.
	 * Actions on that view should then take effect within the context of the new list.
	 * For this reason, we should always set the MViewAppUiCallback correctly when
	 * re-using an existing view.  The MViewAppUiCallback::GetOrCreateViewL method
	 * does this automatically.
	 */
	virtual void SetViewAppUiCallback(MViewAppUiCallback * aViewAppUiCallback) = 0;


	virtual void DoActivateL() = 0;

	/**
	 * The TVwsViewId returned from ViewId is 32 bits app identifier and
	 * 32 bits view identifier.
	 *
	 * The 32 bits view identifier is obtained from a view by calling the overriden
	 * Id() method.
	 *
	 * Usually, the default behaviour for our views will be to return
	 * their KCID_... ECOMPLUS component id.
	 *
	 * Sometimes, however, we may have more than one view of exactly the
	 * same type of implementation that we wish to differentiate.
	 * This method below can be used to set the 32 bit view identifer for a view.
	 */
	virtual void SetViewUid( const TUid & uid ) = 0;

protected:
	virtual ~MView() {}
};

const TInt KIID_MViewAppUiCallback = 0x1000019;

const TObjectUniqueKey KPropertyObjectViewAppUiCallback = {KIID_MViewAppUiCallback, 0x2341};



/**
 * An MView needs a few things from its owner.
 */
class MViewAppUiCallback : public MUnknown
{
public:

	/**
	 * Attempts to retrieve a particular MView or creates and AddViews it using the
	 * provided information if it does not already exist.
	 *
	 * RefCount is irrelevant on returned value as it is
	 * delete-owned by the CAknViewAppUi.
	 *
	 * Note that aViewId used to identify the view can be different than the 
	 * aEComPlusComponentId used to create the view.  This is 
	 * so that two or more instances of the same class can be separately
	 * identified and used for different purposes.  When this is not needed,
	 * it might make sense to use the aEComPlusComponentId as the aViewId.
	 *
	 * You may wish to set an alternate MViewAppUiCallback for the created view.
	 * If you do not specify one, the class implementing the interface on which you have
	 * invoked this method will itself be used.
	 * 
	 */
	virtual MView * GetOrCreateViewL(TInt aViewId, TInt aEComPlusComponentId, MViewAppUiCallback * aViewCallbackForView, TBool aMakeDefault = EFalse ) = 0;


	/**
	 * We're duplicating the list of views a CAknViewAppUi holds,
	 * but doing it in parallel with our MView objects
	 * so that other will be able to query views in a type
	 * safe way using MUnknown.
	 *
	 * I prefer this to the hacky cast way of doing things.
	 *
	 * Usually you should use GetOrCreateViewL to add new views.
	 */
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	virtual void AddViewL(CAknView* aAknView, MView* aView) = 0;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	virtual void AddViewL(CQikViewBase* aQikView, MView* aView) = 0;
#endif

	/**
	 * RefCount is irrelevant on returned value as it is
	 * delete-owned by the CAknViewAppUi.
	 *
	 * Leaves if view is not found -- you will usually want to 
	 * use GetOrCreateViewL instead, unless you are just checking
	 * to see if the view has already been created.
	 */
	virtual MView * GetViewL(TInt aViewId) = 0;

	virtual void SetDefaultViewL(MCoeView & aCoeView) = 0;

	virtual void DoActivateViewL(TVwsViewId aViewId) = 0;
	virtual void RequestForeground() = 0;

	virtual void RequestBackground() = 0;

	virtual void RequestExit(TInt aStatus) = 0;

protected:
	virtual ~MViewAppUiCallback() {}
};




#endif /* __VIEW_INTERFACE_H__ */
