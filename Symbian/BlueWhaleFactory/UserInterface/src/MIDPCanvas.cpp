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

#include <OSVersion.h>
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <avkon.rsg>
#include <CPbkContactEngine.h>
#include <CPbkEmailAddressSelect.h> 
#include <CPbkContactItem.h>
#include <CPbkMultipleEntryFetchDlg.h>
#include <RPbkViewResourceFile.h>
#include <AknUtils.h>
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <QMappExtInterface.h>
#include <ContacUIReturnData.h>
#include <ContacuiListDialog.h>
#include <hal.h>
#endif
#include <cntdb.h>
#include <e32property.h>
#include "application.h"

#include <keymap_input.h>
#include "MIDPCanvas.h"
#include <MIDPView.rsg>

#include "BaseMIDPView.h"

extern "C"
{
#include <pcsl_string.h>
#include <nim.h>
}

#ifdef __WINSCW__
//#define __DEBUGMIDPCANVAS__
#endif

#ifdef __DEBUGMIDPCANVAS__
#define	DEBUGMSG(_XX) RDebug::Print(_XX)
#define	DEBUGMSG1(_XX,_AA) RDebug::Print(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB) RDebug::Print(_XX,_AA,_BB)
#define	DEBUGMSG3(_XX,_AA,_BB,_CC) RDebug::Print(_XX,_AA,_BB,_CC)
#else
#define	DEBUGMSG(_XX)
#define	DEBUGMSG1(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB)
#define	DEBUGMSG3(_XX,_AA,_BB,_CC)
#endif

_LIT(KLookupStr,"Look up");
const TInt KLookupCmd = 0x123;

CMIDPCanvas::CMIDPCanvas()
{}

CMIDPCanvas::~CMIDPCanvas()
{
	iTextArray.ResetAndDestroy();
	iTextArray.Close();
	delete iFEPControl;
	if(iDrawer)
	{
		iDrawer->Cancel();
	}
	delete iDrawer;
	delete iMidpControl;
	
	if(iKeyMapper)
	{
		iKeyMapper->Release();
		iKeyMapper = NULL;
	}
	FreeCommands();
	
	iCommandMutex.Close();
	iMenuSync.Close();
	iBufferMutex.Close();
	delete iThreadRunner;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	delete iPbkContactEngine;
#endif
	iPressedKeys.Close();
}

void CMIDPCanvas::ConstructL(const TRect& aRect,MViewAppUiCallback* aViewAppUiCallback,CBaseMIDPView* aView)
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	CreateWindowL();
#endif
	iView = aView;
	iViewAppUiCallback = aViewAppUiCallback;
	iKeyMapper = TKeyMapperFactory::GetPlatformKeyMapperL();
	iDrawer = new (ELeave) CDrawer(this);
	iDrawer->StartL();
	iMidpControl = CMidpScreenControl::NewL(this);
	iMidpControl->SetContainerWindowL(*this);
	SetExtentToWholeScreen();
	iFullScreen = ETrue;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	iCanvasRect = Rect();
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	TQikViewMode mode(iView->ViewMode());
	mode.SetButtonOrSoftkeyBar(EFalse);
	mode.SetAppTitleBar(EFalse);
	mode.SetStatusBar(EFalse);
	mode.SetToolbar(EFalse);
	iView->SetViewModeL(mode);
#endif
	ActivateL();
	User::LeaveIfError(iCommandMutex.CreateLocal());
	User::LeaveIfError(iMenuSync.CreateLocal(0));
	User::LeaveIfError(iBufferMutex.CreateLocal());
	iThreadRunner  = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL();
	
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	iPbkContactEngine = CPbkContactEngine::NewL();
	CalculateCanvasRect();
#endif
	iKeyPressFIFO = _L("\x00\x00\x00\x00\x00");
	iKeyPressFIFO.SetLength(5);
	HAL::Get(HALData::EMachineUid, iMachineUidValue);
}

TInt CMIDPCanvas::CountComponentControls() const
{
	return iFEPControl != NULL ? 2 : 1;
}
CCoeControl* CMIDPCanvas::ComponentControl(TInt aIndex) const
{
	if(aIndex == 0)
	{
		return iMidpControl;
	}
	else
	{
		return iFEPControl;
	}
}

void CMIDPCanvas::SizeChanged()
{
	iMidpControl->SetRect(Rect());
}

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
void CMIDPCanvas::HandleResourceChange(TInt aType)
{
	if(aType == KEikDynamicLayoutVariantSwitch)
	{
		CalculateCanvasRect();
		iDrawingState = CMIDPCanvas::KWaitingForRepositioning;
		TEventInfo event;
	    event.iEvent = KMIDPScreenChangeEvent;
	    iQueue->AddEvent(event);
	}
}
#else
void CMIDPCanvas::HandleResourceChange(TInt /*aType*/)
{
}
#endif

void CMIDPCanvas::FreeCommands()
{
	TInt commandsLength = iCommands.Count();
	iPreviousCommandsCount = commandsLength;
	for(TInt i=0;i<commandsLength;i++)
	{
		delete iCommands[i].shortLabel_str;
		delete iCommands[i].longLabel_str;
	}
	iCommands.Reset();
}

void CMIDPCanvas::SetMenus(TMidpCommand *aCommands, int aLength)
{
	iCommandMutex.Wait();
	FreeCommands();
	if(iAddLookup)
	{
		TMidpCommand lookup;
		lookup.priority = 100;
		lookup.shortLabel_str = KLookupStr().Alloc();
		lookup.longLabel_str = NULL;
		lookup.type = ENATIVE;
		lookup.id = KLookupCmd;

		iCommands.Append(lookup);
	}
	for(TInt i=0;i<aLength;i++)
	{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
		DEBUGMSG3(_L("CMIDPCanvas::SetMenus %S type %d id %d"),&(aCommands[i].shortLabel_str->Des()),aCommands[i].type,aCommands[i].id);
		iCommands.Append(aCommands[i]);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
		if (aCommands[i].type != EHELP)
		{
			DEBUGMSG3(_L("CMIDPCanvas::SetMenus %S type %d id %d"),&(aCommands[i].shortLabel_str->Des()),aCommands[i].type,aCommands[i].id);
			iCommands.Append(aCommands[i]);
		}
#endif
	}
	iCommandMutex.Signal();
	if(iThreadRunner->DoCallback(SetMenusCallback,this) != KErrNone)
	{
		return;
	}
	iMenuSync.Wait();

}
void CMIDPCanvas::SetMenusCallback(TAny* aThis)
{
	CMIDPCanvas* This = static_cast<CMIDPCanvas*>(aThis);
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if (This->iCba)
	{
		TRAPD(ignore,This->iCba->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY));
		TRAP(ignore,This->SetCbaL());
		This->iCba->DrawDeferred();
	}
	
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__	
	CQikCommandManager& cm = CQikCommandManager::Static();
	cm.DeleteCommandsOriginatingFrom(*This);
	TRAPD(ignore,This->SetCbaL());
#endif
	This->iMenuSync.Signal();
}

RArray<TMidpCommand>& CMIDPCanvas::LockCommands()
{
	iCommandMutex.Wait();
	return iCommands;
}

void CMIDPCanvas::UnlockCommands()
{
	iCommandMutex.Signal();
}

void CMIDPCanvas::SetCba(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
}

void CMIDPCanvas::SetCbaL()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if(iCba)
	{
		TInt commandsLength = iCommands.Count();
		TInt button1Index = KErrNotFound;
		TInt button2Index = KErrNotFound;
		for(TInt i=0;i<commandsLength;i++)
		{
			switch (iCommands[i].type)
			{
			case EBACK:
			case ECANCEL:
			case ESTOP:
			{
				button2Index = i;
				break;
			}
			case EOK:
			{
				button1Index = i;
				break;
			}
			case ESCREEN:
			{
				if (commandsLength == 2)
				{
					button1Index = i;
				}
				break;
			}
			default:
				break;
			}
		}

		if (button2Index != KErrNotFound)
		{
			iCba->SetCommandL(2,EAknSoftkeyExit, iCommands[button2Index].shortLabel_str->Des());
			iBackCommand = iCommands[button2Index].id;
			iCba->DrawDeferred();
			iCommands.Remove(button2Index);
			commandsLength--;
		}

		if (commandsLength > 1)
		{
			iCba->SetCommandL(0,EAknSoftkeyOptions, _L("Options"));
			button1Index = KErrNotFound;
		}

		if ((button1Index == KErrNotFound) && (commandsLength == 1))
		{
			button1Index = 0;
		}
		
		if (button1Index != KErrNotFound)
		{
			iCba->SetCommandL(0,iCommands[button1Index].id | 0x1000, iCommands[button1Index].shortLabel_str->Des());
			iOkCommand = iCommands[button1Index].id;
			iCba->DrawDeferred();
			iCommands.Remove(button1Index);
		}
		
		TBool changed = CalculateCanvasRect();
		if (changed)
		{
			iDrawingState = CMIDPCanvas::KWaitingForRepositioning;
			TEventInfo event;
			event.iEvent = KMIDPScreenChangeEvent;
			iQueue->AddEvent(event);
		}
	}
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__	
	CQikCommandManager& cm = CQikCommandManager::Static();
	TInt commandsLength = iCommands.Count();
	for(TInt i=0;i<commandsLength;i++)
	{
		CQikCommand* cmd = NULL;
		switch (iCommands[i].type)
		{
		case EOK:
			cmd = CQikCommand::NewLC(0x1000 | iCommands[i].id);
			iOkCommand = iCommands[i].id;
			cmd->SetType(EQikCommandTypeYes);
			break;
		case EBACK:
		case ECANCEL:
			cmd = CQikCommand::NewLC(EEikCmdExit);
			iBackCommand = iCommands[i].id;
			cmd->SetType(EQikCommandTypeNo);
			break;
		case ESCREEN:
		case EITEM:
			cmd = CQikCommand::NewLC(0x1000 | iCommands[i].id);
			cmd->SetType(EQikCommandTypeScreen);
			break;
		default:
			cmd = CQikCommand::NewLC(iCommands[i].id);
			cmd->SetType(EQikCommandTypeScreen);
			break;
		}
		cmd->SetTextL(iCommands[i].shortLabel_str->Des());
		cmd->SetHandler(iView);
		cm.InsertCommandL(*this, cmd); 
		CleanupStack::Pop(cmd);
	}

	if ((iCommands.Count() != iPreviousCommandsCount) && (iCommands.Count() == 0 || iPreviousCommandsCount == 0))
	{
		TQikViewMode mode(iView->ViewMode());
		mode.SetButtonOrSoftkeyBar(iCommands.Count());
		iView->SetViewModeL(mode);
		SetRect(iView->ContainerWindow().Rect());

		TEventInfo event;
	    event.iEvent = KMIDPScreenChangeEvent;
	    iQueue->AddEvent(event);
	}
#endif
}

TInt CMIDPCanvas::BackCommand()
{
	return iBackCommand;
}

TInt CMIDPCanvas::OkCommand()
{
	return iOkCommand;
}

void CMIDPCanvas::HandleCommandL(TInt aCommand)
{
	if(aCommand == KLookupCmd)
	{
		LookupEmailL();
	}
}

void CMIDPCanvas::LookupEmailL()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	TBool phoneBookResourceOpened = EFalse;
   	RPbkViewResourceFile phonebookResource( *(CEikonEnv::Static())  );
    if( !phonebookResource.IsOpen() )
    {
        phonebookResource.OpenL();
		CleanupClosePushL(phonebookResource);
		phoneBookResourceOpened = ETrue;
    }

	CContactDatabase::TContactViewFilter filter(CContactDatabase::EMailable); 
    CPbkMultipleEntryFetchDlg::TParams multipleEntryDialogParams;
    multipleEntryDialogParams.iContactView = &iPbkContactEngine->FilteredContactsViewL( filter ); 
    
    // Launch fetching dialog
    CPbkMultipleEntryFetchDlg* fetchDlg = CPbkMultipleEntryFetchDlg::NewL( multipleEntryDialogParams, *iPbkContactEngine );
    fetchDlg->SetMopParent(this);  
    TInt okPressed = fetchDlg->ExecuteLD();
    CleanupStack::PushL( multipleEntryDialogParams.iMarkedEntries );

    if ( okPressed )
    {
		TInt selectionCount = multipleEntryDialogParams.iMarkedEntries->Count();
        // Get the selected contacts id array
		if(selectionCount > 0)
		{
			if(iFEPControl)
			{
				iFEPControl->CancelFEPTransaction();
			}
		}
		for ( TInt i = 0; i < selectionCount; ++i )
        {
            const TContactItemId cid = ( *multipleEntryDialogParams.iMarkedEntries )[i];
            
			// Deal with multiple email addresses for a given contact.
            CPbkContactItem* pbkItem = iPbkContactEngine->ReadContactLC( cid );
            
			CPbkEmailAddressSelect * selectDialog = new (ELeave) CPbkEmailAddressSelect();
			//CleanupStack::PushL(selectDialog); //We ExecuteLD directly below with no Leaving methods between.

			CPbkEmailAddressSelect::TParams addressSelectParams( *pbkItem );
			addressSelectParams.SetFocusedField(pbkItem->DefaultEmailField());
			addressSelectParams.SetUseDefaultDirectly(ETrue);

			if( selectDialog->ExecuteLD(addressSelectParams) )
			{
		        const TPbkContactItemField * selectedField = addressSelectParams.SelectedField();
				//AddRecipientToMessageL( aMimeHeaderType, selectedField->Text() );
		        DEBUGMSG1(_L("Selected entry %S"),&(selectedField->Text()));
		        if(iCurrentField && iCurrentField->TextPtr()->Length() > 0)
		        {
		        	InsertTextL(_L(";"));
				}
		        InsertTextL(selectedField->Text());
		    	SendTextUpdateEvent();
			}

			CleanupStack::PopAndDestroy(pbkItem);
		}
	}

    CleanupStack::PopAndDestroy(multipleEntryDialogParams.iMarkedEntries);

	if( phoneBookResourceOpened )
	{
		CleanupStack::PopAndDestroy(&phonebookResource);
	}
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	
	RPointerArray<CContacuiReturnData> resultArray; 
	if (CContactUiListDialog::RunDlgLD(resultArray, EListDialogDefault, CContactDatabase::EMailable)) 
	{ 
		for(TInt i = 0; i < resultArray.Count(); i++) 
			{ 
			TPtrC emailAddress = resultArray[i]->ReturnedData();

			DEBUGMSG1(_L("Selected entry %S"),&emailAddress);
			if(iCurrentField && iCurrentField->TextPtr()->Length() > 0)
			{
				InsertTextL(_L(";"));
			}
			InsertTextL(emailAddress);
			SendTextUpdateEvent();
			} 

		resultArray.ResetAndDestroy(); 
		resultArray.Close();
	}
	
#endif
}

void CMIDPCanvas::SendTextUpdateEvent()
{
	TEventInfo event;
	event.iEvent = KMIDPKeyDownEvent;
	event.iIntParam1 = 0;
	iQueue->AddEvent(event);
	event.iEvent = KMIDPKeyUpEvent;
	event.iIntParam1 = 0;
	iQueue->AddEvent(event);
	iTextBufferUpdated = ETrue;
}

void CMIDPCanvas::BeginEditSession(TInt aId, TInt aConstraints)
{
	CIndexedText indexedText(aId);
	TInt index = iTextArray.FindInOrder(&indexedText, TLinearOrder<CIndexedText>(CIndexedText::Order));
	if(index == KErrNotFound)
	{
		SetTextFieldString(aId, KNullDesC().Alloc(), 0);
		index = iTextArray.FindInOrder(&indexedText, TLinearOrder<CIndexedText>(CIndexedText::Order));
	}
	ASSERT(index != KErrNotFound);
	iCurrentField = iTextArray[index];
	
	iConstraints = aConstraints;
	DEBUGMSG1(_L("BeginEditSession 0x%08x"),iConstraints);
	iThreadRunner->DoCallback(BeginEditSessionCallback,this);
}

void CMIDPCanvas::EndEditSession()
{
	DEBUGMSG(_L("EndEditSession"));
	iThreadRunner->DoCallback(EndEditSessionCallback,this);
	iCurrentField = NULL;
}

void CMIDPCanvas::BeginEditSessionCallback(TAny* aThis)
{	
	CMIDPCanvas* This = static_cast<CMIDPCanvas*>(aThis);
	
	This->iFEPControl = CFEPInputControl::NewL(This->iCurrentField, This->iQueue, This->iKeyMapper);
	This->iFEPControl->SetMopParent(This);
	This->iFEPControl->SetFocus(ETrue);
	This->iFEPControl->SetCapabilities(This->iConstraints);
	if(This->iConstraints == 1) // TextField.EMAILADDR
	{
		This->iAddLookup = ETrue;
	}
	else
	{
		This->iAddLookup = EFalse;
	}
	This->iTextEdit = ETrue;
}

void CMIDPCanvas::EndEditSessionCallback(TAny* aThis)
{
	CMIDPCanvas* This = static_cast<CMIDPCanvas*>(aThis);
	This->iTextEdit = EFalse;
	This->iAddLookup = EFalse;
	delete This->iFEPControl;
	This->iFEPControl = NULL;
}

TInt CMIDPCanvas::GetNextState()
{
	TInt ret = MEDIATOR_NOOP;
	if (iFEPControl)
	{
		ret = iFEPControl->GetNextState();
	}
	if (iTextBufferUpdated)
	{
		DEBUGMSG(_L("->MEDIATOR_UPDATE"));
		ret = MEDIATOR_UPDATE;
		iTextBufferUpdated = EFalse;	
	}
	return ret;
}

HBufC* CMIDPCanvas::GetInlineText(TInt aId)
{
	ASSERT(iCurrentField != NULL);
	ASSERT(iCurrentField->Id() == aId);
	return iCurrentField->TextPtr();
}

TInt CMIDPCanvas::SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId)
{
	DEBUGMSG3(_L("CMIDPCanvas::SetTextFieldString %d :%S: %d"),aId,&(aText->Des()),aParentId);
	CIndexedText* indexedText = new CIndexedText(aId);
	if (!indexedText)
	{
		return KErrNoMemory;
	}

	TLinearOrder<CIndexedText> order(CIndexedText::Order);
	TInt index = iTextArray.FindInOrder(indexedText, order);
	if (index == KErrNotFound)
	{
		TInt err = iTextArray.InsertInOrder(indexedText, order); // takes ownership
		if (err != KErrNone)	
		{
			delete indexedText;
			return err;
		}
	}
	else
	{
		delete indexedText;
		indexedText = iTextArray[index];
	}
	indexedText->SetTextPtr(aText);
	indexedText->SetEditPoint(aText->Length());
	indexedText->SetParentId(aParentId);
	iCursorPosition = aText->Length();
	return KErrNone;
}

void CMIDPCanvas::SetCursorPosition(TInt aPosition)
{
	DEBUGMSG1(_L("SetCursorPosition %d"),aPosition);
	if (iFEPControl)
	{
		iFEPControl->SetCursorPosition(aPosition);
	}
}

void CMIDPCanvas::GetEditData(TInt& aEditPos, TInt& aEditLength)
{
	TCursorSelection selection(iCursorPosition, iCursorPosition);
	if (iFEPControl)
	{
		selection = iFEPControl->GetEditData();
	}
	DEBUGMSG2(_L("GetEditData %d %d"), selection.LowerPos(), selection.Length());
	aEditPos = selection.LowerPos();
	aEditLength = selection.Length();
}

TBool CMIDPCanvas::GetUnderline()
{
	DEBUGMSG(_L("GetEntryMode"));

	if (iFEPControl)
	{
		return iFEPControl->GetUnderline();
	}
	return EFalse;
}

void CMIDPCanvas::ShowDisplayableNotify(TInt aDisplayableId)
{
	for (TInt i = iTextArray.Count() - 1; i >= 0; i--)
	{
		if (iTextArray[i]->ParentId() != aDisplayableId)
		{
			delete iTextArray[i];
			iTextArray.Remove(i);
		}
	}
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__	
	if (iDrawingState == KWaitingForDisplayableChange)
	{
		iDrawingState = KWaitingForRepositioning;
	}
#endif
}

void CMIDPCanvas::InsertTextL(const TDesC& aNewText)
{
	ASSERT(iCurrentField != NULL);

	// realloc buffer
	TInt newTextLength = aNewText.Length();
	TInt currentLen = iCurrentField->TextPtr()->Length();

	if (iCurrentField->TextPtr()->Des().MaxLength() < currentLen + newTextLength)
	{
		iCurrentField->ReAllocL(currentLen + newTextLength);
	}

	// insert new text
	iCurrentField->TextPtr()->Des().Insert(iFEPControl->CursorPosition(), aNewText);
	CFEPInputCore::DeUnicodeBuffer(iCurrentField->TextPtr()->Des().MidTPtr(iFEPControl->CursorPosition(), newTextLength));
	iFEPControl->SetCursorPosition(iFEPControl->CursorPosition() + newTextLength);
}

TKeyResponse CMIDPCanvas::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	DEBUGMSG3(_L("CMIDPCanvas::OfferKeyEventL scan %d repeats %d type %d"),aKeyEvent.iScanCode,aKeyEvent.iRepeats,aType);
	DEBUGMSG1(_L("CMIDPCanvas::OfferKeyEventL key %d"),aKeyEvent.iCode);
	if(iTextEdit)
	{
		return iFEPControl->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{	
		if (aType == EEventKey)
		{
			iKeyPressFIFO = iKeyPressFIFO.Right(iKeyPressFIFO.Length() - 1);
			iKeyPressFIFO.Append(aKeyEvent.iCode);
			if ((iKeyPressFIFO.CompareF(_L("\xF80A\x0033\x0032\xF809\x0034")) == 0) ||	// down 3 2 up 4
				(iKeyPressFIFO.CompareF(_L("\xF844\x0033\x0032\xF843\x0034")) == 0))	// P1i-specific
			{
				RNotifier notifier;
				if (notifier.Connect() == KErrNone)
				{
					TRequestStatus requestStatus;
					TInt result;
					notifier.Notify(_L("Watcher Monkey:"), _L("Kill and restart?"), _L("Yes"), _L("No"), result, requestStatus);
					User::WaitForRequest(requestStatus);
					if (result == 0)
					{
						TInt ignore = RProperty::Set(KUidStarterExe, KUidWatcherMonkey.iUid, EAnnoyMonkey);
					}
					notifier.Close();
				}
			}
		}
		return PlainOfferKeyEventL(aKeyEvent,aType);
	}
}

void CMIDPCanvas::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	DEBUGMSG3(_L("CMIDPCanvas::HandlePointerEventL type %d X %d Y %d"),aPointerEvent.iType,aPointerEvent.iPosition.iX,aPointerEvent.iPosition.iY);
	TEventInfo event;
	event.iIntParam1 = aPointerEvent.iPosition.iX;
	event.iIntParam2 = aPointerEvent.iPosition.iY;
	switch(aPointerEvent.iType)
	{
	case TPointerEvent::EButton1Down:
		event.iEvent = KMIDPPenPressedEvent;
	    iQueue->AddEvent(event);
		break;
	case TPointerEvent::EButton1Up:
		event.iEvent = KMIDPPenReleaseEvent;
	    iQueue->AddEvent(event);
		break;
	case TPointerEvent::EDrag:
		event.iEvent = KMIDPPenDragEvent;
	    iQueue->AddEvent(event);
		break;
	}
	
}

TKeyResponse CMIDPCanvas::PlainOfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse response = EKeyWasNotConsumed;
	TChar ch = iKeyMapper->CharFromScanCode(aKeyEvent);

	if (TChar(aKeyEvent.iCode).IsPrint())
	{
		ch = aKeyEvent.iCode;
	}
	
	TEventInfo event;
	if((!ch.Eos() || aType == EEventKeyUp) && iQueue)
	{
		switch(aType)
		{
		case EEventKeyDown:
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
			if (!ch.IsPrint())
#else
			if ((iMachineUidValue == KUidZ10Value) && !ch.IsPrint())
#endif
			{
				event.iEvent = KMIDPKeyDownEvent;
				event.iIntParam1 = ch;
				DEBUGMSG1(_L("KMIDPKeyDownEvent %d"),ch);
				iQueue->AddEvent(event);
				response = EKeyWasConsumed;
			}
			break;
		case EEventKey:
			if(aKeyEvent.iRepeats >0)
			{
				for(TInt i=0;i<aKeyEvent.iRepeats;i++)
				{
					event.iEvent = KMIDPKeyRepeatEvent;
					event.iIntParam1 = ch;
					DEBUGMSG1(_L("KMIDPKeyRepeatEvent %d"),ch);
					iQueue->AddEvent(event);
				}
				response = EKeyWasConsumed;
			}
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
			else if (ch.IsPrint())
#else
			else if ((iMachineUidValue != KUidZ10Value) || ch.IsPrint())
#endif
			{
				event.iEvent = KMIDPKeyDownEvent;
				event.iIntParam1 = ch;
				DEBUGMSG1(_L("KMIDPKeyDownEvent %d"),ch);
				iQueue->AddEvent(event);
				TKeyEvent mapping;
				mapping.iScanCode = aKeyEvent.iScanCode;
				mapping.iCode = ch;
				iPressedKeys.Append(mapping);
				response = EKeyWasConsumed;
			}
			break;
		case EEventKeyUp:
			event.iEvent = KMIDPKeyUpEvent;
			for (TInt i = 0; i < iPressedKeys.Count(); i++)
			{
				if (iPressedKeys[i].iScanCode == aKeyEvent.iScanCode)
				{
					ch = iPressedKeys[i].iCode;
					iPressedKeys.Remove(i);
				}
			}
			if (!ch.Eos())
			{
				event.iIntParam1 = ch;
				DEBUGMSG1(_L("KMIDPKeyUpEvent %d"),event.iIntParam1);
				iQueue->AddEvent(event);
				response = EKeyWasConsumed;
			}
			break;
		}
	}
	return response;
}

void CMIDPCanvas::Draw(const TRect& /*aRect*/) const
{
}

TInt CMIDPCanvas::GetDisplayWidth()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	return iCanvasRect.Width();
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	return Rect().Width();
#endif
}

TInt CMIDPCanvas::GetDisplayHeight()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	return iCanvasRect.Height();
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	return Rect().Height();
#endif
}

CFbsBitmap* CMIDPCanvas::Bitmap()
{
	return iMidpControl->Bitmap();
}

TUint32* CMIDPCanvas::CreateDisplayBitmap(TInt aWidth,TInt aHeight)
{
	DEBUGMSG2(_L("CreateDisplayBitmap %d %d"),aWidth,aHeight);
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if(iScreenModeChanging && iFullScreen)
	{
		iScreenModeChanging = EFalse;
		iDoChange = ETrue;
	}
#endif
	return iMidpControl->CreateDisplayBitmap(aWidth,aHeight);	
}

// called by VM thread
void CMIDPCanvas::Refresh(const TRect& aRect)
{
	iDrawer->Refresh(aRect);
}

void CMIDPCanvas::SetFullScreenMode(TBool aFullscreen)
{
	DEBUGMSG1(_L("CMIDPCanvas::SetFullScreenMode %d"),aFullscreen);
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	iScreenModeChanging = (iFullScreen != aFullscreen);
	iFullScreen = aFullscreen;
	if(iScreenModeChanging && !iFullScreen)
	{
		iScreenModeChanging = EFalse;
		iDoChange = ETrue;
		iThreadRunner->DoSyncCallback(SetFullScreenModeCallback,this);
	}
#else
	iFullScreen = aFullscreen;
	iThreadRunner->DoSyncCallback(SetFullScreenModeCallback,this);
#endif
}

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
TBool CMIDPCanvas::CalculateCanvasRect(TBool aIncludeCba)
{
	TRect oldRect = iCanvasRect;
	if (iFullScreen)
	{
		if (iCommands.Count() == 0 || !aIncludeCba)
		{
			// no cba required - use the whole screen
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, iCanvasRect);
		}
		else
		{
			TRect cbaRect;
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPane, cbaRect);
			if (cbaRect.iTl.iY == 0)
			{
				// landscape mode with commands present - use the client rect
				AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, iCanvasRect);
			}
			else
			{
				// portrait mode with commands present - use the whole screen minus the cba
				AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, iCanvasRect);
				iCanvasRect.Resize(0, -cbaRect.Height());
			}
		}
	}
	else
	{	
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, iCanvasRect);
	}
	return (oldRect != iCanvasRect);
}
#endif

void CMIDPCanvas::DoFullScreen()
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if(iView)
	{
		// Don't call canvas SetExtent here - just set up iCanvasRect so that the vm is
		// working with the right size image
		// SetExtent will be delayed until the last minute  (ie CMidpScreenControl::Draw)
		// to minimise the time between the canvas move and redraw.
		// This avoids canvas jumping glitches entering and exiting fullscreen mode.

		CalculateCanvasRect(EFalse);	// don't include the cba - SetCbaL/Draw will update iCanvasRect later if necessary (avoid cba drawing glitch)
		iDrawingState = KWaitingForDisplayableChange;
	}
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if(iView && iDoChange)
	{
		TQikViewMode mode(iView->ViewMode());
		mode.SetButtonOrSoftkeyBar(iCommands.Count());
		mode.SetAppTitleBar(!iFullScreen);
		mode.SetStatusBar(!iFullScreen);
		mode.SetToolbar(!iFullScreen);
		iView->SetViewModeL(mode);
		if (iFullScreen && !iCommands.Count())
		{
			SetRect(CEikonEnv::Static()->ScreenDevice()->SizeInPixels());
		}
		else
		{
			SetRect(iView->ContainerWindow().Rect());
		}
		iDoChange = EFalse;
		TEventInfo event;
	    event.iEvent = KMIDPScreenChangeEvent;
	    iQueue->AddEvent(event);
	}
#endif
}

void CMIDPCanvas::SetFullScreenModeCallback(TAny* aThis)
{	
	DEBUGMSG(_L("CMIDPCanvas::SetFullScreenModeCallback"));
	CMIDPCanvas* This = static_cast<CMIDPCanvas*>(aThis);
	This->DoFullScreen();
}

TBool CMIDPCanvas::FullScreenMode()
{
	return iFullScreen;
}

void CMIDPCanvas::RequestOrdinalChange(TBool aForeground)
{
	DEBUGMSG1(_L("CMIDPCanvas::RequestOrdinalChange %d"),aForeground);
	if(iViewAppUiCallback)
	{
		if(aForeground)
		{
			iViewAppUiCallback->RequestForeground();
		}
		else
		{
			iViewAppUiCallback->RequestBackground();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
CMidpScreenControl* CMidpScreenControl::NewL(CMIDPCanvas* aCanvas)
{
	CMidpScreenControl* self = new (ELeave) CMidpScreenControl(aCanvas);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CMidpScreenControl::~CMidpScreenControl()
{
	delete iThreadRunner;
	delete iBitmap;
}

CMidpScreenControl::CMidpScreenControl(CMIDPCanvas* aCanvas)
: iCanvas(aCanvas)
{}

void CMidpScreenControl::ConstructL()
{
	iThreadRunner  = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL();
}

TInt CMidpScreenControl::CountComponentControls() const
{
	return 0;
}

CCoeControl* CMidpScreenControl::ComponentControl(TInt /*aIndex*/) const
{
	return NULL;
}

void CMidpScreenControl::HandleResourceChange(TInt /*aType*/)
{}

void CMidpScreenControl::SizeChanged()
{}

TUint32* CMidpScreenControl::CreateDisplayBitmap(TInt aWidth,TInt aHeight)
{
	DEBUGMSG2(_L("CreateDisplayBitmap %d %d"),aWidth,aHeight);
	iCanvas->iBufferMutex.Wait();
	iIsLocked = ETrue;
	if(iBuffer)
	{
		User::Free(iBuffer);
		iBuffer = NULL;
	}
	iWidth = aWidth;
	iHeight = aHeight;
	iLength = 2*aWidth*aHeight;
	if(iLength > 0)
	{
		iBuffer = (TUint32*)User::Alloc(iLength);
	}

	iThreadRunner->DoSyncCallback(CreateBitmapCallback,this);
	iIsLocked = EFalse;
    iCanvas->iBufferMutex.Signal();
    return iBuffer;	
}

void CMidpScreenControl::CreateBitmapCallback(TAny* aThis)
{
	CMidpScreenControl* This = static_cast<CMidpScreenControl*>(aThis);
	delete This->iBitmap;
	This->iBitmap = NULL;
	if(This->iLength > 0)
	{
		This->iBitmap = new (ELeave) CWsBitmap(CEikonEnv::Static()->WsSession());
		if(This->iBitmap)
		{
			This->iBitmap->Create(TSize(This->iWidth,This->iHeight),EColor64K);
			TSize size = CEikonEnv::Static()->ScreenDevice()->SizeInTwips();
			TSize pixels = CEikonEnv::Static()->ScreenDevice()->SizeInPixels();
			size.iWidth = (This->iWidth * size.iWidth) / pixels.iWidth;
			size.iHeight= (This->iHeight * size.iHeight) / pixels.iHeight;
			This->iBitmap->SetSizeInTwips(size);
		}
	}
}

CFbsBitmap* CMidpScreenControl::Bitmap()
{
	return iBitmap;
}

void CMidpScreenControl::DrawBitmap(const TRect& aRect,CWindowGc& aGc) const
{
	if(iBitmap && iBuffer && !iIsLocked)
    {
	    iCanvas->iBufferMutex.Wait();
		TSize bitmapSize = iBitmap->SizeInPixels();
	   	TInt topScanLineOffset = Max(bitmapSize.iWidth * aRect.iTl.iY * 2, 0);
		TInt bottomScanLineOffset = Min((bitmapSize.iWidth * aRect.iBr.iY * 2), iLength);
		iBitmap->LockHeap();
	    TUint32* bitmapData = iBitmap->DataAddress();
	    Mem::Copy(((TUint8*)bitmapData) + topScanLineOffset, ((TUint8*)iBuffer) + topScanLineOffset, bottomScanLineOffset - topScanLineOffset);
	    iBitmap->UnlockHeap();
		aGc.BitBlt(aRect.iTl, iBitmap, aRect);
	    iCanvas->iBufferMutex.Signal();
    }
}

void CMidpScreenControl::Draw(const TRect& aRect) const
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if (iCanvas->iDrawingState == CMIDPCanvas::KWaitingForDisplayableChange)
	{
		return;
	}
	
	if (iCanvas->iDrawingState == CMIDPCanvas::KWaitingForRepositioning)
	{
		TBool changed = iCanvas->CalculateCanvasRect();
		if (changed)
		{
			// catch the case where SetCbaL has not been called after going fullscreen
			TEventInfo event;
			event.iEvent = KMIDPScreenChangeEvent;
			iCanvas->iQueue->AddEvent(event);
		}

		iCanvas->SetExtent(iCanvas->iCanvasRect.iTl, iCanvas->iCanvasRect.Size());
		iCanvas->iDrawingState = CMIDPCanvas::KDrawingNormally;
	}
#endif
	CWindowGc& gc(SystemGc());
	DrawBitmap(aRect,gc);
}

void CMidpScreenControl::HandleCommandL(TInt /*aCommand*/)
{
}
