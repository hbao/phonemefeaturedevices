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
#include "TestFEP.h"
#include <OSVersion.h>
#include "Application.h"

TBool CTestFEP::PredictiveText() const
{
	return iPredictiveText;
}

TBool CTestFEP::NumericText() const
{
	return iNumericText;
}

TBool CTestFEP::FieldAllowsChar(TChar /*aChar*/) const
{
	return ETrue;
}

void CTestFEP::AddEvent(TEventInfo& /*aInfo*/)
{
}

TInt CTestFEP::MachineUidValue() const
{
	return iMachineUidValue;
}

TUint CTestFEP::Capabilities() const
{
	return 0;
}

	
void CTestFEP::setUp()
{
	HBufC* buffer = HBufC::NewL(1);
	iText = new (ELeave) CIndexedText(0, buffer);
	iKeyMapper = TKeyMapperFactory::GetPlatformKeyMapperL();
	iFEPInputCore = CFEPInputCore::NewL(iText, this, iKeyMapper);
}

void CTestFEP::tearDown()
{
	delete iFEPInputCore;
	iKeyMapper->Release();
	delete iText;
}

void CTestFEP::CallOfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType, TInt aAssertLength)
{
	iFEPInputCore->OfferKeyEventL(aKeyEvent, aType);
	TCursorSelection selection;
	iFEPInputCore->GetCursorSelectionForFep(selection);
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	TS_ASSERT(iFEPInputCore->DocumentLengthForFep() == aAssertLength);
	TS_ASSERT(selection.iCursorPos == aAssertLength);
	TS_ASSERT(selection.iAnchorPos == aAssertLength);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iMachineUidValue == KUidZ10Value)
	{
		TS_ASSERT(iFEPInputCore->DocumentLengthForFep() == aAssertLength);
		TS_ASSERT(selection.iCursorPos == aAssertLength);
		TS_ASSERT(selection.iAnchorPos == aAssertLength);		
	}
#endif
}

void CTestFEP::CallUpdateFepInlineTextL(const TDesC& aNewInlineText, const TDesC& aAssertText)
{
	iFEPInputCore->UpdateFepInlineTextL(aNewInlineText, aNewInlineText.Length());
	TS_ASSERT(iFEPInputCore->DocumentLengthForFep() == aAssertText.Length());
	TCursorSelection selection;
	iFEPInputCore->GetCursorSelectionForFep(selection);
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__	
	TS_ASSERT(selection.iCursorPos == aAssertText.Length());
	TS_ASSERT(selection.iAnchorPos == aAssertText.Length());
	TS_ASSERT(iText->TextPtr()->Compare(aAssertText) == 0);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iMachineUidValue == KUidZ10Value)
	{
		TS_ASSERT(selection.iCursorPos == aAssertText.Length());
		TS_ASSERT(selection.iAnchorPos == aAssertText.Length());
		TS_ASSERT(iText->TextPtr()->Compare(aAssertText) == 0);
	}	
#endif
}

void CTestFEP::CallDoCommitFepInlineEditL(TInt aAssertLength)
{
	TCursorSelection oldSelection;
	iFEPInputCore->GetCursorSelectionForFep(oldSelection);
	iFEPInputCore->DoCommitFepInlineEditL();
	TS_ASSERT(iFEPInputCore->DocumentLengthForFep() == aAssertLength);
	TCursorSelection selection;
	iFEPInputCore->GetCursorSelectionForFep(selection);
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	TS_ASSERT(selection.iCursorPos == aAssertLength);
	TS_ASSERT(selection.iAnchorPos == aAssertLength);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iMachineUidValue == KUidZ10Value)
	{
		TS_ASSERT(selection.iCursorPos == aAssertLength);
		TS_ASSERT(selection.iAnchorPos == aAssertLength);
	}
#endif
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
}


void CTestFEP::testPredictive1()
{
	// Simulate the CFEPInputCore calls for some S60 predictive text entry (the phrase "I am happy", linebreaking after "am")
	// Calls and parameters are taken from the emulator console output
	iPredictiveText = ETrue;
	iNumericText = EFalse;
	iText->TextPtr()->Des().Zero();
	
	TKeyEvent keyEvent;
	keyEvent.iCode = 0;
	keyEvent.iScanCode = '4';
	keyEvent.iRepeats = 0;
	keyEvent.iModifiers = 0;

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 0);
	
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(KNullDesC, KNullDesC);
	CallUpdateFepInlineTextL(_L("i"), _L("i"));

	CallOfferKeyEventL(keyEvent, EEventKeyUp, 1);

	keyEvent.iScanCode = '0';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 1);
	
	CallDoCommitFepInlineEditL(1);
	
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(_L(""), _L("i"));
	CallUpdateFepInlineTextL(_L(" "), _L("i "));

	CallOfferKeyEventL(keyEvent, EEventKeyUp, 2);

	CallDoCommitFepInlineEditL(2);
	
	keyEvent.iScanCode = '2';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 2);
	
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(_L(""), _L("i "));
	CallUpdateFepInlineTextL(_L("a"), _L("i a"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);

	keyEvent.iScanCode = '6';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 3);
	CallUpdateFepInlineTextL(_L("an"), _L("i an"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 4);

	keyEvent.iScanCode = 133;	// star key to select alternative word
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 4);
	CallUpdateFepInlineTextL(_L("am"), _L("i am"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 4);
	
	keyEvent.iScanCode = '0';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 4);
	CallDoCommitFepInlineEditL(4);
	
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(_L(""), _L("i am"));
	CallUpdateFepInlineTextL(_L(" "), _L("i am "));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 5);
	
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 5);
	CallUpdateFepInlineTextL(_L("0"), _L("i am0"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 5);

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 5);
	CallUpdateFepInlineTextL(_L("\xB2"), _L("i am\xB2"));	// down left arrow
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 5);
	iFEPInputCore->CancelFepInlineEdit();
	
	keyEvent.iCode = 13;
	keyEvent.iScanCode = 0;
	keyEvent.iModifiers = 262144;
	CallOfferKeyEventL(keyEvent, EEventKey, 5);	// line break

	keyEvent.iCode = 0;
	keyEvent.iScanCode = '4';
	keyEvent.iModifiers = 0;
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 5);
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(_L(""), _L("i am\n"));
	CallUpdateFepInlineTextL(_L("i"), _L("i am\ni"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 6);

	keyEvent.iScanCode = '2';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 6);
	CallUpdateFepInlineTextL(_L("ha"), _L("i am\nha"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 7);
	
	keyEvent.iScanCode = '7';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 7);
	CallUpdateFepInlineTextL(_L("has"), _L("i am\nhas"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 8);
	
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 8);
	CallUpdateFepInlineTextL(_L("gaps"), _L("i am\ngaps"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 9);
	
	keyEvent.iScanCode = '9';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 9);
	CallUpdateFepInlineTextL(_L("happy"), _L("i am\nhappy"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 10);
	
	// delete the last letter
	keyEvent.iScanCode = '1';	
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 10);
	CallUpdateFepInlineTextL(_L("gaps"), _L("i am\ngaps"));
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 9);
}

void CTestFEP::testMultiTap1()
{
	// Simulate the CFEPInputCore calls for some S60 multitap text entry (the word "text")
	// Calls and parameters are taken from the emulator console output
	iPredictiveText = EFalse;
	iNumericText = EFalse;
	iText->TextPtr()->Des().Zero();
	
	TKeyEvent keyEvent;
	keyEvent.iCode = 0;
	keyEvent.iScanCode = '8';
	keyEvent.iRepeats = 0;
	keyEvent.iModifiers = 0;

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 0);
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(KNullDesC, KNullDesC);
	CallUpdateFepInlineTextL(_L("t"), _L("t"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 1);
	CallDoCommitFepInlineEditL(1);
	
	keyEvent.iScanCode = '3';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 1);
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(KNullDesC, _L("t"));
	CallUpdateFepInlineTextL(_L("d"), _L("td"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 2);

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 2);
	CallUpdateFepInlineTextL(_L("e"), _L("te"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 2);
	CallDoCommitFepInlineEditL(2);

	keyEvent.iScanCode = '9';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 2);
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(KNullDesC, _L("te"));
	CallUpdateFepInlineTextL(_L("w"), _L("tew"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 3);
	CallUpdateFepInlineTextL(_L("x"), _L("tex"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);
	CallDoCommitFepInlineEditL(3);

	keyEvent.iScanCode = '8';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 3);
	iFEPInputCore->StartFepInlineEditL(KNullDesC, 0, TCursorSelection(0, 0));
	CallUpdateFepInlineTextL(KNullDesC, _L("tex"));
	CallUpdateFepInlineTextL(_L("t"), _L("text"));
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 4);
	CallDoCommitFepInlineEditL(4);

	// delete the last letter
	keyEvent.iScanCode = 1;	
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 4);
	
	keyEvent.iCode = 8;
	keyEvent.iScanCode = 1;
	keyEvent.iModifiers = 1;	
	CallOfferKeyEventL(keyEvent, EEventKey, 3);	

	keyEvent.iCode = 0;
	keyEvent.iScanCode = 1;
	keyEvent.iModifiers = 0;	
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);
}

void CTestFEP::testNumeric1()
{
	// Simulate the CFEPInputCore calls for some S60 numeric entry (the text "12345")
	// Calls and parameters are taken from the emulator console output
	iPredictiveText = EFalse;
	iNumericText = ETrue;
	iText->TextPtr()->Des().Zero();
	
	TKeyEvent keyEvent;
	keyEvent.iCode = 0;
	keyEvent.iScanCode = '1';
	keyEvent.iRepeats = 0;
	keyEvent.iModifiers = 0;

	CallOfferKeyEventL(keyEvent, EEventKeyDown, 0);
	
	keyEvent.iCode = '1';
	keyEvent.iModifiers = 1;
	CallOfferKeyEventL(keyEvent, EEventKey, 1);
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
	
	keyEvent.iCode = 0;
	keyEvent.iModifiers = 0;
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 1);
	TS_ASSERT(iText->TextPtr()->Compare(_L("1")) == 0);
	
	
	keyEvent.iScanCode = '2';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 1);

	keyEvent.iCode = '2';
	keyEvent.iModifiers = 1;
	CallOfferKeyEventL(keyEvent, EEventKey, 2);
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
	
	keyEvent.iCode = 0;
	keyEvent.iModifiers = 0;
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 2);
	TS_ASSERT(iText->TextPtr()->Compare(_L("12")) == 0);

	keyEvent.iScanCode = '3';
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 2);

	keyEvent.iCode = '3';
	keyEvent.iModifiers = 1;
	CallOfferKeyEventL(keyEvent, EEventKey, 3);
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
	
	keyEvent.iCode = 0;
	keyEvent.iModifiers = 0;
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);
	TS_ASSERT(iText->TextPtr()->Compare(_L("123")) == 0);
	
	// exit numeric mode (long press of hash key)
	keyEvent.iScanCode = 127;
	CallOfferKeyEventL(keyEvent, EEventKeyDown, 3);
	
	keyEvent.iCode = 35;
	keyEvent.iModifiers = 1;
	CallOfferKeyEventL(keyEvent, EEventKey, 4);
	TS_ASSERT(iFEPInputCore->GetNextState() == MEDIATOR_UPDATE);
	
	iFEPInputCore->StartFepInlineEditL(_L("#"), 1, TCursorSelection(3, 4));
	iFEPInputCore->UpdateFepInlineTextL(_L("#"), 1);
	CallUpdateFepInlineTextL(_L(""), _L("123"));
	CallDoCommitFepInlineEditL(3);
	
	keyEvent.iCode = 0;
	keyEvent.iModifiers = 0;
	CallOfferKeyEventL(keyEvent, EEventKeyUp, 3);
	TS_ASSERT(iText->TextPtr()->Compare(_L("123")) == 0);
}
