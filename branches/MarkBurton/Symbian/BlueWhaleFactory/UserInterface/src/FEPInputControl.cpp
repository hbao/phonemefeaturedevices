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


#include <coemain.h>
#include <fepbase.h>
#include <coeinput.h>
#include <fepitfr.h>
#include "FEPInputControl.h"
#include <frmtlay.h>
#include <keymap_input.h>
#include <OSVersion.h>
#include "Application.h"

#ifdef __WINSCW__
#define __DEBUG_FEP_INPUTCONTROL__
#endif

#ifdef __DEBUG_FEP_INPUTCONTROL__
#define	DEBUGMSG(_XX) RDebug::Print(_XX)
#define	DEBUGMSG1(_XX,_AA) RDebug::Print(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB) RDebug::Print(_XX,_AA,_BB)
#define	DEBUGMSG4(_XX,_AA,_BB,_CC,_DD) RDebug::Print(_XX,_AA,_BB,_CC,_DD)
#define	DEBUGMSG5(_XX,_AA,_BB,_CC,_DD,_EE) RDebug::Print(_XX,_AA,_BB,_CC,_DD,_EE)
#else
#define	DEBUGMSG(_XX)
#define	DEBUGMSG1(_XX,_AA)
#define	DEBUGMSG2(_XX,_AA,_BB)
#define	DEBUGMSG4(_XX,_AA,_BB,_CC,_DD)
#define	DEBUGMSG5(_XX,_AA,_BB,_CC,_DD,_EE)
#endif

#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif

const TInt Capability[] = 
{
	TCoeInputCapabilities::EAllText, 						// TextField.ANY
	TCoeInputCapabilities::EAllText,						// TextField.EMAILADDR
	TCoeInputCapabilities::EWesternNumericIntegerPositive | 
	TCoeInputCapabilities::EWesternNumericIntegerNegative,	// TextField.NUMERIC
	TCoeInputCapabilities::EDialableCharacters,				// TextField.PHONENUMBER
	TCoeInputCapabilities::EAllText,						// TextField.URL
	TCoeInputCapabilities::EWesternNumericReal,				// TextField.DECIMAL
};

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
const TInt Mode[] =
{
	EAknEditorTextInputMode,	// TextField.ANY
	EAknEditorTextInputMode,	// TextField.EMAILADDR
	EAknEditorNumericInputMode,	// TextField.NUMERIC
	EAknEditorTextInputMode,	// TextField.PHONENUMBER
	EAknEditorTextInputMode,	// TextField.URL
	EAknEditorNumericInputMode,	// TextField.DECIMAL
};

const TInt PermittedModes[] =
{
	EAknEditorAllInputModes,	// TextField.ANY
	EAknEditorAllInputModes,	// TextField.EMAILADDR
	EAknEditorNumericInputMode,	// TextField.NUMERIC
	EAknEditorAllInputModes,	// TextField.PHONENUMBER
	EAknEditorAllInputModes,	// TextField.URL
	EAknEditorNumericInputMode,	// TextField.DECIMAL
};
#endif

typedef enum {
    MIDP_MODIFIER_PASSWORD				= 0x10000,
    MIDP_MODIFIER_UNEDITABLE	 		= 0x20000,
    MIDP_MODIFIER_SENSITIVE	 			= 0x40000,
    MIDP_MODIFIER_NON_PREDICTIVE		= 0x80000,
    MIDP_MODIFIER_INITIAL_CAPS_WORD		= 0x100000,
    MIDP_MODIFIER_INITIAL_CAPS_SENTENCE = 0x200000
} MidpModifier;

EXPORT_DECL CIndexedText::CIndexedText(TInt aId, HBufC* aText)
: iId(aId), iText(aText), iParentId(0)
{
}

CIndexedText::~CIndexedText()
{
	delete iText;
}

TInt CIndexedText::Order(const CIndexedText& aIndexedText1, const CIndexedText& aIndexedText2)
{
	return aIndexedText1.iId - aIndexedText2.iId;
}

EXPORT_DECL HBufC* CIndexedText::TextPtr()
{
	return iText;
}

void CIndexedText::SetTextPtr(HBufC* aText)
{
	delete iText;
	iText = aText;
}

void CIndexedText::ReAllocL(TInt aSize)
{
	iText = iText->ReAllocL(aSize);
}

TInt CIndexedText::Id()
{
	return iId;
}

void CFEPInputCore::SignalMutex(TAny* aPtr)
{
	static_cast<RMutex*>(aPtr)->Signal();
}

EXPORT_DECL CFEPInputCore* CFEPInputCore::NewL(CIndexedText* aText, MFEPInputCoreObserver* aObserver, MKeyMapper* aKeyMapper)
{
	CFEPInputCore* self = new (ELeave) CFEPInputCore(aText, aObserver, aKeyMapper);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CFEPInputCore::CFEPInputCore(CIndexedText* aText, MFEPInputCoreObserver* aObserver, MKeyMapper* aKeyMapper)
: iText(aText), iObserver(aObserver), iKeyMapper(aKeyMapper)
{
	iCursorForFEP.iAnchorPos = iText->EditPoint();
	iCursorForFEP.iCursorPos = iCursorForFEP.iAnchorPos;
	iLastCursorPosition = -1;
	iLastCharCode = 0;
}

CFEPInputCore::~CFEPInputCore()
{
	iText->SetEditPoint(iCursorForFEP.iAnchorPos);
	iMutex.Close();
}

void CFEPInputCore::ConstructL()
{
	User::LeaveIfError(iMutex.CreateLocal());
}

EXPORT_DECL TInt CFEPInputCore::GetNextState()
{
	TInt ret = MEDIATOR_NOOP;
	if (iDeleteSeen)
	{
		if (!iInSession)
		{
			ret = MEDIATOR_CLEAR;
		}
		iDeleteSeen = EFalse;
	}
	else if (iTextBufferUpdated)
	{
		DEBUGMSG(_L("CFEPInputCore::GetNextState MEDIATOR_UPDATE"));
		ret = MEDIATOR_UPDATE;
		iTextBufferUpdated = EFalse;
	}
	return ret;
}

EXPORT_DECL TInt CFEPInputCore::DocumentLengthForFep() const
{
	iMutex.Wait();
	TInt ret = iText ? iText->TextPtr()->Length() : 0;
	iMutex.Signal();
	DEBUGMSG1(_L("CFEPInputCore::DocumentLengthForFep %d"), ret); 
	return ret;
}

EXPORT_DECL void CFEPInputCore::StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, const TCursorSelection& aCursorSelection)
{
	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);
	DEBUGMSG4(_L("CFEPInputCore::StartFepInlineEditL :%S: %d %d %d"), &aInitialInlineText, aPositionOfInsertionPointInInlineText, aCursorSelection.iAnchorPos, aCursorSelection.iCursorPos);
	ASSERT(iText);
	iInSession = ETrue;
	if (aCursorSelection.Length())
	{
#if __S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__	
		if (!iObserver->NumericText() && !iObserver->PredictiveText())
		{
			// in multitap mode on some older phones the ptiengine creates a second "echo" fep edit - ignore
			CleanupStack::PopAndDestroy(&iMutex);
			return;
		}
#endif
		iCursorForFEP = aCursorSelection;
	}

	InsertTextL(aInitialInlineText);
	CleanupStack::PopAndDestroy(&iMutex);
}

void CFEPInputCore::DeUnicodeBuffer(TPtr aTextBuffer)
{
	TInt length = aTextBuffer.Length();
	for(TInt i=0;i<length;i++)
	{
		if(aTextBuffer[i] == CEditableText::EParagraphDelimiter)
		{
			aTextBuffer[i] = '\n';
		}
	}
}

void CFEPInputCore::InsertTextL(const TDesC& aNewText)
{
	if (aNewText.Length() == 1 && !iObserver->FieldAllowsChar(aNewText[0]))
	{
		return;
	}
	ASSERT(iText);
	TInt skew = iCursorForFEP.HigherPos() - iText->TextPtr()->Length();
	if (skew > 0)
	{
		// iCursorForFEP is skewed - occurs during password entry
		// where the original asterisk text has been deleted
		// via CMIDPCanvas::SetTextFieldString while a fep edit is occurring
		iCursorForFEP.iAnchorPos -= skew;
		iCursorForFEP.iCursorPos -= skew;
	}
	
	// delete existing selection
	if (iCursorForFEP.Length() > 0)
	{
		iText->TextPtr()->Des().Delete(iCursorForFEP.LowerPos(), iCursorForFEP.Length());
	}
		
	// realloc buffer
	TInt newTextLength = aNewText.Length();
	TInt currentLen = iText->TextPtr()->Length();

	if (iText->TextPtr()->Des().MaxLength() < currentLen + newTextLength)
	{
		iText->ReAllocL(currentLen + newTextLength);
	}

	// insert new text
	iText->TextPtr()->Des().Insert(iCursorForFEP.LowerPos(), aNewText);
	DeUnicodeBuffer(iText->TextPtr()->Des().MidTPtr(iCursorForFEP.LowerPos(), newTextLength));
	if (iCursorForFEP.iCursorPos >= iCursorForFEP.iAnchorPos)
	{
		iCursorForFEP.iCursorPos = iCursorForFEP.iAnchorPos + newTextLength;
	}
	else
	{
		iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos + newTextLength;
	}
	if (!iInSession)
	{
		iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
	}
	SendTextUpdateEvent();
}

EXPORT_DECL void CFEPInputCore::UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText)
{
	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);
	DEBUGMSG2(_L("CFEPInputCore::UpdateFepInlineTextL :%S: %d"), &aNewInlineText, aPositionOfInsertionPointInInlineText);
	InsertTextL(aNewInlineText);
	DEBUGMSG1(_L("CFEPInputCore::UpdateFepInlineTextL :%S:"), iText->TextPtr());
	CleanupStack::PopAndDestroy(&iMutex);
}

EXPORT_DECL void CFEPInputCore::DoCommitFepInlineEditL()
{
	iMutex.Wait();
	DEBUGMSG(_L("CFEPInputCore::DoCommitFepInlineEditL"));
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iObserver->MachineUidValue() == KUidZ10Value)
	{
		iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
	}
	else
	{
		iCursorForFEP.iCursorPos = iCursorForFEP.iAnchorPos;
	}
#endif
	iInSession = EFalse;
	SendTextUpdateEvent();
	iMutex.Signal();
}

EXPORT_DECL void CFEPInputCore::CancelFepInlineEdit()
{
	iMutex.Wait();
	DEBUGMSG(_L("CFEPInputCore::CancelFepInlineEditL"));
	iText->TextPtr()->Des().Delete(iCursorForFEP.LowerPos(), iCursorForFEP.Length());
	iCursorForFEP.iCursorPos = iCursorForFEP.iAnchorPos;
	iInSession = EFalse;
	SendTextUpdateEvent();
	iMutex.Signal();
}

void CFEPInputCore::SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection)
{
	iMutex.Wait();
	DEBUGMSG2(_L("CFEPInputCore::SetCursorSelectionForFepL %d %d"), aCursorSelection.iAnchorPos, aCursorSelection.iCursorPos);
	if (iObserver->MachineUidValue() == KUidW960iValue)
	{
		if (!iInSession)
		{
			if (iLastCursorPosition != -1)
			{
				iCursorForFEP.iAnchorPos = iLastCursorPosition;
				iCursorForFEP.iCursorPos = iLastCursorPosition;
				iLastCursorPosition = -1;
			}
			else
			{
				iCursorForFEP.iAnchorPos = aCursorSelection.iAnchorPos;
				iCursorForFEP.iCursorPos = aCursorSelection.iCursorPos;
			}
		}
	}
	else if ((iObserver->MachineUidValue() != KUidZ10Value)  || !iInSession)
	{
		iCursorForFEP.iAnchorPos = aCursorSelection.iAnchorPos;
		iCursorForFEP.iCursorPos = aCursorSelection.iCursorPos;
	}
	iMutex.Signal();
}

EXPORT_DECL void CFEPInputCore::GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const
{
	iMutex.Wait();
	DEBUGMSG2(_L("CFEPInputCore::GetCursorSelectionForFep %d %d"), iCursorForFEP.iCursorPos, iCursorForFEP.iCursorPos);
	aCursorSelection.iAnchorPos = iCursorForFEP.iCursorPos;
	aCursorSelection.iCursorPos = iCursorForFEP.iCursorPos;
	iMutex.Signal();
}

void CFEPInputCore::GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const
{
	iMutex.Wait();
	DEBUGMSG2(_L("CFEPInputCore::GetEditorContentForFep %d %d"), aDocumentPosition, aLengthToRetrieve);
	if (iText && (aDocumentPosition + aLengthToRetrieve) <= iText->TextPtr()->Length() && aDocumentPosition >= 0)
	{
		aEditorContent.Copy(iText->TextPtr()->Mid(aDocumentPosition, aLengthToRetrieve));
	}
	else if (iText && aLengthToRetrieve <= iText->TextPtr()->Length())
	{
		// aDocumentPosition is skewed - occurs during password entry
		// where the original asterisk text has been deleted
		// via CMIDPCanvas::SetTextFieldString while a fep edit is occurring
		aEditorContent.Copy(iText->TextPtr()->Mid(iText->TextPtr()->Length() - aLengthToRetrieve , aLengthToRetrieve));
	}
	iMutex.Signal();
}

EXPORT_DECL TKeyResponse CFEPInputCore::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	iMutex.Wait();
	TCleanupItem cleanupItem(SignalMutex, &iMutex);
	CleanupStack::PushL(cleanupItem);
	DEBUGMSG5(_L("CFEPInputCore::OfferKeyEvent %d %d %d %d %d"), aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iRepeats, aKeyEvent.iModifiers, aType);
	TKeyResponse response = EKeyWasNotConsumed;
	TChar ch = iKeyMapper->CharFromScanCode(aKeyEvent);
	
	if ((iObserver->Capabilities() & MIDP_MODIFIER_PASSWORD) && !iText->ModifiedByUser())
	{
		if (TInt(ch) > 0 || ch == KEYMAP_KEY_CLEAR || (TInt(ch) == 0 && aKeyEvent.iModifiers == 0))
		{
			// user is starting to modify the password (ie has entered a printable char)
			// so clear the asterisked text
			iText->TextPtr()->Des().Zero();
			iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos = 0;
			iText->SetModifiedByUser(ETrue);
			SendTextUpdateEvent();
		}
	}
	
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iObserver->MachineUidValue() == KUidW960iValue)
	{
		if ((ch == KEYMAP_KEY_UP) && (iCursorForFEP.iCursorPos > 0) && !iInSession)
		{
			ch = KEYMAP_KEY_LEFT;
		}
		else if ((ch == KEYMAP_KEY_DOWN) && (iCursorForFEP.iCursorPos < iText->TextPtr()->Length()) && !iInSession)
		{
			ch = KEYMAP_KEY_RIGHT;
		}
	}
#endif
	if (aKeyEvent.iCode == 13)
	{
		// use simple insert and update rather than StartFepInlineEditL/DoCommitFepInlineEditL
		// because the return key event is timing sensitive and often arrives during the next edit session
		TBuf<1> buffer;
		buffer.Append('\n');
		iText->ReAllocL(iText->TextPtr()->Length() + 1);
		iText->TextPtr()->Des().Insert(iCursorForFEP.iAnchorPos, buffer);
		iCursorForFEP.iCursorPos += 1;
		iCursorForFEP.iAnchorPos += 1;
		SendTextUpdateEvent();
		CleanupStack::PopAndDestroy(&iMutex);
		return EKeyWasConsumed;
	}
	if (iInSession && ch != KEYMAP_KEY_CLEAR && aKeyEvent.iCode != 8)
	{
		CleanupStack::PopAndDestroy(&iMutex);
		return response;
	}
	if (!iObserver->FieldAllowsChar(ch) && aType != EEventKey)
	{
		CleanupStack::PopAndDestroy(&iMutex);
		return response;
	}	

	TEventInfo event;
	switch(aType)
	{
	case EEventKeyDown:
		iCurrentChar = ch;
		if (iObserver->MachineUidValue() == KUidZ10Value && ch == KEYMAP_KEY_CLEAR && iText->TextPtr()->Length())
		{
			iDeleteSeen = ETrue;
			SendTextUpdateEvent();
		}
		else if (aKeyEvent.iModifiers == 0 || iObserver->MachineUidValue() == KUidW960iValue) 
		{
			event.iEvent = KMIDPKeyDownEvent;
			event.iIntParam1 = ch;
			DEBUGMSG1(_L("KMIDPKeyDownEvent %d"),ch);
			iObserver->AddEvent(event);
		}
		response = EKeyWasConsumed;
		break;
	case EEventKeyUp:
		iCurrentChar = 0;
		if (iObserver->MachineUidValue() == KUidZ10Value && ch == KEYMAP_KEY_CLEAR)
		{
			// do nothing
		}
		else if (aKeyEvent.iModifiers == 0 || iObserver->MachineUidValue() == KUidW960iValue) 
		{
			event.iEvent = KMIDPKeyUpEvent;
			event.iIntParam1 = ch;
			DEBUGMSG1(_L("KMIDPKeyUpEvent %d"),ch);
			iObserver->AddEvent(event);
		}
		response = EKeyWasConsumed;
		break;
	case EEventKey:
		if (aKeyEvent.iCode == 8)
		{
			if ((iCursorForFEP.iCursorPos > 0) && (iCurrentChar <= 0 || iObserver->FieldAllowsChar(iLastCharCode)))
			{
				iCursorForFEP.iCursorPos--;
				iText->TextPtr()->Des().Delete(iCursorForFEP.iCursorPos, 1);
				iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
				SendTextUpdateEvent();
			}
		}
		else if (iObserver->FieldAllowsChar(aKeyEvent.iCode) && aKeyEvent.iCode < ENonCharacterKeyBase)
		{
			TBuf<1> buffer;
			buffer.Append(aKeyEvent.iCode);
			iText->ReAllocL(iText->TextPtr()->Length() + 1);
			iText->TextPtr()->Des().Insert(iCursorForFEP.iCursorPos, buffer);
			iCursorForFEP.iCursorPos += 1;
			iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
			SendTextUpdateEvent();
		}
		else if (aKeyEvent.iRepeats > 0)
		{
			for (TInt i = 0; i < aKeyEvent.iRepeats; i++)
			{
				if (ch == KEYMAP_KEY_CLEAR)
				{
					iDeleteSeen = ETrue;
					SendTextUpdateEvent();
				}
				else
				{
					event.iEvent = KMIDPKeyRepeatEvent;
					event.iIntParam1 = ch;
					DEBUGMSG1(_L("KMIDPKeyRepeatEvent %d"),ch);
					iObserver->AddEvent(event);
				}
			}
		}
		iLastCharCode = aKeyEvent.iCode;
		response = EKeyWasConsumed;
		break;
	}
	CleanupStack::PopAndDestroy(&iMutex);
	return response;
}

void CFEPInputCore::SendTextUpdateEvent()
{
	TEventInfo event;
	event.iEvent = KMIDPKeyDownEvent;
	event.iIntParam1 = 0;
	iObserver->AddEvent(event);
	event.iEvent = KMIDPKeyUpEvent;
	event.iIntParam1 = 0;
	iObserver->AddEvent(event);
	iTextBufferUpdated = ETrue;	
}

void CFEPInputCore::SetCursorPosition(TInt aPosition)
{
	iMutex.Wait();
	DEBUGMSG1(_L("CFEPInputCore::SetCursorPosition %d"), aPosition);
	if (aPosition >= iCursorForFEP.LowerPos() && aPosition <= iCursorForFEP.HigherPos())
	{
		iLastCursorPosition = -1;
	}
	else
	{	
		iLastCursorPosition = aPosition;
	}

	if (!iInSession)
	{
		iCursorForFEP.iCursorPos = aPosition;
		iCursorForFEP.iAnchorPos = iCursorForFEP.iCursorPos;
	}
	iMutex.Signal();
}

TInt CFEPInputCore::CursorPosition()
{
	iMutex.Wait();
	TInt ret(iCursorForFEP.LowerPos());
	DEBUGMSG1(_L("CFEPInputCore::CursorPosition %d"), ret);
	iMutex.Signal();
	return ret;
}

TCursorSelection CFEPInputCore::GetEditData()
{
	iMutex.Wait();
	TCursorSelection result(iCursorForFEP);
	DEBUGMSG2(_L("CFEPInputCore::GetEditData %d %d"), iCursorForFEP.LowerPos(), iCursorForFEP.HigherPos());
	iMutex.Signal();
	return result;
}

CFEPInputControl* CFEPInputControl::NewL(CIndexedText* aText, MEventQueue* aQueue, MKeyMapper* aKeyMapper)
{
	CFEPInputControl* self = new (ELeave) CFEPInputControl(aQueue);
	CleanupStack::PushL(self);
	self->ConstructL(aText, aKeyMapper);
	CleanupStack::Pop();
	return self;
}

CFEPInputControl::CFEPInputControl(MEventQueue* aQueue)
: iQueue(aQueue)
{}

CFEPInputControl::~CFEPInputControl()
{
	delete iState;
	delete iFEPInputCore;
	delete iThreadRunner;
}

TBool CFEPInputControl::FieldAllowsChar(TChar aChar) const
{
	if (NumericText() && !InputCapabilities().SupportsAllText() && !aChar.IsDigit() && TInt(aChar) > 0)
	{
		// numeric-only field and non-digit, non-control char
		return EFalse;
	}
	else if (TInt(aChar) == 0)
	{
		return EFalse;
	}
	else
	{
		return ETrue;
	}
}

void CFEPInputControl::AddEvent(TEventInfo& aInfo)
{
	if (iQueue)
	{
		iQueue->AddEvent(aInfo);
	}
}

TInt CFEPInputControl::MachineUidValue() const
{
	return iMachineUidValue;
}

TUint CFEPInputControl::Capabilities() const
{
	return iCapabilities;
}

void CFEPInputControl::ConstructL(CIndexedText* aText, MKeyMapper* aKeyMapper)
{
	iInputCapabilities = TCoeInputCapabilities::EAllText;
	iFepActive = EFalse;
	iCapabilities = 2;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	MAknEditingStateIndicator* ei = CAknEnv::Static()->EditingStateIndicator();
	iIc = ei->IndicatorContainer();	
	iState= new (ELeave) CAknEdwinState();

 	iState->SetObjectProvider(this);
	iState->SetDefaultCase(EAknEditorLowerCase);
	iState->SetFlags(EAknEditorFlagNoEditIndicators );
	iState->SetPermittedInputModes(EAknEditorAllInputModes);
	iState->SetDefaultInputMode(EAknEditorNumericInputMode);
	iState->SetNumericKeymap(EAknEditorSATHiddenNumberModeKeymap);
	iState->SetMenu();
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	HAL::Get(HALData::EMachineUid, iMachineUidValue);
#endif	
	iFEPInputCore = CFEPInputCore::NewL(aText, this, aKeyMapper);
	iThreadRunner = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL();
}

void CFEPInputControl::SetCapabilities(TInt aCapabilities)
{
	DEBUGMSG1(_L("CFEPInputControl::SetCapabilities 0x%08x"),aCapabilities);
	iCapabilities = aCapabilities;
	TInt flags = 0;
	iInputCapabilities = Capability[iCapabilities & 0xffff];
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	iState->SetPermittedInputModes(PermittedModes[iCapabilities & 0xffff]);
	iState->SetCurrentInputMode(Mode[iCapabilities & 0xffff]);
	iState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateInputModeUpdate);
	if(iCapabilities & MIDP_MODIFIER_INITIAL_CAPS_SENTENCE)
	{
		iState->SetPermittedCases(EAknEditorAllCaseModes);
		iState->SetDefaultCase(EAknEditorTextCase);
		iState->SetCurrentCase(EAknEditorTextCase);
		iState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateCaseModeUpdate);
	}
	if(iCapabilities & MIDP_MODIFIER_NON_PREDICTIVE)
	{
		flags |= EAknEditorFlagNoT9;
	}
	iState->SetFlags(flags);
	iState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateFlagsUpdate);
#endif
	SetFocus(ETrue);
	iFepActive = ETrue;
}

void CFEPInputControl::CancelFep()
{
	DEBUGMSG(_L("CFEPInputControl::CancelFep"));
	iCapabilities = -1;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	iState->SetCurrentInputMode(EAknEditorNumericInputMode);
	iState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateInputModeUpdate);
	iState->SetFlags(EAknEditorFlagNoEditIndicators);
	iState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateFlagsUpdate);
#endif
	SetFocus(ETrue);
	iFepActive = EFalse;
}

TCoeInputCapabilities CFEPInputControl::InputCapabilities() const
{
	DEBUGMSG1(_L("InputCapabilities %d"),iInputCapabilities);
	if(iInputCapabilities != -1)
	{
		return TCoeInputCapabilities(iInputCapabilities,const_cast<CFEPInputControl*>(this),NULL);
	}
	else
	{
		return TCoeInputCapabilities(ENone,NULL,NULL);
	}
}

TBool CFEPInputControl::PredictiveText() const
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if(iIc->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorT9)))
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
#else
	return EFalse;
#endif
}

TBool CFEPInputControl::NumericText() const
{
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	if(iState && iState->CurrentInputMode() & EAknEditorNumericInputMode)
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
#else
	return ETrue;
#endif
}

TBool CFEPInputControl::GetUnderline()
{
	iThreadRunner->DoSyncCallback(UpdateUnderlineCallback, this);
	return iUnderline;
}

void CFEPInputControl::UpdateUnderlineCallback(TAny* aThis)
{	
	CFEPInputControl* This = static_cast<CFEPInputControl*>(aThis);
	This->iUnderline = EFalse;
	if (This->iInlineTextFormatRetriever)
	{
		TCharFormat format;
		TInt numberOfCharactersWithSameFormat;
		This->iInlineTextFormatRetriever->GetFormatOfFepInlineText(format, numberOfCharactersWithSameFormat, 0);
		This->iUnderline = (format.iFontPresentation.iUnderline == EUnderlineOn);
	}
}

TKeyResponse CFEPInputControl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	return iFEPInputCore->OfferKeyEventL(aKeyEvent, aType);
}

MCoeFepAwareTextEditor_Extension1* CFEPInputControl::Extension1(TBool& aSetToTrue)
{
	aSetToTrue = ETrue;
	return this;
}

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
void CFEPInputControl::SetStateTransferingOwnershipL(CState* aState, TUid /*aTypeSafetyUid*/)
#else
void CFEPInputControl::SetStateTransferingOwnershipL(CState* /*aState*/, TUid /*aTypeSafetyUid*/)
#endif
{
	DEBUGMSG(_L("SetStateTransferingOwnershipL"));
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	iState = aState;
#endif
}

MCoeFepAwareTextEditor_Extension1::CState* CFEPInputControl::State(TUid /*aTypeSafetyUid*/)
{
	return iState;
}

void CFEPInputControl::CancelFEPTransaction()
{
	if(iCurrentFEP)
	{
		iCurrentFEP->CancelTransaction();
	}
}

void CFEPInputControl::StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, 
											TBool /*aCursorVisibility*/, const MFormCustomDraw* /*aCustomDraw*/, 
											MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, 
											MFepPointerEventHandlerDuringInlineEdit& /*aPointerEventHandlerDuringInlineEdit*/)
{
	iCurrentFEP = CCoeEnv::Static()->Fep();
	iFEPInputCore->StartFepInlineEditL(aInitialInlineText, aPositionOfInsertionPointInInlineText, TCursorSelection(0, 0));
	iInlineTextFormatRetriever = &aInlineTextFormatRetriever;
}

void CFEPInputControl::StartFepInlineEditL(TBool& aSetToTrue, const TCursorSelection& aCursorSelection, const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool /*aCursorVisibility*/, const MFormCustomDraw* /*aCustomDraw*/, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& /*aPointerEventHandlerDuringInlineEdit*/)
{
	iFEPInputCore->StartFepInlineEditL(aInitialInlineText, aPositionOfInsertionPointInInlineText, aCursorSelection);
	iInlineTextFormatRetriever = &aInlineTextFormatRetriever;
	aSetToTrue = ETrue;
}

void CFEPInputControl::UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText)
{	
	iFEPInputCore->UpdateFepInlineTextL(aNewInlineText, aPositionOfInsertionPointInInlineText);
}

void CFEPInputControl::SetInlineEditingCursorVisibilityL(TBool aCursorVisibility)
{
	DEBUGMSG1(_L("SetInlineEditingCursorVisibilityL %d"),aCursorVisibility);
}

void CFEPInputControl::CancelFepInlineEdit()
{
	iFEPInputCore->CancelFepInlineEdit();
	iInlineTextFormatRetriever = NULL;
}

TInt CFEPInputControl::DocumentLengthForFep() const
{
	return iFEPInputCore->DocumentLengthForFep();
}

TInt CFEPInputControl::DocumentMaximumLengthForFep() const
{
	TInt len = 0;
	DEBUGMSG1(_L("DocumentMaximumLengthForFep %d"),len);
	return len;
}

void CFEPInputControl::SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection)
{
	iFEPInputCore->SetCursorSelectionForFepL(aCursorSelection);
}

void CFEPInputControl::GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const
{
	iFEPInputCore->GetCursorSelectionForFep(aCursorSelection);
}

void CFEPInputControl::GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const
{
	iFEPInputCore->GetEditorContentForFep(aEditorContent,aDocumentPosition,aLengthToRetrieve);
}

void CFEPInputControl::GetFormatForFep(TCharFormat& aFormat, TInt /*aDocumentPosition*/) const
{
	DEBUGMSG(_L("GetFormatForFep"));
	aFormat = TCharFormat();
}

void CFEPInputControl::GetScreenCoordinatesForFepL(TPoint& /*aLeftSideOfBaseLine*/, TInt& /*aHeight*/, TInt& /*aAscent*/, TInt /*aDocumentPosition*/) const
{
	DEBUGMSG(_L("GetScreenCoordinatesForFepL"));
}

void CFEPInputControl::DoCommitFepInlineEditL()
{
	iFEPInputCore->DoCommitFepInlineEditL();
	iInlineTextFormatRetriever = NULL;
}

void CFEPInputControl::SetCursorType(TBool& aSetToTrue, const TTextCursor& /*aTextCursor*/)
{
	DEBUGMSG(_L("SetCursorType"));
	aSetToTrue = ETrue;
}

MCoeFepLayDocExtension* CFEPInputControl::GetFepLayDocExtension(TBool& aSetToTrue)
{
	DEBUGMSG(_L("GetFepLayDocExtension"));
	aSetToTrue = ETrue;
	return 0;
}

TInt CFEPInputControl::GetNextState()
{
	return iFEPInputCore->GetNextState();
}

void CFEPInputControl::SetCursorPosition(TInt aPosition)
{
	iFEPInputCore->SetCursorPosition(aPosition);
}

TInt CFEPInputControl::CursorPosition()
{
	return iFEPInputCore->CursorPosition();
}

TCursorSelection CFEPInputControl::GetEditData()
{
	return iFEPInputCore->GetEditData();
}
