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


#include "BaseMIDPView.h"
#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
#include <aknviewappui.h>
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <qikappui.h>
#include <qikcommand.h>
#endif
#include <eikbtgpc.h>
#include <eikmenup.h>
#include "PlatformVMTimerStateMachine.h"
#include "KeyMapperFactory.h"
#include <MIDPView.rsg>
#include "ThreadUtils.h"


#ifdef __WINSCW__
//#define __DEBUGMIDPVIEW__
#endif

#ifdef __DEBUGMIDPVIEW__
#define	DEBUGMSG(_XX) RDebug::Print(_XX)
#define	DEBUGMSG1(_XX,_AA) RDebug::Print(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB) RDebug::Print(_XX,_AA,_BB)
#else
#define	DEBUGMSG(_XX)
#define	DEBUGMSG1(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB)
#endif

#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
MUnknown* CBaseMIDPView::NewL(TAny* )
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
MUnknown* CBaseMIDPView::NewL(TAny* aConstructionParameters)
#endif
{
#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
	CBaseMIDPView* self = new (ELeave) CBaseMIDPView();
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	CBaseMIDPView* self = new (ELeave) CBaseMIDPView(*((CQikAppUi*)aConstructionParameters));
#endif
	CleanupStack::PushL(self);
	self->ConstructL();
	MUnknown* unknown = self->QueryInterfaceL(KIID_MUnknown);
	CleanupStack::Pop(self);
	return unknown;
}

#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
CBaseMIDPView::CBaseMIDPView()
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
CBaseMIDPView::CBaseMIDPView(CQikAppUi& aAppUi)
: CQikViewBase(aAppUi, KNullViewId)
#endif
{}

CBaseMIDPView::~CBaseMIDPView()
{
	CEikonEnv::Static()->DeleteResourceFile(iResourceFileOffset);
#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
	AppUi()->RemoveFromStack(iCanvas);
	delete iCanvas;
#endif
}

void CBaseMIDPView::ConstructL()
{
	CEikonEnv* eikEnv=CEikonEnv::Static();    //cache the Eikon pointer, as Static() is slow
#ifdef __WINSCW__
	iResourceFileOffset = eikEnv->AddResourceFileL(_L("z:\\resource\\apps\\MIDPView.rsc")); 
#else
	_LIT(KLitResourceFileName, "MIDPView.rsc");
	_LIT(KLitResourcePath, "\\resource\\apps\\");
	  
	TFileName resourceFileName;
	Dll::FileName(resourceFileName); // Get the drive letter

	TParse parse;
	User::LeaveIfError(parse.SetNoWild(KLitResourceFileName, &KLitResourcePath, &resourceFileName));
	resourceFileName.Copy(parse.FullName());
	
	iResourceFileOffset = eikEnv->AddResourceFileL(resourceFileName); 
#endif
#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
	BaseConstructL(R_MIDPVIEW_VIEW);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	CQikViewBase::ConstructL();
#endif
	if(!iCanvas)
	{
		// construct the view control
		iCanvas = new (ELeave) CMIDPCanvas();

#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
		// show the control
		iCanvas->SetMopParent(this);
		TRect rect(ClientRect());
		iCanvas->ConstructL(rect,iViewAppUiCallback,this);
		AppUi()->AddToStackL(*this, iCanvas);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
		TRect rect(iQikAppUi.ClientRect().Size());
		iCanvas->ConstructL(rect,iViewAppUiCallback,this);
#endif
	}
}

// MUnknown
MUnknown* CBaseMIDPView::QueryInterfaceL(TInt aInterfaceId)
{
	if(aInterfaceId == KIID_MUnknown)
	{
		AddRef();
		return static_cast<MUnknown*>(this);
	}
	else if(aInterfaceId == KIID_MView)
	{
		AddRef();
		return static_cast<MView*>(this);
	}
	else if(aInterfaceId == KIID_MMidpView)
	{
		AddRef();
		return static_cast<MMidpView*>(this);
	}
	else
	{
		User::Leave(KErrNotSupported);
	}
	return NULL;
}

void CBaseMIDPView::AddRef()
{
	iRefCount++;
}

void CBaseMIDPView::Release()
{
	--iRefCount;
	if(iRefCount == 0)
	{
		delete this;
	}
}

void CBaseMIDPView::SetEventQueue(MEventQueue* aQueue)
{
	iQueue = aQueue;
	if(iCanvas)
	{
		iCanvas->SetEventQueue(aQueue);
	}
}

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__

TUid CBaseMIDPView::Id() const
{
	return iViewUid;
}

void CBaseMIDPView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8&)
{
	iCanvas->SetCba(Cba());
	iCanvas->MakeVisible(ETrue);
}

void CBaseMIDPView::DoDeactivate()
{
	iCanvas->MakeVisible(EFalse);
}

void CBaseMIDPView::HandleForegroundEventL(TBool aForeground)
{
	DEBUGMSG1(_L("CBaseMIDPView::HandleForegroundEventL %d"),aForeground);
	if(iQueue)
	{
		TEventInfo event;
		event.iEvent = KMIDPForegroundEvent;
		event.iIntParam1 = aForeground;
	    iQueue->AddEvent(event);
	}
}

void CBaseMIDPView::DynInitMenuBarL(TInt /*aResourceId*/,CEikMenuBar* /*aMenuBar*/)
{
}

void CBaseMIDPView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MIDPVIEW_MENUPANE && aMenuPane)
	{
		RArray<TMidpCommand>& commands = iCanvas->LockCommands();
		TInt length = commands.Count();
		// Add the EOK items first so that they appear at the top of the menu
		// (usually they'd be on the CBA but there's no room)
		for(TInt i=0;i<length;i++)
		{
			if (commands[i].type == EOK)
			{
				CEikMenuPaneItem::SData item;
				item.iCommandId = 0x1000 | commands[i].id;
				item.iCascadeId = 0;
				item.iFlags = 0;
				item.iText.Copy(commands[i].shortLabel_str->Des());
				aMenuPane->AddMenuItemL(item);
			}
		}		

		// Add the other items
		for(TInt i=0;i<length;i++)
		{
			switch (commands[i].type)
			{
			case ESCREEN:
			case EITEM:
			{
				CEikMenuPaneItem::SData item;
				item.iCommandId = 0x1000 | commands[i].id;
				item.iCascadeId = 0;
				item.iFlags = 0;
				item.iText.Copy(commands[i].shortLabel_str->Des());
				aMenuPane->AddMenuItemL(item);
				break;
			}
			case ENATIVE:
			{
				CEikMenuPaneItem::SData item;
				item.iCommandId = commands[i].id;
				item.iCascadeId = 0;
				item.iFlags = 0;
				item.iText.Copy(commands[i].shortLabel_str->Des());
				aMenuPane->AddMenuItemL(item);
				break;
			}
			default:
				break;
			}
		}
		iCanvas->UnlockCommands();
	}
}

#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__

void CBaseMIDPView::SetViewModeL(const TQikViewMode& aMode)
{
	CQikViewBase::SetViewModeL(aMode);
}

void CBaseMIDPView::ViewConstructL()
{
	ViewConstructFromResourceL(R_MIDPVIEW_VIEW);
	Controls().AppendLC(iCanvas);
	CleanupStack::Pop(iCanvas);
}

TVwsViewId CBaseMIDPView::ViewId() const
{
	return TVwsViewId(KUidBlueWhalePlatformApp, iViewUid);
}

void CBaseMIDPView::ViewActivatedL(const TVwsViewId &/*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8 &/*aCustomMessage*/)
{
	iCanvas->MakeVisible(ETrue);
	if(iQueue)
	{
		TEventInfo event;
		event.iEvent = KMIDPForegroundEvent;
		event.iIntParam1 = ETrue;
	    iQueue->AddEvent(event);
	}
}

void CBaseMIDPView::ViewDeactivated()
{
	iCanvas->MakeVisible(EFalse);
	if(iQueue)
	{
		TEventInfo event;
		event.iEvent = KMIDPForegroundEvent;
		event.iIntParam1 = EFalse;
	    iQueue->AddEvent(event);
	}
}

TKeyResponse CBaseMIDPView::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
{
	return iCanvas->OfferKeyEventL(aKeyEvent, aType);
}

#endif

#if __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
void CBaseMIDPView::HandleCommandL(TInt aCommand)
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
void CBaseMIDPView::HandleCommandL(CQikCommand& aCommand)
#endif
{
#if __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	TInt commandId = aCommand;
	if (commandId & 0x1000 || commandId == EAknSoftkeyExit)
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	TInt commandId = aCommand.Id();
	if (commandId & 0x1000 || commandId == EEikCmdExit || commandId == EQikCmdGoBack)
#endif
	{
		if(iQueue)
		{
			TEventInfo event;
			if (commandId & 0x1000)
			{
				event.iIntParam1 = 0x0fff & commandId;
			}
			else
			{
				event.iIntParam1 = iCanvas->BackCommand();
			}
			event.iEvent = KMIDPCommandEvent;
			iQueue->AddEvent(event);
		}
	}
	else 
	{
		iCanvas->HandleCommandL(commandId);
	}
}

// MView
void CBaseMIDPView::AddViewL( MViewAppUiCallback * aViewAppUiCallback, TBool aMakeDefault)
{
	iViewAppUiCallback = aViewAppUiCallback;
	iViewAppUiCallback->AddViewL(this,this);
	iRefCount = -10000;
	if( aMakeDefault )
	{
		iViewAppUiCallback->SetDefaultViewL(*this);
	}
	if(iCanvas)
	{
		iCanvas->SetViewAppUiCallback(iViewAppUiCallback);
	}
}

void CBaseMIDPView::SetViewAppUiCallback(MViewAppUiCallback * aViewAppUiCallback)
{
	iViewAppUiCallback = aViewAppUiCallback;
	if(iCanvas)
	{
		iCanvas->SetViewAppUiCallback(iViewAppUiCallback);
	}
}

void CBaseMIDPView::DoActivateL()
{
	iViewAppUiCallback->DoActivateViewL(ViewId());
}

void CBaseMIDPView::SetViewUid( const TUid & uid )
{
	iViewUid = uid;
}
