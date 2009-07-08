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


#ifndef __FEPCONTROL_H__
#define __FEPCONTROL_H__

#include <coecntrl.h>
#include <fepbase.h>
#include "ThreadUtils.h"
#include <OSVersion.h>
#include "KeyMapperFactory.h"
#include "EventQueue.h"
#include <frmtlay.h> 

extern "C"
{
#include <pcsl_string.h>
#include <nim.h>
}
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <aknedsts.h>
#include <aknenv.h>
#include <aknindicatorcontainer.h>
#include <avkon.hrh>

enum TAknEditingState
{
	EStateNone,
	ET9Upper,
	ET9Lower,
	ET9Shifted,
	ENumeric,
	EMultitapUpper,
	EMultitapLower,
	EMultitapShifted,
};

class MAknEditingStateIndicator
{
public:
	virtual void SetState(TAknEditingState aState) = 0;
	virtual CAknIndicatorContainer *IndicatorContainer() = 0;
};

#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <hal.h>
#endif

class CIndexedText : public CBase
{
public:
	static TInt Order(const CIndexedText& aIndexedText1, const CIndexedText& aIndexedText2);
	CIndexedText(TInt aId, HBufC* aText = NULL);	// can function as stack-based T class
													// allows quick definition of search parameter for RArray
	virtual	~CIndexedText();
	HBufC* TextPtr();
	void SetTextPtr(HBufC* aText);
	void ReAllocL(TInt aSize);
	TInt Id();
	TInt EditPoint(){return iEditPoint;};
	void SetEditPoint(TInt aEditPoint){iEditPoint = aEditPoint;};
	TInt ParentId(){return iParentId;};
	void SetParentId(TInt aParentId){iParentId = aParentId;};
	TBool ModifiedByUser(){return iModifiedByUser;};
	void SetModifiedByUser(TBool aModifiedByUser){iModifiedByUser = aModifiedByUser;};
private:
	TInt iId;
	HBufC* iText;
	TInt iEditPoint;
	TInt iParentId;
	TBool iModifiedByUser;
};

class MFEPInputCoreObserver
{
public:
	virtual TBool PredictiveText() const = 0;
	virtual TBool NumericText() const = 0;
	virtual TBool FieldAllowsChar(TChar aChar) const = 0;
	virtual void AddEvent(TEventInfo& aInfo) = 0;
	virtual TInt MachineUidValue() const = 0;
	virtual TUint Capabilities() const = 0;
};

// internal implementation class for CFEPInputControl that can be used in a non-CONE test environment
class CFEPInputCore : public CBase
{
public:
	static void SignalMutex(TAny* aPtr);
	static CFEPInputCore* NewL(CIndexedText* aText, MFEPInputCoreObserver* aObserver, MKeyMapper* aKeyMapper);
	static void DeUnicodeBuffer(TPtr aTextBuffer);
	virtual ~CFEPInputCore();
	TInt DocumentLengthForFep() const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void SendTextUpdateEvent();
	TInt GetNextState();
	void SetCursorPosition(TInt aPosition);
	TInt CursorPosition();
	TCursorSelection GetEditData();
	
	void StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, const TCursorSelection& aCursorSelection);
	void UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText);
	void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
	void CancelFepInlineEdit();
	void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
	void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
	void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const;
	void DoCommitFepInlineEditL();

protected:
	CFEPInputCore(CIndexedText* aText, MFEPInputCoreObserver* aObserver, MKeyMapper* aKeyMapper);
	void ConstructL();
	void InsertTextL(const TDesC& aNewText);

private:
	mutable RMutex iMutex;		// controls access to data members
	CIndexedText* iText;
	TBool iInSession;
	TCursorSelection iCursorForFEP;
	TBool iTextBufferUpdated;
	MKeyMapper* iKeyMapper;
	MFEPInputCoreObserver* iObserver;
	TBool iDeleteSeen;
	TInt iLastCursorPosition;
	TUint iLastCharCode;
	TInt iCurrentChar;
};

class CFEPInputControl : public CCoeControl, public MCoeFepAwareTextEditor, public MCoeFepAwareTextEditor_Extension1, public MFEPInputCoreObserver
{
public:
	static CFEPInputControl* NewL(CIndexedText* aText, MEventQueue* aQueue, MKeyMapper* aKeyMapper);
	virtual ~CFEPInputControl();
	void SetCapabilities(TInt aCapabilities);
	void CancelFep();
	void CancelFEPTransaction();
	TInt GetNextState();
	void SetCursorPosition(TInt aPosition);
	TInt CursorPosition();
	TCursorSelection GetEditData();
	TBool GetUnderline();
	
	// from CCoeControl
	virtual TCoeInputCapabilities InputCapabilities() const;
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	// from MFEPInputCoreObserver
	virtual TBool PredictiveText() const;
	virtual TBool NumericText() const;
	virtual TBool FieldAllowsChar(TChar aChar) const;
	virtual void AddEvent(TEventInfo& aInfo);
	virtual TInt MachineUidValue() const;
	virtual TUint Capabilities() const;

protected:
	CFEPInputControl(MEventQueue* iQueue);
	void ConstructL(CIndexedText* aText, MKeyMapper* aKeyMapper);
	
	virtual void StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
	virtual void UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText);
	virtual void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
	virtual void CancelFepInlineEdit();
	virtual TInt DocumentLengthForFep() const;
	virtual TInt DocumentMaximumLengthForFep() const;
	virtual void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
	virtual void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
	virtual void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const;
	virtual void GetFormatForFep(TCharFormat& aFormat, TInt aDocumentPosition) const;
	virtual void GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine, TInt& aHeight, TInt& aAscent, TInt aDocumentPosition) const;
	virtual void DoCommitFepInlineEditL();
	virtual MCoeFepAwareTextEditor_Extension1* Extension1(TBool& aSetToTrue);
	void SetStateTransferingOwnershipL(CState* aState, TUid aTypeSafetyUid);
	MCoeFepAwareTextEditor_Extension1::CState* State(TUid aTypeSafetyUid); // this function does *not* transfer ownership

	virtual void StartFepInlineEditL(TBool& aSetToTrue, const TCursorSelection& aCursorSelection, const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
	virtual void SetCursorType(TBool& aSetToTrue, const TTextCursor& aTextCursor);
	virtual MCoeFepLayDocExtension* GetFepLayDocExtension(TBool& aSetToTrue);

	static void UpdateUnderlineCallback(TAny * aThis);

private:
	CCoeFep* iCurrentFEP;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	CAknEdwinState* iState;
	CAknIndicatorContainer* iIc;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	MCoeFepAwareTextEditor_Extension1::CState* iState;
#endif
	MFepInlineTextFormatRetriever* iInlineTextFormatRetriever;
	TUint iInputCapabilities;
	TInt iCapabilities;
	TBool iFepActive;
	CFEPInputCore* iFEPInputCore;
	MEventQueue* iQueue;
	TInt iMachineUidValue;
	CThreadRunner* iThreadRunner;
	TBool iUnderline;
};

#endif /* __FEPCONTROL_H__ */
