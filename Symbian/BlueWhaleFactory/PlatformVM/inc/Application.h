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


#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <W32STD.H>
#include <os_symbian.hpp>
#include <RefCountedBase.h>
#include <flogger.h>
#include <OSVersion.h>
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
#include <HWRMVibra.h>
#include <avkon.hrh>
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <vibractrl.h> 
#include <avkon.hrh>
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <vibration.h>
#include <coefont.h>
#endif
#include "logger.h"
#include "Thread.h"
#include "DirectoryList.h"
#include "Sockets.h"
#include "MIDPApp.h"
#include "EventQueue.h"
#include "ThreadUtils.h"
#include "AudioPlayer.h"
#include <imageconversion.h>
#include "PhoneCall.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <gxj_putpixel.h>
#ifdef __cplusplus
}
#endif

static const TUid KUidBlueWhalePlatformApp = {0x2000E27A};

const TUid KUidStarterExe = {0x2000E27B};
_LIT(KAutostartConnection,"autostart://:");
const TUid KUidAutoStart = {0xA0003F50};
const TUid KUidWatcherMonkey = {0xA0003F52};

enum TMonkeyBusiness
{
	EStartMonkey,
	ESpankMonkey,
	EStopMonkey,
	EAnnoyMonkey
};

const TUid KUidSisReaderExe = {0x2000E27F};
const TUid KUidSisFileName = {0xA0003F51};
_LIT(KQuitReasonFile, "quitreason.txt");

class MLogger;
class CTimeOut;

class THeapMapper
{
	public:
		THeapMapper(RHeap* aHeap,TAny* aAlloc,TAny* aUnalignedAlloc): iHeap(aHeap),iAlloc(aAlloc),iUnalignedAlloc(aUnalignedAlloc){}
		THeapMapper(THeapMapper& aCopy) : iHeap(aCopy.iHeap),iAlloc(aCopy.iAlloc),iUnalignedAlloc(aCopy.iUnalignedAlloc){}
		THeapMapper(){}
		RHeap* iHeap;
		TAny* iAlloc;			// aligned heap pointer
		TAny* iUnalignedAlloc;	// actual heap pointer
};


class CTicker : public CBase
{
	public: 
		CTicker(MApplication* aParent,RThread& aThread);
		virtual ~CTicker();
		void ConstructL();
		TBool Start(TInt(*aFunction)(TAny *aPtr));
		TBool Running();
		void Stop();
		void Suspend();
		void Resume();
		// debug fuction
		void Debug();
	private:
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();
		static TInt TickerMain(TAny* aSelf);
		static void StartTickerCallback(TAny * aThis);
		static void StopTickerCallback(TAny * aThis);
		static void SuspendTickerCallback(TAny * aThis);
		static void ResumeTickerCallback(TAny * aThis);
		
	private:
		CPeriodic* iTicker;
		TInt(*iFunction)(TAny *aPtr);
		MApplication* iParent;
		TBool iRunning;
		CThreadRunner* iThreadRunner;
};


const TInt KCID_MApplication = 0xA0003F53;
const TInt KCID_MMIDPApplication = 0xA0003F54;
const TInt KIID_MApplication = 0xA0003F55;
const TInt KIID_MDebugApplication = 0xA0003F56;

class MDebugApplication : public MApplication, public MEventQueue
{
public:
	virtual void DebugResources() = 0;
	virtual ~MDebugApplication(){}
};

class MPropertyPersist
{
public:
	virtual void SavePropertiesL() = 0;
	virtual ~MPropertyPersist(){}

};

class CVMManager : public CBase, public MDebugApplication, public MRunnable, public MPropertyPersist, public MThreadSupervisor
{
	public:
		static MApplication * NewL();
		IMPORT_C static const TFileName FullPath(const TDesC& aFileName);
		
		void ConstructL();
		void StartL(RThread& aThread);
		void StopL();
		void SetThread(MThread* aThread) {iThread = aThread;}
		
		// MApplication
		virtual TAny* NewChunk(TUint aInitialSize,TUint aMaxSize,TUint aGrowBy,TUint aAlignment);
		virtual TInt AdjustChunk(TAny* aPtr,TInt aNewSize);
		virtual void FreeChunk(TAny* aPtr);

		// System event things
		virtual void GetNextEvent(long aTimeout,TEventInfo& aInfo);

		// Network/socket things
		virtual MHostResolver* HostResolver();
		virtual MSocket* OpenSocket();
	
		// Timer things
		virtual TBool StartTicker(TInt(*aFunction)(TAny *aPtr));
		virtual TBool TickerRunning();
		virtual void StopTicker();
		virtual void SuspendTicker();
		virtual void ResumeTicker();
	
		// file system things
		virtual const TFileName GetFullPath(const TDesC& aFileName);
		
		// MEventQueue
		virtual void AddEvent(TEventInfo aEvent);
		
		// graphics things
		virtual TUint32* CreateScreen(TInt aWidth,TInt aHeight);
		virtual void Refresh(TInt aTlx,TInt aTly,TInt aBrX,TInt aBry);
		virtual void SetFullScreenMode(TBool /*aFullscreen*/){};
		virtual TBool FullScreenMode() {return EFalse;}
		virtual TInt GetScreenWidth() {return 0;}
		virtual TInt GetScreenHeight(){return 0;}
		virtual void RequestOrdinalChange(TInt /*aDisplayId*/,TBool /*aForeground*/) {}
		virtual void StartVibrate(TInt /*aDuration*/){}
		virtual void FontInfo(TInt, TInt, TInt, TInt*, TInt*, TInt*) {}
		virtual void DrawChars(TInt, const TInt16*, const TAny*, TInt, TInt, TInt, TInt, TInt, TInt, TInt, const TText16*, TInt){}
		virtual TInt CharsWidth(TInt, TInt, TInt, const TText16*, TInt) {return 0;}	
		virtual void SetMenus(TMidpCommand*, int) {};
		// keyboard handling things
		virtual void BeginEditSession(TInt, TInt) {}
		virtual void EndEditSession(){}
		// text field things
		virtual TInt GetNextState() {return 0;}
		virtual HBufC* GetTextFieldString(TInt){ return NULL;}
		virtual TInt SetTextFieldString(TInt, HBufC*, TInt){ return KErrNone;}
		virtual void SetCursorPosition(TInt) {}
		virtual void GetEditData(TInt&, TInt&) {}
		virtual TBool GetUnderline() {return EFalse;}
		virtual void ShowDisplayableNotify(TInt) {}
		// platform things
		virtual TBool PlatformRequest(const TDesC& /*aUrl*/) { return EFalse;}
		virtual TBool RegisterConnection(const TDesC& /*aConnection*/,const TDesC& /*aMidlet*/,const TDesC& /*aFilter*/) { return EFalse;}
		virtual TBool UnregisterConnection(const TDesC& /*aConnection*/) { return EFalse;}
		virtual TInt PlayAudio(const TDesC8&, TPtr8&){ return KErrNone;}
		virtual TInt InitializeDecoder(TDesC8& /*aSourceData*/, TInt& /*aWidth*/, TInt& /*aHeight*/, TBool& /*aHasMask*/){return KErrNone;}
		virtual TInt DecodeImage(char* /*aOutData*/, char* /*aOutMaskData*/){return KErrNone;}
		// debug things
		virtual void DebugMessage(const TDesC& aMsg);
		void DebugResources();
	private:
		void TearDownMemory();
		void LoadPropertiesL();
		void SavePropertiesL();
		virtual void ThreadTerminated(TThreadId aId,TInt aReason);
	protected:
		virtual ~CVMManager();
		CVMManager();
		static void TimeoutCallback(TAny * aThis);
		RArray<THeapMapper> iHeapMapper;
		CTicker* iTicker;
		CDirectoryLister* iLister;
		CSocketFactory* iSocketFactory;
		MProperties* iProperties;
		MLogger * iLogger; 
		MThread* iThread;
		CThreadRunner* iThreadRunner;
		RMsgQueue<TEventInfo> iEventQueue;
		TInt iTimeout;
		
	class CEventTimer : public CTimer
		{
		public:
			CEventTimer(CVMManager* aParent) : CTimer(CActive::EPriorityStandard),iParent(aParent)
			{
				CActiveScheduler::Add(this);
			}
			~CEventTimer()
			{
				if(IsActive())
				{
					Cancel();
				}
			}
			void ConstructL()
			{
				CTimer::ConstructL();
			}
			void Start(TInt aTimeout)
			{
				Cancel();
				After(1000 * aTimeout);
			}
		protected:
			void RunL()
			{
				TEventInfo event;
				event.iEvent = -1;
				event.iHandle = 0;
				iParent->AddEvent(event);
			}
		private:
			CVMManager* iParent;
		};
		CEventTimer* iTimer;

};

class CTextWidthCacheEntry : public CBase
{
public:
	static CTextWidthCacheEntry* NewL(const TDesC& aText, TInt aFace, TInt aStyle, TInt aSize);
	static TBool IdentityRelation(const CTextWidthCacheEntry& aObject1, const CTextWidthCacheEntry& aObject2);
	virtual ~CTextWidthCacheEntry();
	TInt Width() {return iWidth;};
	void SetWidth(TInt aWidth) {iWidth = aWidth;};
protected:
	CTextWidthCacheEntry(TInt aFace, TInt aStyle, TInt aSize);
	void ConstructL(const TDesC& aText);
private:
	HBufC* iText;
	TInt iFace;
	TInt iStyle;
	TInt iSize;
	TInt iWidth;
};

class TCachedFont
{
public:
	TCachedFont(CFont* aFont, TFontSpec aFontSpec);
	static TBool IdentityRelation(const TCachedFont& aObject1, const TCachedFont& aObject2);
	CFont* Font() {return iFont;};
private:
	CFont* iFont;
	TFontSpec iFontSpec;
};

class CMIDPFontManager : public CBase
{
public:
	CMIDPFontManager();
	virtual ~CMIDPFontManager();
	void ConstructL();
	void SetCanvas(MMIDPCanvas* aCanvas);
	void FontInfo(TInt face, TInt style, TInt size,TInt *ascent, TInt *descent, TInt *leading);
	void DrawChars(TInt pixel, const TInt16 *clip, const gxj_screen_buffer *dst, TInt dotted, TInt face, TInt style,TInt size,TInt x,TInt y,TInt anchor,const TText16 *charArray, TInt n);
	TInt CharsWidth(TInt face, TInt style, TInt size,const TText16 *charArray, TInt n);
	
private:
	static void FontInfoCallback(TAny* aThis);
	static void DrawCharsCallback(TAny* aThis);
	static void CharsWidthCallback(TAny* aThis);
	void EnsureBitmap();
	TBool SetupFont(TInt aFace,TInt aStyle,TInt aSize);
	TInt TextWidthInPixels(const TDesC &aText);
	void UpdateFont();
private:
	CThreadRunner* iThreadRunner;
	MMIDPCanvas* iCanvas;
	CFont* iCurrentFont;
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iDevice;
	CFbsBitGc* iGc;
	TFontSpec iCurrentFontSpec;
	TFontSpec iNormalFontSpec;
	TInt iFace;
	TInt iStyle;
	TInt iSize;
	TInt iPoints;
	TInt iAscent;
	TInt iDescent;
	TInt iHeight;
	TInt iLeading;
	TInt iPixel;
	const TInt16 *iClip;
	const gxj_screen_buffer *iDest;
	TInt iDotted;
	TInt iX;
	TInt iY;
	TInt iAnchor;
	const TText16 *iCharArray;
	TInt iN;
	TInt iCharsWidth;
	enum
	{
			HCENTER 	= 1,
			VCENTER 	= 2,
			LEFT 		= 4,
			RIGHT 		= 8,
			TOP 		= 16,
			BOTTOM 		= 32,
			BASELINE 	= 64
	} TAnchor;
	enum
	{
		STYLE_PLAIN = 0,
		STYLE_BOLD = 1,
		STYLE_ITALIC = 2,
		STYLE_UNDERLINED = 4
	} TFontStyle;
	enum
	{
		SIZE_SMALL = 8,
		SIZE_MEDIUM = 0,
		SIZE_LARGE = 16
	} TFontSize;
	TBool iCurrentFontSpecHasChanged;
	RPointerArray<CTextWidthCacheEntry> iTextWidthCache;
	CFontCache* iFontCache;
};

class CImageConverter : public CActive
{
public:
	static CImageConverter* NewL();
	virtual ~CImageConverter();
	
	void Initialize(TDesC8* aSourceData);
	void Start(char* aOutData, char* aOutMaskData);
	TFrameInfo FrameInfo();
	RSemaphore& ConvertSemaphore();
	
protected:
	CImageConverter();
	void ConstructL();
	void StartL();
	
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;
	CImageDecoder* iImageDecoder;
	RSemaphore iConvertSemaphore;
	TFrameInfo iFrameInfo;

	// not owned
	char* iOutData;
	char* iOutMaskData;
};

class CMIDPApp : public CVMManager
{
public:
	static CMIDPApp* NewL();
	static TInt VibraTimerFunction(TAny* aSelf);
	void StartL(RThread& aThread);	
	void SetCanvas(MMIDPCanvas* aCanvas);
public:
	// graphics things
	virtual TUint32* CreateScreen(TInt aWidth,TInt aHeight);
	virtual void Refresh(TInt aTlx,TInt aTly,TInt aBrX,TInt aBry);
	virtual void SetFullScreenMode(TBool aFullscreen);
	virtual TBool FullScreenMode();
	virtual TInt GetScreenWidth();
	virtual TInt GetScreenHeight();
	virtual void RequestOrdinalChange(TInt aDisplayId,TBool aForeground);
	virtual void StartVibrate(TInt aDuration);
	virtual void FontInfo(TInt face, TInt style, TInt size,TInt *ascent, TInt *descent, TInt *leading);
	virtual void DrawChars(TInt pixel, const TInt16 *clip, const TAny* dst, TInt dotted, TInt face, TInt style,TInt size,TInt x,TInt y,TInt anchor,const TText16 *charArray, TInt n);
	virtual TInt CharsWidth(TInt face, TInt style, TInt size,const TText16 *charArray, TInt n);
	virtual void SetMenus(TMidpCommand *commands, int length);
	// keyboard handling things
	virtual void BeginEditSession(TInt aId, TInt aConstraints);
	virtual void EndEditSession();
	// text field things
	virtual TInt GetNextState();
	virtual HBufC* GetTextFieldString(TInt aId);
	virtual TInt SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId);
	virtual void SetCursorPosition(TInt aPosition);
	virtual void GetEditData(TInt& aEditPos, TInt& aEditLength);
	virtual TBool GetUnderline();
	virtual void ShowDisplayableNotify(TInt aDisplayableId);
	// platform things
	virtual TBool PlatformRequest(const TDesC& aUrl);
	virtual TBool RegisterConnection(const TDesC& aConnection,const TDesC& aMidlet,const TDesC& aFilter);
	virtual TBool UnregisterConnection(const TDesC& aConnection);
	virtual TInt PlayAudio(const TDesC8& aType, TPtr8& aData);
	virtual TInt InitializeDecoder(TDesC8& aSourceData, TInt& aWidth, TInt& aHeight, TBool& aHasMask);
	virtual TInt DecodeImage(char* aOutData, char* aOutMaskData);

protected:
	CMIDPApp();
	virtual ~CMIDPApp();
private:
	void ConstructL();
	static void LaunchBrowserCallback(TAny* aThis);
	static void DialNumberCallback(TAny* aThis);
	static void LaunchFileCallback(TAny* aThis);
	static void StartVibrateCallback(TAny* aThis);
	static void PlayAudioCallback(TAny* aThis);
	static void InitializeDecoderCallback(TAny* aThis);
	static void DecodeImageCallback(TAny* aThis);

private:
	MMIDPCanvas* iCanvas;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	CHWRMVibra* iVibraControl;
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__	
	CVibraControl * iVibraControl;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	RLibrary iVibraDll;
	SonyEricsson::CVibration* iVibraControl;
#endif
	CMIDPFontManager* iFontManager;
	HBufC* iLaunchBrowserUrl;
	HBufC* iPhoneNumber;
	TInt iDuration;
	CPeriodic* iVibraTimer;
	CAudioPlayer* iAudioPlayer;
	HBufC8* iAudioType;
	TPtr8 iAudioData;

	CImageConverter* iImageConverter;
	TDesC8* iSourceData;
	char* iOutData;
	char* iOutMaskData;
	CPhoneCall* iPhoneCall;
};


#endif // __APPLICATION_H__
