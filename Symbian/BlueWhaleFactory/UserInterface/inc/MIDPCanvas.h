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

#ifndef __MIDPCANVAS_H__
#define __MIDPCANVAS_H__

#include <OSVersion.h>
#include <coecntrl.h>
#include "ViewInterface.h"
#include "MIDPApp.h"
#include "FEPInputControl.h"
#include "EventQueue.h"
#include "KeyMapperFactory.h"
#include <hal.h>
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <qikcommand.h>
#endif

class CPbkContactEngine;

typedef enum
{
	ESCREEN 	= 1,
	EBACK 		= 2,
	ECANCEL 	= 3,
	EOK			= 4,
	EHELP		= 5,
	ESTOP		= 6,
	EEXIT		= 7,
	EITEM		= 8,
	ENATIVE		= 9
} TCommandType;

class CMIDPCanvas;
class CMidpScreenControl : public CCoeControl
{
public:
	static CMidpScreenControl* NewL(CMIDPCanvas* aCanvas);
	~CMidpScreenControl();
	TUint32* CreateDisplayBitmap(TInt aWidth,TInt aHeight);
	CFbsBitmap* Bitmap();
protected:
	CMidpScreenControl(CMIDPCanvas* aCanvas);
	void ConstructL();
	virtual TInt CountComponentControls() const;
	virtual CCoeControl* ComponentControl(TInt aIndex) const;
	virtual void HandleResourceChange(TInt aType);
	virtual void SizeChanged();
	virtual void Draw(const TRect& aRect) const;
	virtual void HandleCommandL(TInt aCommand);
	void DrawBitmap(const TRect& aRect,CWindowGc& aGc) const;
	static void CreateBitmapCallback(TAny* aThis);
private:
	CFbsBitmap* iBitmap;
	TUint32* iBuffer;
	TInt iLength;
	TInt iWidth;
	TInt iHeight;
	CMIDPCanvas* iCanvas;
	CThreadRunner* iThreadRunner;
	TBool iIsLocked;
};


class CBaseMIDPView;
class CMIDPCanvas : public CCoeControl, public MMIDPCanvas
{
friend class CMidpScreenControl;
public:
	CMIDPCanvas();
	virtual ~CMIDPCanvas();
	void ConstructL(const TRect& aRect,MViewAppUiCallback* aViewAppUiCallback,CBaseMIDPView* iView);

	void SetViewAppUiCallback(MViewAppUiCallback* aViewAppUiCallback) {iViewAppUiCallback = aViewAppUiCallback;}
	virtual TInt CountComponentControls() const;
	virtual CCoeControl* ComponentControl(TInt aIndex) const;
	virtual void HandleResourceChange(TInt aType);
	virtual void SizeChanged();
	virtual void Draw(const TRect& aRect) const;
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	virtual TInt GetDisplayWidth();
	virtual TInt GetDisplayHeight();
	virtual TUint32* CreateDisplayBitmap(TInt aWidth,TInt aHeight);
	virtual void Refresh(const TRect& aRect);
	virtual void SetFullScreenMode(TBool aFullscreen);
	virtual TBool FullScreenMode();
	virtual void RequestOrdinalChange(TBool aForeground);
	void SetEventQueue(MEventQueue* aQueue){iQueue = aQueue;}
	virtual CFbsBitmap* Bitmap();
	virtual void BeginEditSession(TInt aId, TInt aConstraints);
	virtual void EndEditSession();
	virtual TInt GetNextState();
	virtual HBufC* GetInlineText(TInt aId);
	virtual TInt SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId);
	virtual void SetCursorPosition(TInt aPosition);
	virtual void GetEditData(TInt& aEditPos, TInt& aEditLength);
	virtual TBool GetUnderline();
	virtual void ShowDisplayableNotify(TInt aDisplayableId);
	virtual void SetMenus(TMidpCommand *aCommands, int aLength);
	virtual RArray<TMidpCommand>& LockCommands();
	virtual void UnlockCommands();
	virtual void SetCba(CEikButtonGroupContainer* aCba);
	virtual TInt BackCommand();
	virtual TInt OkCommand();
	virtual void HandleCommandL(TInt aCommand);
	void DoFullScreen();
	virtual RWindow& Window() const {return CCoeControl::Window();};
private:
	void FreeCommands();
	void SetCbaL();
	
	static void SetMenusCallback(TAny* aThis);
	static void FreeBitmapCallback(TAny* aThis);
	static void BeginEditSessionCallback(TAny* aThis);
	static void EndEditSessionCallback(TAny* aThis);
	static void SetFullScreenModeCallback(TAny* aThis);
	void LookupEmailL();
	void InsertTextL(const TDesC& aNewText);
	TKeyResponse PlainOfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void SendTextUpdateEvent();
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	TBool CalculateCanvasRect(TBool aIncludeCba = ETrue);
#endif
private:
	CFEPInputControl* iFEPControl;
	CMidpScreenControl* iMidpControl;
	class CDrawer : public CActive
	{
	public:
		CDrawer(CMIDPCanvas* aParent) : CActive(EPriorityUserInput), iParent(aParent)
		{
			CActiveScheduler::Add(this);
		}
		virtual ~CDrawer()
		{
			iThread.Close();
		}
		void RunL()
		{
			iParent->Window().Invalidate(iRect);
		}
		void DoCancel()
		{}
		TInt RunError(TInt /*aError*/)
		{
			return KErrNone;
		}
		void StartL()
		{
			User::LeaveIfError(iThread.Open(RThread().Id()));
		}
		void Refresh(TRect aRect)
		{
			if(!IsActive())
			{
				iRect = aRect;
				iStatus = KRequestPending;
				TRequestStatus* status = & iStatus;
				SetActive();
				iThread.RequestComplete(status,KErrNone);
			}
		}
		RThread iThread;
		CMIDPCanvas* iParent;
		TRect iRect;
	};
	CDrawer* iDrawer;
	MEventQueue* iQueue;
	TBool iShifted;
	MKeyMapper* iKeyMapper;
	MViewAppUiCallback * iViewAppUiCallback;
	RPointerArray<CIndexedText> iTextArray;		// Text content of each editor field on the canvas, sorted by id
	CIndexedText* iCurrentField;
	TBool iTextBufferUpdated;
	RArray<TMidpCommand> iCommands;
	RMutex iCommandMutex;
	mutable RMutex iBufferMutex;

	CThreadRunner* iThreadRunner;
	RSemaphore iMenuSync;
	CEikButtonGroupContainer* iCba;
	TInt iBackCommand;
	TInt iOkCommand;
	TBool iAddLookup;
	CPbkContactEngine* iPbkContactEngine;
	TInt iConstraints;
	TBool iTextEdit;
	CBaseMIDPView* iView;
	TBool iFullScreen;
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	enum TDrawingState
	{
		KDrawingNormally,
		KWaitingForDisplayableChange,
		KWaitingForRepositioning
	};
	TDrawingState iDrawingState;
	TRect iCanvasRect;
#endif
	TInt iPreviousCommandsCount;
	TBuf<5> iKeyPressFIFO;
	RArray<TKeyEvent> iPressedKeys;
	TInt iMachineUidValue;
	TInt iCursorPosition;
	TInt iCommandsLength;
	};



#endif /*__MIDPCANVAS_H__*/
