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


#ifndef __BASEMIDPVIEW_H__
#define __BASEMIDPVIEW_H__

#include <OSVersion.h>
#include <unknown.h>
#include <fepbase.h>
#include "ViewInterface.h"
#include <coecntrl.h>
#include "MIDPApp.h"
#include "EventQueue.h"
#include "KeyMapperFactory.h"
#include "FEPInputControl.h"
#include "MIDPCanvas.h"

const TInt KCID_MBaseMIDPView = 0xA0003F59;
const TInt KIID_MMidpView = 0xA0003F5A;

class MMidpView : public MView
{
public:
	virtual CMIDPCanvas* GetCanvas() = 0;
	virtual void SetEventQueue(MEventQueue* aQueue) =0;
protected:
	virtual ~MMidpView(){}
};

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
class CBaseMIDPView : public CAknView, public MMidpView
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
class CBaseMIDPView : public CQikViewBase, public MMidpView
#endif
{
public:
	static MUnknown* NewL(TAny * aConstructionParameters);
	
	// MUnknown
	MUnknown* QueryInterfaceL(TInt aInterfaceId);
	void AddRef();
	void Release();
	
	// MView
	virtual void AddViewL( MViewAppUiCallback * aViewAppUiCallback, TBool aMakeDefault = EFalse );
	virtual void SetViewAppUiCallback(MViewAppUiCallback * aViewAppUiCallback);
	virtual void DoActivateL();
	virtual void SetViewUid( const TUid & uid );

	virtual CMIDPCanvas* GetCanvas(){return iCanvas;}
	virtual void SetEventQueue(MEventQueue* aQueue);

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__

	// CAknView
	virtual TUid Id() const;
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8&);
    void DoDeactivate();
    void HandleForegroundEventL(TBool aForeground);
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    void DynInitMenuBarL(TInt aResourceId,CEikMenuBar* aMenuBar);
    void HandleCommandL(TInt aCommand);
private:
	CBaseMIDPView();

#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__

	virtual void SetViewModeL(const TQikViewMode& aMode);
	
	// from CQikViewBase
	TVwsViewId ViewId() const;
	void HandleCommandL(CQikCommand& aCommand);
	void ViewActivatedL(const TVwsViewId &aPrevViewId, TUid aCustomMessageId, const TDesC8 &aCustomMessage);
	void ViewDeactivated();
	TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
	
protected: // from CQikViewBase
	void ViewConstructL();
private:
	CBaseMIDPView(CQikAppUi& aAppUi);

#endif
	virtual ~CBaseMIDPView();
	void ConstructL();
	TInt iRefCount;
	TUid iViewUid;
	CMIDPCanvas* iCanvas;
	MEventQueue* iQueue;
	MViewAppUiCallback * iViewAppUiCallback;
	TInt iResourceFileOffset;
};

#endif /*__BASEMIDPVIEW_H__*/
