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


#include <e32std.h>
#include <BAUTILS.H>
#include <OSVersion.h>
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <aknglobalnote.h>
#endif
#include <hal.h>
#include "BlueWhalePlatformAppUi.h"
#include "TimerStateMachine.h"
#include <ecomplus.h>
#include "ManagementObjectFactory.h"
#include "Application.h"

CBlueWhalePlatformAppUi::CBlueWhalePlatformAppUi()
{}

void CBlueWhalePlatformAppUi::ConstructL()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	BaseConstructL(EAknEnableSkin);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	CQikAppUi::ConstructL();
#endif

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	// We need at least KMaxHeap bytes of available memory to run VM
	TInt freeRam = 0;
	HAL::Get( HAL::EMemoryRAMFree, freeRam );
	if(freeRam < KMaxHeap)
	{
		User::Leave(KErrNoMemory);
	}
#endif
	AddRef();
	iEikonEnv->RootWin().SetOrdinalPosition(-1);
	iThreadRunner = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL();
}
    
CBlueWhalePlatformAppUi::~CBlueWhalePlatformAppUi()
{
	if(iVM)
	{
		iVM->Release();
	}
	iVM = NULL;

	if (iMidpView)
	{
		iMidpView->SetEventQueue(NULL);
		iMidpView->Release();
	}
	if(iView)
	{
		iView->Release();
	}
	delete iThreadRunner;
}

MUnknown * CBlueWhalePlatformAppUi::QueryInterfaceL(TInt aInterfaceId)
{
	if(aInterfaceId == KIID_MViewAppUiCallback)
	{
		AddRef();
		return static_cast<MViewAppUiCallback*>(this);
	}
	else
	{
		User::Leave(KErrNotSupported);
	}
}

TKeyResponse CBlueWhalePlatformAppUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
{
    TKeyResponse response = EKeyWasNotConsumed;
    return response;
}
MView * CBlueWhalePlatformAppUi::GetOrCreateViewL(TInt aViewId, TInt aEComPlusComponentId, MViewAppUiCallback * aViewCallbackForView, TBool aMakeDefault)
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
    MView* view = CiL(aEComPlusComponentId,MView);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
    MView* view = static_cast<MView*>(REComPlusSession::CreateImplementationL(TUid::Uid(aEComPlusComponentId), TUid::Uid(KIID_MView), this));
#endif    
    view->SetViewUid(TUid::Uid(aViewId));
    view->SetViewAppUiCallback(aViewCallbackForView);
    view->AddViewL(this,aMakeDefault);
    return view;
}

#if __S60_VERSION__ >= __S60_V2_FP3_VERSION_NUMBER__
void CBlueWhalePlatformAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination)
{
	// intercept the red hangup key and background the app rather than letting it get killed
	switch (aEvent.Type())
	{
	case KAknUidValueEndKeyCloseEvent:
	{
		RequestBackground();
		break;
	}
	default:
	{
		CAknAppUi::HandleWsEventL(aEvent, aDestination);
	}
    }
}
#endif

void CBlueWhalePlatformAppUi::AddViewL(__BWM_VIEW__* aBwmView, MView* /*aView*/)
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	__BWM_APPUI__::AddViewL(aBwmView);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	__BWM_APPUI__::AddViewL(*aBwmView);
#endif    
}

MView * CBlueWhalePlatformAppUi::GetViewL(TInt /*aViewId*/)
{
	return iView;
}

void CBlueWhalePlatformAppUi::SetDefaultViewL(MCoeView & aCoeView)
{
	__BWM_APPUI__::SetDefaultViewL(aCoeView);
}

void CBlueWhalePlatformAppUi::DoActivateViewL(TVwsViewId aViewId)
{
	__BWM_APPUI__::ActivateViewL(aViewId);
}

void CBlueWhalePlatformAppUi::ShowLowMemoryWarningL()
{
	_LIT(KLowMemoryText, "BlueWhale:\nMemory full. Close some applications and try again");
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__	
	CAknGlobalNote* query = CAknGlobalNote::NewLC();
	TRequestStatus status;
	query->ShowNoteL(status,EAknGlobalErrorNote,KLowMemoryText);
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(query);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	CEikonEnv::Static()->InfoWinL(_L("Information"), KLowMemoryText);
#endif
}

void CBlueWhalePlatformAppUi::RequestForeground()
{
	iThreadRunner->DoCallback(ForegroundCallback,this);
}

void CBlueWhalePlatformAppUi::RequestBackground()
{
	iThreadRunner->DoCallback(BackgroundCallback,this);
}

void CBlueWhalePlatformAppUi::BackgroundCallback(TAny* /*aThis*/)
{
	CEikonEnv::Static()->RootWin().SetOrdinalPosition(-1);
}

void CBlueWhalePlatformAppUi::ForegroundCallback(TAny* aThis)
{
	CBlueWhalePlatformAppUi* This = static_cast<CBlueWhalePlatformAppUi*>(aThis);
	This->iView->DoActivateL();
}

void CBlueWhalePlatformAppUi::RequestExit(TInt aStatus)
{
	iExitStatus = aStatus;
	iThreadRunner->DoCallback(ExitCallback,this);
}

void CBlueWhalePlatformAppUi::ExitCallback(TAny* aThis)
{
	CBlueWhalePlatformAppUi* This = static_cast<CBlueWhalePlatformAppUi*>(aThis);

	if(This->iExitStatus != KErrNone)
	{
		TRAPD(ignore,ShowLowMemoryWarningL());
	}

	This->Exit();
}

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
TBool CBlueWhalePlatformAppUi::ProcessCommandParametersL(CApaCommandLine& aCommandLine)
{
	if(aCommandLine.Command() == EApaCommandBackground)
	{
		iEikonEnv->RootWin().SetOrdinalPosition(-1);
	}
	else
	{
		iEikonEnv->RootWin().SetOrdinalPosition(0);
	}
	
	iView = GetOrCreateViewL(KCID_MBaseMIDPView,KCID_MBaseMIDPView,this,ETrue);
	StartMidpL(aCommandLine.OpaqueData());
	return __BWM_APPUI__::ProcessCommandParametersL(aCommandLine);
}
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
TBool CBlueWhalePlatformAppUi::ProcessCommandParametersL(TApaCommand aCommand, TFileName &aDocumentName, const TDesC8 &aTail)
{
	if (aCommand == EApaCommandRun)
	{
		iEikonEnv->RootWin().SetOrdinalPosition(0);
	    iView = GetOrCreateViewL(KCID_MBaseMIDPView,KCID_MBaseMIDPView,this,ETrue);
	    StartMidpL(aTail);
	}
	else
	{
		// autostarting - need to to bring the window to the front or midlet will fail to launch on s60v2fp2
		iEikonEnv->RootWin().SetOrdinalPosition(0);
	    iView = GetOrCreateViewL(KCID_MBaseMIDPView,KCID_MBaseMIDPView,this,ETrue);
	    StartMidpL(aTail);
	}
	return CAknViewAppUi::ProcessCommandParametersL(aCommand, aDocumentName, aTail);
}
#endif

void CBlueWhalePlatformAppUi::HandleCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EEikCmdExit:
		Exit();
		break;
	}
}

void CBlueWhalePlatformAppUi::StartMidpL(const TDesC8& aClassName)
{
	MVMObjectFactory* factory = DiL(MVMObjectFactory);
	CleanupReleasePushL(*factory);
	
	// a little test hack
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	properties->SetObjectL(KPropertyObjectViewAppUiCallback,this);
	iVM = static_cast<MTimerStateMachine*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MTimerStateMachine), TUid::Uid(KIID_MTimerStateMachine),properties));
	CleanupStack::PopAndDestroy(properties);
	properties = NULL;
	
	MVMObjectFactoryClient* client = QiL(iVM,MVMObjectFactoryClient);
	CleanupReleasePushL(*client);
	client->SetClient(factory);

	iMidpView = QiL(iView,MMidpView);
	factory->SetCanvas(iMidpView->GetCanvas());
	
	iVM->AcceptCommandL(KCommandOpen,NULL);
	properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	properties->SetString8L(KPropertyString8ClassName, aClassName);
	iVM->AcceptCommandL(KCommandOnline, properties);
	CleanupStack::PopAndDestroy(properties);

	MEventQueue* queue = factory->EventQueue();
	iMidpView->SetEventQueue(queue);
	
	CleanupStack::PopAndDestroy(client);
	CleanupStack::PopAndDestroy(factory);
}
