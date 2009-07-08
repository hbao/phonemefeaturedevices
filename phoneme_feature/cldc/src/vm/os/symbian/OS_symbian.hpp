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


#ifndef __OS_SYMBIAN_HPP__
#define __OS_SYMBIAN_HPP__

#include <e32base.h>
#include <ES_SOCK.H>
#include <in_sock.h>
#include <f32file.h>
#include <BADESCA.h>
#include <jvm.h>
#include <OSVersion.h>

#define KInterval 10000

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
const TInt KMinHeap = 4 * 1024 * 1024; // 5M
const TInt KMaxHeap = 8 * 1024 * 1024; // 8M
#elif (__S60_VERSION__ == __S60_V2_FP3_VERSION_NUMBER__)
const TInt KMinHeap = 11 * 256 * 1024; // 2.75M
const TInt KMaxHeap = 4 * 1024 * 1024; // 4M
#else
const TInt KMinHeap = 11 * 256 * 1024; // 2.75M
const TInt KMaxHeap = 11 * 256 * 1024; // 2.75M
#endif


class MApplication;

#ifndef IMPORT_C 
#define IMPORT_C
#endif

class CVMArguments : public CBase
{
public:
    IMPORT_C static CVMArguments* NewLC();
    virtual ~CVMArguments();
    IMPORT_C void AddL(const TDesC8& aProperty);
    inline const TPtrC8 Property(TInt aIndex){return (*iProperty)[aIndex];}
    IMPORT_C const TInt Count();
protected:
    CVMArguments();
    virtual void ConstructL();
protected:
    CDesC8ArrayFlat* iProperty;
};

class CVMProperties : public CVMArguments
{
public:
    IMPORT_C static CVMProperties* NewLC();
    virtual ~CVMProperties();
    IMPORT_C void AddL(const TDesC8& aKey,const TDesC8& aProperty);
    inline const TPtrC8 Key(TInt aIndex){return (*iKey)[aIndex];}
    IMPORT_C const TInt Count();
private:
    CVMProperties();
    void ConstructL();
private:
    CDesC8ArrayFlat* iKey;
};



IMPORT_C TInt RunVMCode(const JvmPathChar * aClassPath,char* aMainClass,CVMProperties& aProperties,CVMArguments& aArguments,MApplication* aApp);

typedef enum
{
	KMIDPKeyDownEvent 	= 0xd0,
	KMIDPKeyUpEvent,
	KMIDPKeyRepeatEvent,
	KMIDPPenPressedEvent,
	KMIDPPenReleaseEvent,
	KMIDPPenDragEvent,
	KMIDPForegroundEvent,
	KMIDPCommandEvent,
	KMIDPScreenChangeEvent,
	KMIDPPushRegisterEvent,
    KMIDPPlatformRequestInstallEvent
} TSymbianMIDPEvent;

typedef enum
{
	KSymbianEntryModeUnknown,
	KSymbianEntryModeT9,
	KSymbianEntryModeMultiTap,
	KSymbianEntryModeNumeric
} TSymbianEntryMode;

class TMidpCommand
{
public:
  TInt priority;
  HBufC* shortLabel_str;
  HBufC* longLabel_str;
  TUint8 type;
  TUint id;
};


class TEventInfo
{
public:
	TEventInfo() :iEvent(0),iHandle(0),iIntParam1(0),iIntParam2(0),iStringParam1(NULL){}
	TEventInfo(TEventInfo& aCopy){iEvent = aCopy.iEvent;iHandle=aCopy.iHandle;iIntParam1=aCopy.iIntParam1;iIntParam2=aCopy.iIntParam2;iStringParam1=aCopy.iStringParam1;}
	TInt iEvent;
	TInt iHandle;
	TInt iIntParam1;
	TInt iIntParam2;
	HBufC* iStringParam1;
};

class MSocket
{
public:
	virtual TInt Connect(TInetAddr aAddr) = 0;
	virtual void Close() = 0;
	virtual TInt Available() = 0;
	virtual TInt Read(TPtr8 aData,TInt* aBytesRead) = 0;
	virtual TInt Send(TPtr8 aData, TInt* aTxLen) = 0;
	virtual TInt SetOption(TUint aOption,TUint aVal) = 0;
	
	/**
	 * Returns 0 if socket is closed or there is an error obtaining remote IP address.
	 */
	virtual TInt GetIPv4Address() = 0;
protected:
	virtual ~MSocket(){}
};

class MHostResolver
{
public:
	virtual TInt ResolveHost(const TDesC& aHost,TUint32& aAddr) = 0;
	virtual void Close() = 0;
protected:
	virtual ~MHostResolver(){}
};

class MApplication
{
public:
	virtual TAny* NewChunk(TUint aInitialSize,TUint aMaxSize,TUint aGrowBy,TUint aAlignment) = 0;
	virtual TInt AdjustChunk(TAny* aPtr,TInt aNewSize) = 0;
	virtual void FreeChunk(TAny* aPtr) = 0;

	// System event things
	virtual void GetNextEvent(long aTimeout,TEventInfo& aInfo) = 0;

	// Network/socket things
	virtual MHostResolver* HostResolver() = 0;
	virtual MSocket* OpenSocket() = 0;

	// Timer things
	virtual TBool StartTicker(TInt(*aFunction)(TAny *aPtr)) = 0;
	virtual TBool TickerRunning() = 0;
	virtual void StopTicker() = 0;
	virtual void SuspendTicker() = 0;
	virtual void ResumeTicker() = 0;

	// file system things
	virtual const TFileName GetFullPath(const TDesC& aFileName) = 0;

	// graphics things
	/** if either aWidth &or aHeight equal 0 then this is treated as freeing resources.
	This function frees any exisiting buffers before re-allocating
	**/
	virtual TUint32* CreateScreen(TInt aWidth,TInt aHeight) = 0;
	virtual void Refresh(TInt aTlx,TInt aTly,TInt aBrX,TInt aBry) =0;
	virtual void SetFullScreenMode(TBool aFullscreen) = 0;
	virtual TBool FullScreenMode() = 0;
	virtual TInt GetScreenWidth() = 0;
	virtual TInt GetScreenHeight() = 0;
	virtual void RequestOrdinalChange(TInt aDisplayId,TBool aForeground) = 0;
	virtual void StartVibrate(TInt aDuration) = 0;
	virtual void FontInfo(TInt face, TInt style, TInt size,TInt *ascent, TInt *descent, TInt *leading) = 0;
	virtual void DrawChars(TInt pixel, const TInt16 *clip, const TAny* dst, TInt dotted, TInt face, TInt style,TInt size,TInt x,TInt y,TInt anchor,const TText16 *charArray, TInt n) = 0;
	virtual TInt CharsWidth(TInt face, TInt style, TInt size,const TText16 *charArray, TInt n) = 0;
	virtual void SetMenus(TMidpCommand *commands, int length) = 0;
	// keyboard handling things
	virtual void BeginEditSession(TInt aId, TInt aConstraints) = 0;
	virtual void EndEditSession() = 0;
	// text field things
	virtual TInt GetNextState() = 0;
	virtual HBufC* GetTextFieldString(TInt aId)= 0;
	virtual TInt SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId) = 0;
	virtual void SetCursorPosition(TInt aPosition) = 0;
	virtual void GetEditData(TInt& aEditPos, TInt& aEditLength) =0;
	virtual TBool GetUnderline() = 0;
	virtual void ShowDisplayableNotify(TInt aDisplayableId) = 0;
	// platform things
	virtual TBool PlatformRequest(const TDesC& aUrl) = 0;
	virtual TBool RegisterConnection(const TDesC& aConnection,const TDesC& aMidlet,const TDesC& aFilter) = 0;
	virtual TBool UnregisterConnection(const TDesC& aConnection) = 0;
	virtual TInt PlayAudio(const TDesC8& aType, TPtr8& aData) = 0;
	virtual TInt InitializeDecoder(TDesC8& aSourceData, TInt& aWidth, TInt& aHeight, TBool& aHasMask) = 0;
	virtual TInt DecodeImage(char* aOutData, char* aOutMaskData) = 0;
	
	// debug things
	virtual void DebugMessage(const TDesC& aMsg) = 0;
protected:
	virtual ~MApplication(){};
};


#endif
