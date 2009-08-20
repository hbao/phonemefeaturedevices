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


#include <e32base.h>
#include <gdi.h>
#include <OSVersion.h>
#if (__S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__)
#include <aknutils.h>
#elif (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#include <coemain.h>
#include <QWebExternalInterface.h>
#include <eikenv.h>
#include <eikappui.h>
#endif
#include <GULUTIL.H>
#include "Application.h"
#include "DirectoryList.h"
#include "Sockets.h"
#include "ecomplus.h"
#include "Properties.h"
#include "logger.h"
#include "Thread.h"
#include "SocketEngine.h"
#include "DebugUtilities.h"
#include "DataAccess.h"
#include "MessagingDataAccessConnection.h"
#include <apgcli.h>
#include <apgtask.h>
#include <bautils.h>
#include <e32property.h>

//#define __DEBUG_APP_INTERFACE__
//#define __LOG_JAVA_MESSAGES__
//#define __DEBUGEVENT__

//#define __TESTING_ONLY_DEVBOX_DOWNLOAD__

#ifdef __DEBUG_APP_INTERFACE__
#define	DEBUGMSG(_XX) RDebug::Print(_XX)
#else
#define	DEBUGMSG(_XX)
#endif

#ifdef __DEBUGEVENT__
#define DEBUGEVENTMSG(_XX) {RDebug::Print(_XX);}
#define DEBUGEVENTMSG1(_XX,_YY) {RDebug::Print(_XX,_YY);}
#define DEBUGEVENTMSG2(_XX,_YY,_ZZ) {RDebug::Print(_XX,_YY,_ZZ);}
#define DEBUGEVENTMSG3(_XX,_YY,_ZZ,_AA) {RDebug::Print(_XX,_YY,_ZZ,_AA);}
#else
#define DEBUGEVENTMSG(_XX)
#define DEBUGEVENTMSG1(_XX,_YY)
#define DEBUGEVENTMSG2(_XX,_YY,_ZZ)
#define DEBUGEVENTMSG3(_XX,_YY,_ZZ,_AA)
#endif

const TInt KUIQStopVibrationDelay = 1000000; // 1s

#define KSmallFontHeightInTwipsValue 140
#define KMediumFontHeightInTwipsValue 158
#define KLargeFontHeightInTwipsValue 176

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
const TInt KSmallFontHeightInTwips = KSmallFontHeightInTwipsValue;
const TInt KMediumFontHeightInTwips = KMediumFontHeightInTwipsValue;
const TInt KLargeFontHeightInTwips = KLargeFontHeightInTwipsValue;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
// UIQ thinks a twip is 100th of a mm (rather than 1/1440th of an inch)
// So, twipsUIQ = twips * (25.4 / 1440) * 100 
const TInt KSmallFontHeightInTwips = (KSmallFontHeightInTwipsValue * 2540) / 1440;
const TInt KMediumFontHeightInTwips = (KMediumFontHeightInTwipsValue * 2540) / 1440;
const TInt KLargeFontHeightInTwips = (KLargeFontHeightInTwipsValue * 2540) / 1440;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
MApplication* CVMManager::NewL()
{
	CVMManager * self = new (ELeave) CVMManager();
	return self;
}

EXPORT_C const TFileName CVMManager::FullPath(const TDesC& aFileName)
{
	TFileName result;
	TBuf<2> drive;

#ifdef __WINSCW__
	drive = _L("c:"); 
#else
	Dll::FileName(result); // Get the drive letter
	TParsePtrC parse(result);
	drive = parse.Drive();
#endif
	_LIT(KLitPrivatePath, "\\private\\%08x\\");
	TBuf<18> pathToUid;
	pathToUid.Format(KLitPrivatePath, KUidBlueWhalePlatformApp);
	
	result = drive;
	result.Append(pathToUid);
	result.Append(aFileName);
	ASSERT(BaflUtils::Parse(result) == KErrNone);
	
	return result;
}

CVMManager::CVMManager()
{}

CVMManager::~CVMManager()
{
	DEBUGMSG(_L("->CVMManager::~CVMManager"));
	delete iLister;
	delete iTimer;
	iEventQueue.Close();
	DEBUGMSG(_L("<-CVMManager::~CVMManager"));
}

void CVMManager::ConstructL()
{
}

void CVMManager::ThreadTerminated(TThreadId /*aId*/,TInt /*aReason*/)
{
}

void CVMManager::StartL(RThread& aThread)
{
	iTicker = new (ELeave) CTicker(this,aThread);
	iTicker->ConstructL();
	
	iLister = new (ELeave) CDirectoryLister(aThread);
	iLister->ConstructL();
	
	LoadPropertiesL();
	
	iSocketFactory = new (ELeave) CSocketFactory(this,iProperties,this);
	iSocketFactory->ConstructL();
	iSocketFactory->StartL(aThread);
	User::LeaveIfError(iEventQueue.CreateLocal(32));
}

TUint32* CVMManager::CreateScreen(TInt /*aWidth*/, TInt /*aHeight*/)
{
	return NULL;
}

void CVMManager::Refresh(TInt /*aTlx*/, TInt /*aTly*/, TInt /*aBrX*/, TInt /*aBry*/)
{
}

void CVMManager::StopL()
{
	DebugResources();
	SavePropertiesL();
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	TearDownMemory();
	delete iTicker;
	iTicker = NULL;
	delete iLister;
	iLister = NULL;
	//delete iSocketUndertaker;
	//iSocketThread->Kill(0);
	//iSocketThread->Release();
	iSocketFactory->StopL();
	delete iSocketFactory;
	iSocketFactory = NULL;
}

_LIT( KPlatformFolder, "platform");

void CVMManager::LoadPropertiesL()
{
	MDataAccessConnection * dataAccessConnection = CiL( KCID_MMessagingDataAccessConnection, MDataAccessConnection);
	CleanupReleasePushL(*dataAccessConnection);
	MDataAccessStatement* statement = dataAccessConnection->CreateStatementL();
	CleanupReleasePushL(*statement);

	statement->SetType(KStatementTypeRootFolderInfo);
	statement->SetStringL(KPropertyStringFolder, KPlatformFolder);
	iProperties = ExL( statement, MProperties );

	CleanupStack::PopAndDestroy(statement);
	CleanupStack::PopAndDestroy(dataAccessConnection);
}

void CVMManager::SavePropertiesL()
{
	MDataAccessConnection * dataAccessConnection = CiL( KCID_MMessagingDataAccessConnection, MDataAccessConnection);
	CleanupReleasePushL(*dataAccessConnection);
	MDataAccessStatement* statement = dataAccessConnection->CreateStatementL();
	CleanupReleasePushL(*statement);

	statement->SetStringL(KPropertyStringFolder, KPlatformFolder);

	statement->SetType(KStatementTypeCommit);
	statement->SetObjectL(KPropertyObjectItem, iProperties);
	statement->ExecuteL(0);

	CleanupStack::PopAndDestroy(statement);
	CleanupStack::PopAndDestroy(dataAccessConnection);
}

void CVMManager::TearDownMemory()
{
	DEBUGMSG(_L("->CVMManager::TearDownMemory()"));
	TInt count = iHeapMapper.Count();
	while(count>0)
	{
		RHeap* heap = (iHeapMapper[0]).iHeap;
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		heap->Reset();
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__	
		heap->FreeAll();
#endif
		heap->Close();
		iHeapMapper.Remove(0);
		count = iHeapMapper.Count();
	}
	iHeapMapper.Close();
	DEBUGMSG(_L("<-CVMManager::TearDownMemory()"));
}

void CVMManager::DebugMessage(const TDesC& /*aMsg*/)
{
#ifdef __WINSCW__
	DEBUGMSG(aMsg);
#else
//	TBuf<32> filename;
//	filename.Format(_L("vm-%d.txt"),(TInt)RThread().Id());
//	RFileLogger::Write(_L("BlueWhale"), filename, EFileLoggingModeAppend,aMsg);
#endif
}

void CVMManager::DebugResources()
{
#ifdef __WINS__
	RDebug::Print(_L("== VM application resource usage =="));
	RDebug::Print(_L("==================================="));
	RDebug::Print(_L("===========Memory allocations======"));
	for(TInt i=0;i<iHeapMapper.Count();i++)
		{
		RHeap* heap  = (iHeapMapper[i]).iHeap;
		TInt size = heap->AllocLen((iHeapMapper[i]).iUnalignedAlloc);
		TInt ptr = (TInt)(iHeapMapper[i]).iAlloc;
		RDebug::Print(_L("Heap 0x%08x Ptr 0x%08x size 0x%08x"),heap,ptr,size);
		}
	if(iSocketFactory)
	{
		RDebug::Print(_L("===========Sockets================="));
		iSocketFactory->DebugFactory();
	}
	if(iTicker)
	{
		RDebug::Print(_L("===========Ticker=================="));
		iTicker->Debug();
		
	}
	RDebug::Print(_L("==================================="));

#endif
}

TAny* CVMManager::NewChunk(TUint aInitialSize,TUint aMaxSize,TUint aGrowBy,TUint aAlignment)
{
	DEBUGMSG(_L("->CVMManager::NewChunk"));
	RHeap* heap = User::ChunkHeap(NULL,(TInt)aInitialSize,(TInt)(aMaxSize + aAlignment),aGrowBy);
	if(heap)
	{
		TAny* ptr = heap->Alloc(aInitialSize + aAlignment);
		TAny* alignedPtr = reinterpret_cast<TAny*>(((reinterpret_cast<TInt>(ptr) + aAlignment - 1) / aAlignment) * aAlignment);
		THeapMapper entry(heap, alignedPtr, ptr);
		iHeapMapper.Append(entry);
		DEBUGMSG(_L("<-CVMManager::NewChunk A"));
		return alignedPtr;
	}
	DEBUGMSG(_L("<-CVMManager::NewChunk B"));
	return NULL;
}
TBool HeapFinder(const THeapMapper &aOne,const THeapMapper &aTwo)
{
	return (aOne.iAlloc == aTwo.iAlloc);
}

TInt CVMManager::AdjustChunk(TAny* aPtr,TInt aNewSize)
{
	DEBUGMSG(_L("->CVMManager::AdjustChunk"));
	TInt ret = 0;
	THeapMapper search(0,aPtr,NULL);
	TIdentityRelation<THeapMapper> relation(HeapFinder);

	TInt found = iHeapMapper.Find(search,relation);
	if(found != KErrNotFound)
	{
		THeapMapper& foundHeap(iHeapMapper[found]);
		RHeap* heap = foundHeap.iHeap;
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		TAny* newPtr = heap->ReAlloc(foundHeap.iUnalignedAlloc,aNewSize,RAllocator::ENeverMove);
#else
		TAny* newPtr = heap->ReAlloc(foundHeap.iUnalignedAlloc,aNewSize);
#endif
		if(newPtr != aPtr)
		{
			ret = 0;
		}
		else
		{
			ret = heap->AllocLen(newPtr);
		}
	}
	DEBUGMSG(_L("<-CVMManager::AdjustChunk"));
	return ret;
}

void CVMManager::FreeChunk(TAny* aPtr)
{
	DEBUGMSG(_L("->CVMManager::FreeChunk"));
	TInt ret = 0;
	THeapMapper search(0,aPtr,NULL);
	TIdentityRelation<THeapMapper> relation(HeapFinder);

	TInt found = iHeapMapper.Find(search,relation);
	if(found != KErrNotFound)
	{
		THeapMapper foundHeap(iHeapMapper[found]);
		RHeap* heap = foundHeap.iHeap;
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		heap->Free(foundHeap.iUnalignedAlloc);
		heap->Reset();
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__	
		heap->Free(foundHeap.iUnalignedAlloc);
		heap->FreeAll();
#endif
		heap->Close();
		iHeapMapper.Remove(found);
	}
	DEBUGMSG(_L("<-CVMManager::FreeChunk"));
}

void CVMManager::AddEvent(TEventInfo aEvent)
{
	DEBUGEVENTMSG1(_L("->::AddEvent 0x%02x"),aEvent.iEvent);
	iEventQueue.Send(aEvent);
	DEBUGEVENTMSG(_L("<-::AddEvent"));
}

void CVMManager::TimeoutCallback(TAny * aThis)
{
	CVMManager* This = static_cast<CVMManager*>(aThis);
	if(!This->iTimer)
	{
		This->iTimer = new CEventTimer(This);
		if(This->iTimer)
		{
			TRAPD(err,This->iTimer->ConstructL());
		}
	}
	if(This->iTimer)
	{
		This->iTimer->Start(This->iTimeout);
	}
}

void CVMManager::GetNextEvent(long aTimeout,TEventInfo& aInfo)
{
	DEBUGEVENTMSG1(_L("->::GetNextEvent %d"),aTimeout);
	if(iEventQueue.Receive(aInfo) != KErrNone)
	{
		aInfo.iEvent = -1;
		aInfo.iHandle = 0;
		if(aTimeout > 0)
		{
			iTimeout = aTimeout;
			iThreadRunner->DoCallback(TimeoutCallback,this);
			iEventQueue.ReceiveBlocking(aInfo);
		}
		else if(aTimeout < 0)
		{
			iEventQueue.ReceiveBlocking(aInfo);
		}
	}
		
	DEBUGEVENTMSG1(_L("<-::GetNextEvent 0x%02x"),aInfo.iEvent);
}

MHostResolver* CVMManager::HostResolver()
{
	DEBUGMSG(_L("->CVMManager::HostResolver"));
	return iSocketFactory->CreateResolver();
}

MSocket* CVMManager::OpenSocket()
{
	DEBUGMSG(_L("->CVMManager::OpenSocket"));
	return iSocketFactory->CreateSocket();
}

TBool CVMManager::StartTicker(TInt(*aFunction)(TAny *aPtr))
{
	DEBUGMSG(_L("->CVMManager::StartTicker"));
	TBool ret = iTicker->Start(aFunction);
	DEBUGMSG(_L("<-CVMManager::StartTicker"));
	return ret;
}

TBool CVMManager::TickerRunning()
{
	DEBUGMSG(_L("->::CVMManager::TickerRunning"));
	return iTicker->Running();
}

void CVMManager::StopTicker()
{
	DEBUGMSG(_L("->CVMManager::StopTicker"));
	iTicker->Stop();
	DEBUGMSG(_L("<-CVMManager::StopTicker"));
}

void CVMManager::SuspendTicker()
{
	DEBUGMSG(_L("->CVMManager::SuspendTicker"));
	iTicker->Suspend();
	DEBUGMSG(_L("<-CVMManager::SuspendTicker"));
}

void CVMManager::ResumeTicker()
{
	DEBUGMSG(_L("->CVMManager::ResumeTicker"));
	iTicker->Resume();
	DEBUGMSG(_L("<-CVMManager::ResumeTicker"));
}


const TFileName CVMManager::GetFullPath(const TDesC& aFileName)
{
	return CVMManager::FullPath(aFileName);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
CTicker::CTicker(MApplication* aParent,RThread& aThread) 
	: CBase(),iParent(aParent)
{}

CTicker::~CTicker()
{
	delete iTicker;
	delete iThreadRunner;
}

void CTicker::ConstructL()
{
	iThreadRunner = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL(); 
}

void CTicker::Debug()
{
#ifdef __WINS__
	RDebug::Print(_L("Ticker 0x%08x Running %d"),this,iRunning);
#endif
}

TBool CTicker::Start(TInt(*aFunction)(TAny *aPtr))
{
	iFunction = aFunction;
	iThreadRunner->DoCallbackNoBlock(StartTickerCallback,this);
	return true;
}

void CTicker::Stop()
{
	iThreadRunner->DoCallbackNoBlock(StopTickerCallback,this);
}

TBool CTicker::Running()
{
	return iRunning;
}

void CTicker::Suspend()
{
	iThreadRunner->DoCallbackNoBlock(SuspendTickerCallback,this);
}

void CTicker::Resume()
{
	iThreadRunner->DoCallbackNoBlock(ResumeTickerCallback,this);
}
void CTicker::StartTickerCallback(TAny * aThis)
{
	CTicker* This = static_cast<CTicker*>(aThis);
	TTimeIntervalMicroSeconds32 interval(KInterval);
	TCallBack callBack(This->iFunction,This->iParent);
	This->iRunning = ETrue;
	if(!This->iTicker)
	{
		This->iTicker = CPeriodic::New(CActive::EPriorityStandard);
	}
	This->iTicker->Start(0,interval,callBack);
}

void CTicker::StopTickerCallback(TAny * aThis)
{
	CTicker* This = static_cast<CTicker*>(aThis);
	This->iRunning = EFalse;
	if(This->iTicker)
	{
		This->iTicker->Cancel();
		delete This->iTicker;
		This->iTicker = NULL;
	}
}

void CTicker::SuspendTickerCallback(TAny * aThis)
{
	CTicker* This = static_cast<CTicker*>(aThis);
	This->iRunning = EFalse;
	This->iTicker->Cancel();
}

void CTicker::ResumeTickerCallback(TAny * aThis)
{
	CTicker* This = static_cast<CTicker*>(aThis);
	TCallBack callBack(This->iFunction,This->iParent);
	TTimeIntervalMicroSeconds32 interval(KInterval);
	This->iRunning = ETrue;
	if(!This->iTicker)
	{
		This->iTicker = CPeriodic::New(CActive::EPriorityStandard);
	}
	if(!This->iTicker->IsActive())
	{
		This->iTicker->Start(0,interval,callBack);
	}
}

////////////////////////////////////////////////////////////////////////
CMIDPApp* CMIDPApp::NewL()
{
	CMIDPApp* self = new (ELeave) CMIDPApp();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CMIDPApp::CMIDPApp()
: iAudioData(NULL, 0)
{
}

CMIDPApp::~CMIDPApp()
{
	TInt ignore = RProperty::Set(KUidStarterExe, KUidWatcherMonkey.iUid, EStopMonkey);

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
	delete iVibraControl;
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	if (iVibraControl)
	{
		iVibraControl->VibrationOff();
	}
	delete iVibraControl;
	iVibraDll.Close();
#endif
	delete iFontManager;
	delete iThreadRunner;
	delete iLaunchBrowserUrl;
	if (iVibraTimer)
	{
		iVibraTimer->Cancel();
	}
	delete iVibraTimer;
	delete iAudioPlayer;
	delete iAudioType;
	delete iImageConverter;
	delete iPhoneCall;
	delete iPhoneNumber;
}

void CMIDPApp::ConstructL()
{
	if(!iFontManager)
	{
		iFontManager = new (ELeave) CMIDPFontManager();
		iFontManager->ConstructL();
	}
	if (!iThreadRunner)
	{
		iThreadRunner = new (ELeave) CThreadRunner();
		iThreadRunner->ConstructL();	
	}
	if (!iVibraTimer)
	{
		iVibraTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__	
	if (!iVibraDll.Handle())
	{
		iVibraDll.Load(_L("vibration.dll"));	// ingore errors as this is only present on SE UIQ
	}
#endif
	if (!iAudioPlayer)
	{
		iAudioPlayer = CAudioPlayer::NewL();
	}
	if (!iImageConverter)
	{
		iImageConverter = CImageConverter::NewL();
	}
	if (!iPhoneCall)
	{
		TRAPD(ignore, iPhoneCall = CPhoneCall::NewL());	// don't let telephony failures stop us starting up
	}
}

void CMIDPApp::StartL(RThread& aThread)
{
	CVMManager::StartL(aThread);
}

void CMIDPApp::SetCanvas(MMIDPCanvas* aCanvas)
{
	iCanvas = aCanvas;
	iFontManager->SetCanvas(aCanvas);
}

TInt CMIDPApp::GetScreenWidth()
{
	DEBUGMSG(_L("->CMIDPApp::GetScreenWidth"));
	if(iCanvas)
	{
		DEBUGMSG(_L("<-CMIDPApp::GetScreenWidth A"));
		return iCanvas->GetDisplayWidth();
	}
	else
	{
		DEBUGMSG(_L("<-CMIDPApp::GetScreenWidth B"));
		return 0;
	}
}

TInt CMIDPApp::GetScreenHeight()
{	
	DEBUGMSG(_L("->CMIDPApp::GetScreenHeight"));
	if(iCanvas)
	{
		DEBUGMSG(_L("<-CMIDPApp::GetScreenHeight A"));
		return iCanvas->GetDisplayHeight();
	}
	else
	{
		DEBUGMSG(_L("<-CMIDPApp::GetScreenHeight B"));
		return 0;
	}
}

void CMIDPApp::Refresh(TInt aTlx,TInt aTly,TInt aBrX,TInt aBry)
{
	DEBUGMSG(_L("->CMIDPApp::Refresh"));
	if(iCanvas)
	{
		TRect rect(aTlx, aTly, aBrX, aBry);
		iCanvas->Refresh(rect);
	}
	DEBUGMSG(_L("<-CMIDPApp::Refresh"));
}

void CMIDPApp::SetFullScreenMode(TBool aFullscreen)
{
	DEBUGMSG(_L("->CMIDPApp::SetFullScreenMode"));
	if(iCanvas)
	{
		iCanvas->SetFullScreenMode(aFullscreen);
	}
	DEBUGMSG(_L("<-CMIDPApp::SetFullScreenMode"));
}

TBool CMIDPApp::FullScreenMode()
{
	DEBUGMSG(_L("->CMIDPApp::FullScreenMode"));
	if(iCanvas)
	{
		return iCanvas->FullScreenMode();
	}
	else
	{
		return EFalse;
	}
}


TUint32* CMIDPApp::CreateScreen(TInt aWidth,TInt aHeight)
{
	DEBUGMSG(_L("->CMIDPApp::CreateScreen"));
	if(iCanvas)
	{
		return iCanvas->CreateDisplayBitmap(aWidth,aHeight);
	}
	else
	{
		return NULL;
	}
}

TInt CMIDPApp::VibraTimerFunction(TAny* aSelf)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aSelf);
	This->iVibraTimer->Cancel();
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	This->iVibraControl->VibrationOff();
#endif
	return KErrNone;
}

void CMIDPApp::StartVibrateCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	DEBUGMSG(_L("->CMIDPApp::StartVibrate"));
#ifndef __WINSCW__
	if(!This->iVibraControl)
	{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TRAPD(ignore, This->iVibraControl = CHWRMVibra::NewL());
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
		TRAPD(ignore, This->iVibraControl = VibraFactory::NewL());
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
		if (This->iVibraDll.Handle())
		{
			typedef TInt(*TVibrateNewFunction)(SonyEricsson::MVibrationObserver*);

			TVibrateNewFunction newFunction = (TVibrateNewFunction)This->iVibraDll.Lookup(1);
			if (newFunction)
			{
				TRAPD(ignore, This->iVibraControl = (SonyEricsson::CVibration*)newFunction(NULL));
			}
		}
#endif
	}
#endif // __WINSCW__

	if (This->iVibraControl)
	{
		if (This->iDuration == 0)
		{
#if (__S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__)
			TRAPD(ignore,This->iVibraControl->StopVibraL());
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
			This->iVibraTimer->Cancel();
			TCallBack timerCallBack(CMIDPApp::VibraTimerFunction, This);
			This->iVibraTimer->Start(TTimeIntervalMicroSeconds32(KUIQStopVibrationDelay), TTimeIntervalMicroSeconds32(KUIQStopVibrationDelay), timerCallBack);
#endif
		}
		else
		{
#if (__S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__)
			TRAPD(ignore,This->iVibraControl->StartVibraL(This->iDuration));
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
			This->iVibraTimer->Cancel();
			This->iVibraControl->VibrationOn(100, 1);	// specifies that there should be no off section in the cycle
#endif
		}
	}

	DEBUGMSG(_L("<-CMIDPApp::StartVibrate"));
}

void CMIDPApp::StartVibrate(TInt aDuration)
{
	iDuration = aDuration;
	iThreadRunner->DoCallback(StartVibrateCallback,this);
}
void CMIDPApp::FontInfo(TInt face, TInt style, TInt size,TInt *ascent, TInt *descent, TInt *leading)
{
	iFontManager->FontInfo(face,style,size,ascent,descent,leading);
}

void CMIDPApp::DrawChars(TInt pixel, const TInt16 *clip, const TAny* dst, TInt dotted, TInt face, TInt style,TInt size,TInt x,TInt y,TInt anchor,const TText16 *charArray, TInt n)
{
	iFontManager->DrawChars(pixel,clip,static_cast<const gxj_screen_buffer*>(dst),dotted,face,style,size,x,y,anchor,charArray,n);
}

TInt CMIDPApp::CharsWidth(TInt face, TInt style, TInt size,const TText16 *charArray, TInt n)
{
	return iFontManager->CharsWidth(face,style,size,charArray,n);
}

void CMIDPApp::SetMenus(TMidpCommand *aCommands, int aLength)
{
	iCanvas->SetMenus(aCommands,aLength);
}

void CMIDPApp::RequestOrdinalChange(TInt /*aDisplayId*/, TBool aForeground)
{
	DEBUGMSG(_L("->CMIDPApp::RequestOrdinalChange"));
	iCanvas->RequestOrdinalChange(aForeground);
	DEBUGMSG(_L("<-CMIDPApp::RequestOrdinalChange"));
}

void CMIDPApp::BeginEditSession(TInt aId, TInt aConstraints)
{
	iCanvas->BeginEditSession(aId, aConstraints);
}
void CMIDPApp::EndEditSession()
{
	iCanvas->EndEditSession();
}

TInt CMIDPApp::GetNextState()
{
	return iCanvas->GetNextState();
}

HBufC* CMIDPApp::GetTextFieldString(TInt aId)
{
	return iCanvas->GetInlineText(aId);
}

TInt CMIDPApp::SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId)
{
	return iCanvas->SetTextFieldString(aId, aText, aParentId);
}
void CMIDPApp::SetCursorPosition(TInt aPosition)
{
	iCanvas->SetCursorPosition(aPosition);
}

TBool CMIDPApp::GetUnderline()
{
	return iCanvas->GetUnderline();
}

void CMIDPApp::ShowDisplayableNotify(TInt aDisplayableId)
{
	iCanvas->ShowDisplayableNotify(aDisplayableId);
}

void CMIDPApp::GetEditData(TInt& aEditPos, TInt& aEditLength)
{
	iCanvas->GetEditData(aEditPos,aEditLength);
}

_LIT(KResetAPN,"x-bw-clearapn://:");
_LIT(KEnableAutoAPN,"x-bw-autoapn://:");

void CMIDPApp::LaunchFileCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	RApaLsSession appArcSession;
	if (appArcSession.Connect() == KErrNone)
	{
		TThreadId id;
		TFileName doc(*This->iLaunchBrowserUrl);
		if(doc.Left(8).Compare(_L("file:///")) == 0)
		{
			doc.Delete(0,8);
		}
		TInt len = doc.Length();
		for(TInt i=0;i<len;i++)
		{
			if(doc[i] == '/')
			{
				doc[i] = '\\';
			}
		}
		appArcSession.StartDocument(doc, id);
		appArcSession.Close();
		
	}
}

void CMIDPApp::LaunchBrowserCallback(TAny* aThis)
{	
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	TBool launched = EFalse;
	if (This->iLaunchBrowserUrl)
	{
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
		const TUid browserUid = {0x100051E6};
		TVwsViewId id(KUidQWebApp, KUidQWebPageView);
    	TQWebDNLUrlEntryBuf urlBuf;
    	urlBuf().iUrl = *This->iLaunchBrowserUrl;
    	TRAPD(ignore, CEikonEnv::Static()->EikAppUi()->ActivateViewL(id, KQWebCustomMsgId, urlBuf));
#else
		const TUid browserUid = {0x10008D39};

		RWsSession wsSession;
		if (wsSession.Connect() == KErrNone)
		{
			TApaTaskList taskList(wsSession);
			TApaTask task = taskList.FindApp(browserUid);
			if (task.Exists())
			{
				HBufC8* url8 = HBufC8::New(This->iLaunchBrowserUrl->Length());
				if (url8)
				{
					url8->Des().Append(*This->iLaunchBrowserUrl);
					task.SendMessage(TUid::Uid(0), *url8); 
					delete url8;
					launched = ETrue;
				}
			}
			wsSession.Close();
		}
		
		if (!launched)
		{
			RApaLsSession appArcSession;
			if (appArcSession.Connect() == KErrNone)
			{
				TThreadId id;
				appArcSession.StartDocument(*This->iLaunchBrowserUrl, browserUid, id);
				appArcSession.Close();
			}
		}
#endif
	}
}

void CMIDPApp::DialNumberCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	if (This->iPhoneCall && This->iPhoneNumber)
	{
		This->iPhoneCall->Dial(*This->iPhoneNumber);
	}
}

TBool CMIDPApp::PlatformRequest(const TDesC& aUrl)
{
	_LIT(KInternalInstall,"&x-bw-internal-download");
	_LIT(KFileUrl,"file:");
	_LIT(KPhoneCallUrl,"tel:");
	_LIT(KRegisterMonkey,"x-bw-registermonkey");
	_LIT(KSpankMonkey,"x-bw-spankmonkey");
	_LIT(KDeregisterMonkey,"x-bw-deregistermonkey");
	_LIT(KCustomCommandPrefix,"x-bw-");
#ifdef __TESTING_ONLY_DEVBOX_DOWNLOAD__
	_LIT(KMyURL,"http://172.20.222.21/download/bluewhalemail_midlet.jad");
#endif
	
	DEBUGMSG(_L("->CMIDPApp::PlatformRequest"));
	TBool ret = EFalse;
	RDebug::Print(_L("aURL %S"),&aUrl);
	if (KRegisterMonkey().Compare(aUrl) == 0)
	{
		TInt ignore = RProperty::Set(KUidStarterExe, KUidWatcherMonkey.iUid, EStartMonkey);
	}
	else if (KSpankMonkey().Compare(aUrl) == 0)
	{
		TInt ignore = RProperty::Set(KUidStarterExe, KUidWatcherMonkey.iUid, ESpankMonkey);
	}
	else if (KDeregisterMonkey().Compare(aUrl) == 0)
	{
		TInt ignore = RProperty::Set(KUidStarterExe, KUidWatcherMonkey.iUid, EStopMonkey);
	}
	else if(KResetAPN().Compare(aUrl) == 0)
	{
		iSocketFactory->ResetAPN();
		ret = ETrue;
	}
	else if(KEnableAutoAPN().Compare(aUrl) == 0)
	{
		iSocketFactory->SetAutoAPN();
		ret = ETrue;
	}
	else if(KFileUrl().Compare(aUrl.Left(KFileUrl().Length())) == 0)
	{
		// Open a file
		delete iLaunchBrowserUrl;
		iLaunchBrowserUrl = HBufC::New(aUrl.Length() + 2);
		if (iLaunchBrowserUrl)
		{
			iLaunchBrowserUrl->Des() = aUrl;
			iThreadRunner->DoCallback(LaunchFileCallback,this);
			ret = ETrue;
		}

	}
	else if(KPhoneCallUrl().Compare(aUrl.Left(KPhoneCallUrl().Length())) == 0)
	{
		delete iPhoneNumber;
		iPhoneNumber = HBufC::New(aUrl.Length() - KPhoneCallUrl().Length());
		if (iPhoneNumber)
		{
			iPhoneNumber->Des() = aUrl.Right(aUrl.Length() - KPhoneCallUrl().Length());
			iThreadRunner->DoCallback(DialNumberCallback,this);
			ret = ETrue;
		}
	}
	else if(aUrl.FindF(KInternalInstall) != KErrNotFound)
	{
		// if url contains &x-bw-internal-download
		// then pass back to AMS to install the midlet
		TEventInfo event;
		event.iEvent = KMIDPPlatformRequestInstallEvent;
		event.iIntParam1 = 0;
#ifdef __TESTING_ONLY_DEVBOX_DOWNLOAD__
		event.iStringParam1 = KMyURL().Alloc();
#else
		event.iStringParam1 = aUrl.Alloc();
#endif
		if (event.iStringParam1)
		{
		    AddEvent(event);
			ret = ETrue;			
		}
	}
	else if (aUrl.Left(KCustomCommandPrefix().Length()).CompareF(KCustomCommandPrefix) == 0)
	{
		// "x-bw-" command - ignore
		ret = ETrue;
	}
	else
	{
		delete iLaunchBrowserUrl;
		iLaunchBrowserUrl = HBufC::New(aUrl.Length() + 2);
		if (iLaunchBrowserUrl)
		{
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
			iLaunchBrowserUrl->Des() = aUrl;
#else
			iLaunchBrowserUrl->Des().Format(_L( "4 %S" ), &aUrl);
#endif
			iThreadRunner->DoCallback(LaunchBrowserCallback,this);
			ret = ETrue;
		}
	}
	DEBUGMSG(_L("<-CMIDPApp::PlatformRequest"));
	return ret;
}

TBool CMIDPApp::RegisterConnection(const TDesC& aConnection,const TDesC& /*aMidlet*/,const TDesC& /*aFilter*/)
{
	DEBUGMSG(_L("->CMIDPApp::RegisterConnection"));
	TBool ret = EFalse;
	if(aConnection.Left(KAutostartConnection().Length()).Compare(KAutostartConnection()) == 0)
	{
		TInt err = RProperty::Set(KUidStarterExe, KUidAutoStart.iUid, 1);
		ret = ETrue;

		if (err == KErrNone)
		{
			ret = ETrue;
		}
		TEventInfo info;
		info.iEvent = KMIDPPushRegisterEvent;
		info.iIntParam1 = 1;
		AddEvent(info);

	}
	DEBUGMSG(_L("<-CMIDPApp::RegisterConnection"));
	return ret;
}

TBool CMIDPApp::UnregisterConnection(const TDesC& aConnection)
{
	DEBUGMSG(_L("->CMIDPApp::UnregisterConnection"));
	TBool ret = EFalse;
	if(aConnection.Compare(KAutostartConnection()) == 0)
	{
		TInt err = RProperty::Set(KUidStarterExe, KUidAutoStart.iUid, 0);
		if (err == KErrNone)
		{
			ret = ETrue;
		}
		TEventInfo info;
		info.iEvent = KMIDPPushRegisterEvent;
		info.iIntParam1 = 0;
		AddEvent(info);
	}
	DEBUGMSG(_L("<-CMIDPApp::UnregisterConnection"));
	return ret;
}

void CMIDPApp::PlayAudioCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	TRAPD(err, This->iAudioPlayer->StartL(*This->iAudioType, This->iAudioData));
}

TInt CMIDPApp::PlayAudio(const TDesC8& aType, TPtr8& aData)
{
	delete iAudioType;
	iAudioType = aType.Alloc();
	if (iAudioType)
	{
		iAudioData.Set(aData);
		iThreadRunner->DoCallback(PlayAudioCallback,this);
	}
	return KErrNone;
}

void CMIDPApp::InitializeDecoderCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	This->iImageConverter->Initialize(This->iSourceData);
}

TInt CMIDPApp::InitializeDecoder(TDesC8& aSourceData, TInt& aWidth, TInt& aHeight, TBool& aHasMask)
{
	iSourceData = &aSourceData;
	iThreadRunner->DoSyncCallback(InitializeDecoderCallback, this);
	TInt err = iImageConverter->iStatus.Int();
	if (err == KErrNone)
	{
		TFrameInfo info = iImageConverter->FrameInfo();
		aWidth = info.iOverallSizeInPixels.iWidth;
		aHeight = info.iOverallSizeInPixels.iHeight;
		aHasMask = info.iFlags & TFrameInfo::ETransparencyPossible;
	}
	return err;
}

void CMIDPApp::DecodeImageCallback(TAny* aThis)
{
	CMIDPApp* This = static_cast<CMIDPApp*>(aThis);
	This->iImageConverter->Start(This->iOutData, This->iOutMaskData);
}

TInt CMIDPApp::DecodeImage(char* aOutData, char* aOutMaskData)
{
	if (aOutData == NULL)
	{
		return KErrArgument;
	}
	iOutData = aOutData;
	iOutMaskData = aOutMaskData;
	iThreadRunner->DoSyncCallback(DecodeImageCallback, this);
	TInt err = iImageConverter->iStatus.Int();
	if (err != KErrNone)
	{
		return err;
	}
	iImageConverter->ConvertSemaphore().Wait();
	return iImageConverter->iStatus.Int();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMIDPFontManager::CMIDPFontManager()
: iTextWidthCache(20)
{}

CMIDPFontManager::~CMIDPFontManager()
{
	while (iTextWidthCache.Count())
	{
		delete iTextWidthCache[0];
		iTextWidthCache.Remove(0);
	}
	iTextWidthCache.Close();
	if (iCurrentFont)
	{
		iGc->DiscardFont();
	}
	if (iFontCache)
	{
		CFont* font = NULL;
		while ((font = iFontCache->RemoveFirstEntry()) != NULL)
		{
			iDevice->ReleaseFont(font);
		}
	}
	delete iFontCache;
	delete iGc;
	delete iDevice;
	delete iBitmap;
	delete iThreadRunner;
}

void CMIDPFontManager::ConstructL()
{
	iFontCache = new (ELeave) CFontCache();
	iThreadRunner = new (ELeave) CThreadRunner();
	iThreadRunner->ConstructL();

	iNormalFontSpec = CCoeEnv::Static()->NormalFont()->FontSpecInTwips();
	iNormalFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	iNormalFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
}

void CMIDPFontManager::SetCanvas(MMIDPCanvas* aCanvas)
{
	iCanvas = aCanvas;
}

TBool CMIDPFontManager::SetupFont(TInt aFace,TInt aStyle,TInt aSize)
{
	if( iFace != aFace || iStyle != aStyle || iSize != aSize)
	{
		iFace = aFace;
		iStyle = aStyle;
		iSize = aSize;
		switch(aSize)
		{
		case SIZE_SMALL:
			iCurrentFontSpec = iNormalFontSpec;
			iCurrentFontSpec.iHeight = KSmallFontHeightInTwips;
			iPoints = 0;
			break;
		case SIZE_LARGE:
			iCurrentFontSpec = iNormalFontSpec;
			iCurrentFontSpec.iHeight = KLargeFontHeightInTwips;
			iPoints = 0;
			break;
		case SIZE_MEDIUM:
		default:
			iCurrentFontSpec = iNormalFontSpec;
			iCurrentFontSpec.iHeight = KMediumFontHeightInTwips;
			iPoints = 0;
			break;
		}
		return ETrue;
	}	
	return EFalse;
}

void CMIDPFontManager::FontInfo(TInt face, TInt style, TInt size,TInt *ascent, TInt *descent, TInt *leading)
{
	iCurrentFontSpecHasChanged = SetupFont(face,style,size);
	if (iCurrentFontSpecHasChanged)
	{
		if(iThreadRunner->DoSyncCallback(FontInfoCallback,this) != KErrNone)
		{
			return;
		}
	}
	*ascent = iAscent;
	*descent = iDescent;
	*leading = iLeading;
}

void CMIDPFontManager::DrawChars(TInt pixel, const TInt16 *clip, const gxj_screen_buffer *dst, TInt dotted, TInt face, TInt style,TInt size,TInt x,TInt y,TInt anchor,const TText16 *charArray, TInt n)
{
	iPixel = pixel;
	iClip = clip;
	iDest = dst;
	iDotted = dotted;
	iX = x;
	iY = y;
	iAnchor = anchor;
	iCharArray = charArray;
	iN = n;
	iCurrentFontSpecHasChanged = SetupFont(face,style,size);
	
	iThreadRunner->DoSyncCallback(DrawCharsCallback,this);
	iDest = NULL;
}
TInt CMIDPFontManager::CharsWidth(TInt face, TInt style, TInt size,const TText16 *charArray, TInt n)
{
	iCharsWidth = 0;
	iCharArray = charArray;
	iN = n;
	iCurrentFontSpecHasChanged = SetupFont(face,style,size);
	iThreadRunner->DoSyncCallback(CharsWidthCallback,this);
	
	return iCharsWidth;
}

void CMIDPFontManager::EnsureBitmap()
{
	if (!iBitmap)
	{
		delete iGc;
		iGc = NULL;
		if (iDevice && iCurrentFont)
		{
			iCurrentFont = NULL;
		}
		delete iDevice;
		iDevice = NULL;
		iBitmap = new (ELeave) CFbsBitmap();
		if (iDest)
		{
			iBitmap->Create(TSize(iDest->width, iDest->height),EColor64K);
		}
		else
		{
			iBitmap->Create(iCanvas->Bitmap()->SizeInPixels(),EColor64K);
		}	

	    iDevice = CFbsBitmapDevice::NewL(iBitmap);
	    iDevice->CreateContext(iGc);
	}
	else if (iDest)
	{
		TSize newSize(iDest->width, iDest->height);
		if (newSize != iBitmap->SizeInPixels())
		{
			iBitmap->Resize(newSize);
			iDevice->Resize(newSize);
			iGc->Resized();
		}
	}
}

CTextWidthCacheEntry* CTextWidthCacheEntry::NewL(const TDesC& aText, TInt aFace, TInt aStyle, TInt aSize)
{
	CTextWidthCacheEntry* self = new (ELeave) CTextWidthCacheEntry(aFace, aStyle, aSize);
	CleanupStack::PushL(self);
	self->ConstructL(aText);
	CleanupStack::Pop(self);
	return self;
}

CTextWidthCacheEntry::~CTextWidthCacheEntry()
{
	delete iText;
}

CTextWidthCacheEntry::CTextWidthCacheEntry(TInt aFace, TInt aStyle, TInt aSize)
: iFace(aFace), iStyle(aStyle), iSize(aSize)
{
}

void CTextWidthCacheEntry::ConstructL(const TDesC& aText)
{
	iText = aText.AllocL();
}

TBool CTextWidthCacheEntry::IdentityRelation(const CTextWidthCacheEntry& aObject1, const CTextWidthCacheEntry& aObject2)
{
	return (aObject1.iText->Compare(*aObject2.iText) == 0) && (aObject1.iFace == aObject2.iFace) && (aObject1.iStyle == aObject2.iStyle) && (aObject1.iSize == aObject2.iSize);
}

TInt CMIDPFontManager::TextWidthInPixels(const TDesC &aText)
{
	CTextWidthCacheEntry* findObject = CTextWidthCacheEntry::NewL(aText, iFace, iStyle, iSize);
	TIdentityRelation<CTextWidthCacheEntry> identityRelation(CTextWidthCacheEntry::IdentityRelation);
	TInt index = iTextWidthCache.Find(findObject, identityRelation);
	if (index == KErrNotFound)
	{
		findObject->SetWidth(iCurrentFont->TextWidthInPixels(aText));
		if (iTextWidthCache.Count() > 20)
		{
			delete iTextWidthCache[iTextWidthCache.Count() - 1];
			iTextWidthCache.Remove(iTextWidthCache.Count() - 1);
		}
	}
	else
	{
		delete findObject;
		findObject = iTextWidthCache[index];
		iTextWidthCache.Remove(index);
	}
	iTextWidthCache.Insert(findObject, 0);
	return findObject->Width();
}

TCachedFont::TCachedFont(CFont* aFont, TFontSpec aFontSpec)
: iFont(aFont), iFontSpec(aFontSpec)
{	
}

TBool TCachedFont::IdentityRelation(const TCachedFont& aObject1, const TCachedFont& aObject2)
{
	return (aObject1.iFontSpec == aObject2.iFontSpec);
}

void CMIDPFontManager::UpdateFont()
{
	if (!iCurrentFont || iCurrentFontSpecHasChanged)
	{
		TFontSpec spec = iCurrentFontSpec;
		if(iPoints != 0)
		{
			spec.iHeight = iPoints;
		}
		spec.iFontStyle.SetStrokeWeight(iStyle & STYLE_BOLD ? EStrokeWeightBold : EStrokeWeightNormal);
		spec.iFontStyle.SetPosture(iStyle & STYLE_ITALIC ? EPostureItalic : EPostureUpright);

		CFont* newFont = iFontCache->Search(spec);
		if (!newFont)
		{
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
			TInt ret = iDevice->GetNearestFontToDesignHeightInTwips(newFont,spec);
#else
			TInt ret = iDevice->GetNearestFontInTwips(newFont,spec);
#endif
			if (ret == KErrNone)
			{
				CFont* displacedFont = NULL;
				TRAPD(ignore, displacedFont = iFontCache->AddEntryL(newFont, spec));
				if (displacedFont)
				{
					iDevice->ReleaseFont(displacedFont);
				}
			}
		}
		if (newFont && newFont != iCurrentFont)
		{
			if (iCurrentFont)
			{
				iGc->DiscardFont();
			}
			iCurrentFont = newFont;
			iGc->UseFont(iCurrentFont);
			iAscent = iCurrentFont->AscentInPixels();
			iDescent = iCurrentFont->DescentInPixels();
			iHeight = iCurrentFont->HeightInPixels();
		}
		iLeading = spec.iHeight / 50;
		iGc->SetUnderlineStyle(iStyle & STYLE_UNDERLINED ? EUnderlineOn : EUnderlineOff);
	}
}

void CMIDPFontManager::DrawCharsCallback(TAny* aThis)
{	
	CMIDPFontManager* This = static_cast<CMIDPFontManager*>(aThis);
	TPtr text((TUint16 *)This->iCharArray, This->iN, This->iN);

	This->EnsureBitmap();
	This->UpdateFont();
	
	TInt textWidth = This->TextWidthInPixels(text);
	TInt gap = This->iHeight / 8;
	
	TRect clipRect(This->iClip[0], This->iClip[1], This->iClip[2], This->iClip[3]);

	TRect textRect(TPoint(This->iX, This->iY), TSize(textWidth, This->iHeight));
	
	if(This->iAnchor & RIGHT)
	{
		textRect.Move(-textWidth, 0);
	}
	else if(This->iAnchor & HCENTER)
	{
		textRect.Move(-textWidth / 2, 0);
	}
	
	if(This->iAnchor & BOTTOM)
	{
		textRect.Move(0, -(This->iHeight + gap));
	}
	else if(This->iAnchor & TOP)
	{
		// no shift required
	}
	else
	{
		textRect.Move(0, -(This->iAscent + gap));
	}
	
	TRect intersectRect(clipRect);
	intersectRect.Intersection(textRect);
	if (!intersectRect.IsEmpty())
	{
		TInt topScanLineOffset = This->iDest->width * textRect.iTl.iY * 2;
		topScanLineOffset = Max(topScanLineOffset, 0);
		topScanLineOffset = Min(topScanLineOffset, This->iDest->width * This->iDest->height * 2);
		
		TInt bottomScanLineOffset = (This->iDest->width * (textRect.iTl.iY + gap + This->iHeight)) * 2;
		bottomScanLineOffset = Max(bottomScanLineOffset, 0);
		bottomScanLineOffset = Min(bottomScanLineOffset, This->iDest->width * This->iDest->height * 2);
		
		This->iBitmap->LockHeap();
		TUint32* bitmapData = This->iBitmap->DataAddress();
		Mem::Copy(bitmapData, ((TUint8*)This->iDest->pixelData) + topScanLineOffset, bottomScanLineOffset - topScanLineOffset);
		This->iBitmap->UnlockHeap();

		clipRect.Move(0, -textRect.iTl.iY);
		TBool useClippingRect = (intersectRect != textRect);
		if (useClippingRect)
		{
			This->iGc->SetClippingRect(clipRect);
		}
		This->iGc->SetPenColor(TRgb(This->iPixel));
		This->iGc->DrawText(text, TPoint(textRect.iTl.iX, gap + This->iAscent));
		if (useClippingRect)
		{
			This->iGc->CancelClippingRect();
		}
		
		This->iBitmap->LockHeap();
		bitmapData = This->iBitmap->DataAddress();
		Mem::Copy(((TUint8*)This->iDest->pixelData) + topScanLineOffset, bitmapData, bottomScanLineOffset - topScanLineOffset);
		This->iBitmap->UnlockHeap();
	}
}

void CMIDPFontManager::FontInfoCallback(TAny* aThis)
{
	CMIDPFontManager* This = static_cast<CMIDPFontManager*>(aThis);

	This->EnsureBitmap();
	This->UpdateFont();
}

void CMIDPFontManager::CharsWidthCallback(TAny* aThis)
{
	CMIDPFontManager* This = static_cast<CMIDPFontManager*>(aThis);
	TPtr text((TUint16 *)This->iCharArray, This->iN, This->iN);

	This->EnsureBitmap();
	This->UpdateFont();
	This->iCharsWidth = (This->iN == 0) ? 0 : This->TextWidthInPixels(text);
}

CImageConverter* CImageConverter::NewL()
{
	CImageConverter* self = new (ELeave) CImageConverter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CImageConverter::~CImageConverter()
{
	Cancel();
	iConvertSemaphore.Close();
	delete iBitmap;
	delete iMask;
	delete iImageDecoder;
}

void CImageConverter::Initialize(TDesC8* aSourceData)
{
	ASSERT(aSourceData);
	if (iImageDecoder)
	{
		delete iImageDecoder;
		iImageDecoder = NULL;
	}
	TRAPD(err, iImageDecoder = CImageDecoder::DataNewL(CCoeEnv::Static()->FsSession(), *aSourceData, CImageDecoder::EOptionNone));
	iStatus = err;
}

void CImageConverter::StartL()
{
	TFrameInfo info = iImageDecoder->FrameInfo();
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(info.iOverallSizeInPixels, EColor64K));

	iMask = NULL;
	if (iOutMaskData)
	{
		iMask = new (ELeave) CFbsBitmap;
		User::LeaveIfError(iMask->Create(info.iOverallSizeInPixels, EGray256));
	}
}

void CImageConverter::Start(char* aOutData, char* aOutMaskData)
{
	ASSERT(iImageDecoder);
	
	iOutData = aOutData;
	iOutMaskData = aOutMaskData;

	TRAPD(err, StartL());
	iStatus = err;
	if (iStatus == KErrNone)
	{
		if (iOutMaskData)
		{
			iImageDecoder->Convert(&iStatus, *iBitmap, *iMask);
		}
		else
		{
			iImageDecoder->Convert(&iStatus, *iBitmap);
		}
		SetActive();
	}
}

void CImageConverter::RunL()
{
	ASSERT(iOutData);
	if (iStatus == KErrNone)
	{
		TFrameInfo info = iImageDecoder->FrameInfo();
		TInt internalScanLineBytes = iBitmap->ScanLineLength(info.iOverallSizeInPixels.iWidth, EColor64K);	// padded to be DWORD aligned
		TInt scanLineBytes = info.iOverallSizeInPixels.iWidth * 2;
		iBitmap->LockHeap();
		if (scanLineBytes == internalScanLineBytes)
		{
			// no padding so copy all bitmap data
			TUint32* bitmapData = iBitmap->DataAddress();
			Mem::Copy(iOutData, bitmapData, scanLineBytes * info.iOverallSizeInPixels.iHeight);
		}
		else
		{
			// copy each scanline, avoiding padding
			TUint8* src = (TUint8*)iBitmap->DataAddress();
			TUint8* trg = (TUint8*)iOutData;
			for (TInt y = 0; y < info.iOverallSizeInPixels.iHeight; y++)
			{
				Mem::Copy(trg, src, scanLineBytes);
				src += internalScanLineBytes;
				trg += scanLineBytes;
			}
		}
		iBitmap->UnlockHeap();

		if (iOutMaskData)
		{
			TInt internalScanLineBytes = iMask->ScanLineLength(info.iOverallSizeInPixels.iWidth, EGray256);	// padded to be DWORD aligned
			TInt scanLineBytes = info.iOverallSizeInPixels.iWidth;
			iMask->LockHeap();
			if (scanLineBytes == internalScanLineBytes)
			{
				// no padding so copy all bitmap data
				TUint32* maskData = iMask->DataAddress();
				Mem::Copy(iOutMaskData, maskData, scanLineBytes * info.iOverallSizeInPixels.iHeight);
			}
			else
			{
				// copy each scanline, avoiding padding
				TUint8* src = (TUint8*)iMask->DataAddress();
				TUint8* trg = (TUint8*)iOutMaskData;
				for (TInt y = 0; y < info.iOverallSizeInPixels.iHeight; y++)
				{
					Mem::Copy(trg, src, scanLineBytes);
					src += internalScanLineBytes;
					trg += scanLineBytes;
				}
			}
			iMask->UnlockHeap();
		}
	}

	delete iMask;
	iMask = NULL;
	delete iBitmap;
	iBitmap = NULL;
	delete iImageDecoder;
	iImageDecoder = NULL;
	iConvertSemaphore.Signal();
}

RSemaphore& CImageConverter::ConvertSemaphore()
{
	return iConvertSemaphore;
}

void CImageConverter::DoCancel()
{
	iImageDecoder->Cancel();
}

TInt CImageConverter::RunError(TInt aError)
{
	iStatus = aError;
	iConvertSemaphore.Signal();
	return KErrNone;
}

TFrameInfo CImageConverter::FrameInfo()
{
	return iImageDecoder->FrameInfo();
}

CImageConverter::CImageConverter() :
CActive(CActive::EPriorityStandard)
{
	CActiveScheduler::Add(this);
}

void CImageConverter::ConstructL()
{
	User::LeaveIfError(iConvertSemaphore.CreateLocal(0));
}

