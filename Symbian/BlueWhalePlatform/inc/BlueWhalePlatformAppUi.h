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



#ifndef BLUEWHALEPLATFORMAPPUI_H
#define BLUEWHALEPLATFORMAPPUI_H

#include <OSVersion.h>
#include "BlueWhalePlatformApplication.h"

#include "ViewInterface.h"
#include "ThreadUtils.h"
#include "BaseMIDPView.h"
class MTimerStateMachine;

class CBlueWhalePlatformAppUi : public __BWM_APPUI__, public MViewAppUiCallback
{
public:
    void ConstructL();
    CBlueWhalePlatformAppUi();
    virtual ~CBlueWhalePlatformAppUi();

    virtual MUnknown * QueryInterfaceL(TInt aInterfaceId);
	virtual void AddRef() { iRefCount++;}
	virtual void Release()
	{	
		if(--iRefCount <= 0)
		{
			delete this;
		}
	}
    
    // MViewAppUiCallback
    virtual MView * GetOrCreateViewL(TInt aViewId, TInt aEComPlusComponentId, MViewAppUiCallback * aViewCallbackForView, TBool aMakeDefault = EFalse );
	virtual void AddViewL(__BWM_VIEW__* aBwmView, MView* aView);
	virtual MView * GetViewL(TInt aViewId) ;
	virtual void SetDefaultViewL(MCoeView & aCoeView);
	virtual void DoActivateViewL(TVwsViewId aViewId);
	virtual void RequestForeground();
	virtual void RequestBackground();
	virtual void RequestExit(TInt aStatus);

	// CEikAppUi
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	virtual TBool ProcessCommandParametersL(CApaCommandLine& aCommandLine);
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	virtual TBool ProcessCommandParametersL(TApaCommand aCommand, TFileName &aDocumentName, const TDesC8 &aTail);
#endif
	virtual void HandleCommandL(TInt aCommand);
	
private: // CEikAppUi
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
	void HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination);
#endif
	static void BackgroundCallback(TAny* aThis);
	static void ForegroundCallback(TAny* aThis);
	static void ExitCallback(TAny* aThis);
	void StartMidpL(const TDesC8& aClassName);
	static void ShowLowMemoryWarningL();
	
	MTimerStateMachine* iVM;
	MView* iView;
	TInt iRefCount;
	CThreadRunner* iThreadRunner;
	MMidpView* iMidpView;
	TInt iExitStatus;
};


#endif /* BLUEWHALEPLATFORMAPPUI_H */

