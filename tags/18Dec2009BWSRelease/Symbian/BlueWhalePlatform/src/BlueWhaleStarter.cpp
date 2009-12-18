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


#include <apacmdln.h>
#include <apgcli.h>
#include <eikdll.h>
#include "BlueWhaleStarter.h"
#include <apgtask.h>
#include <OSVersion.h>
#include <bautils.h>

#define __MONKEY_LOGGING__

#ifdef __MONKEY_LOGGING__
#define __ENABLE_LOG__
_LIT(KLogFileName,"WatcherMonkey.txt");
#endif
#include "FileLogger.h"

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__)
const TInt KInstallerAppValue=0x101F875A;
const TInt KMenuAppValue=0x101F4CD2;
const TInt KPhoneAppValue=0x100058B3;
#elif  (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)		
const TInt KInstallerAppValue=0x10000419;
const TInt KMenuAppValue=0x101F4CD2;
const TInt KPhoneAppValue=0x100058B3;
#else
const TInt KInstallerAppValue=0x10000419;
const TInt KMenuAppValue=0x101F4CD2;
const TInt KPhoneAppValue=0x100058B3;
#endif

const TUid KUidInstallerApp = {KInstallerAppValue};
const TUid KUidMenuApp = {KMenuAppValue};
const TUid KUidPhoneApp = {KPhoneAppValue};

const TInt KBrowserAppValue=0x10008D39;
const TUid KUidBrowserApp = {KBrowserAppValue};

#define KSleepTimeUntilNextBootCheck			10000000	// 10 seconds
#define KSleepTimeAfterBooted					30000000	// 30 seconds
#define KSleepTimePlatformRunning				30000000	// 30 seconds
#define KSleepTimeUntilNextInstallerAppCheck	1000000		// 1 second
#define KSleepTimeUntilNextBrowserAppCheck		10000000	// 10 second
#define KMonkeyTimeout							30000000	// 30 seconds

_LIT(KBootUpThreadName, "BlueWhaleStarterThr");
_LIT(KAutostartFile,"autostart.txt");


CWatcherMonkey* CWatcherMonkey::NewL(RFs& aFs)
{
	DEBUGMSG(_L("CWatcherMonkey::NewL"));
	CWatcherMonkey* self = new CWatcherMonkey(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CWatcherMonkey::~CWatcherMonkey()
{
	Cancel();
	iProperty.Close();
	if (iTimer)
	{
		iTimer->Cancel();
	}
	delete iTimer;
}
	
void CWatcherMonkey::DoCancel()
{
	iProperty.Cancel();
}

TInt CWatcherMonkey::TimeOutCallBack(TAny* aPtr)
{
	CWatcherMonkey* This = static_cast<CWatcherMonkey*>(aPtr);
	This->iTimer->Cancel();
	TRAPD(err, This->RestartAppL());
	return err;
}

void CWatcherMonkey::RestartAppL()
{
	DEBUGMSG(_L("CWatcherMonkey::RestartAppL"));
	iAnnoyed = EFalse;
	TFullName res;
	TFindProcess find;
	while (find.Next(res) == KErrNone)
	{
		RProcess process;
		if (process.Open(find) == KErrNone)
		{
			if (process.Type().MostDerived() == KUidBlueWhalePlatformApp)
			{
				DEBUGMSG(_L("Killing platform"));
				TRequestStatus requestStatus;
				process.Logon(requestStatus);
				process.Kill(KErrLocked);
				User::WaitForRequest(requestStatus);
				DEBUGMSG1(_L("Platform killed %d"), requestStatus.Int());
				process.Close();
				break;
			}
			process.Close();
		}
	}

	DEBUGMSG(_L("Restarting platform"));
	CBlueWhaleStarter::FindAndLaunchPlatformL(iFs, ETrue);
}

void CWatcherMonkey::StartTimer()
{
	iTimer->Cancel();
	TCallBack callBack(TimeOutCallBack, this);
	iTimer->Start(TTimeIntervalMicroSeconds32(KMonkeyTimeout), TTimeIntervalMicroSeconds32(KMonkeyTimeout), callBack);
}

void CWatcherMonkey::StopTimer()
{
	iTimer->Cancel();
}

void CWatcherMonkey::RunL()
{
	TInt value = 0;
	iProperty.Get(KUidStarterExe, KUidWatcherMonkey.iUid, value);
	DEBUGMSG1(_L("CWatcherMonkey::RunL KUidWatcherMonkey value = %d"), value);
	switch (value)
	{
	case EStartMonkey:
	{
		StartTimer();
		break;
	}
	case ESpankMonkey:
	{
		if (!iAnnoyed)
		{
			StopTimer();
			StartTimer();
		}
		break;
	}
	case EStopMonkey:
	{
		StopTimer();
		break;
	}
	case EAnnoyMonkey:
	{
		iAnnoyed = ETrue;
		break;
	}
	}

	iProperty.Subscribe(iStatus);
	SetActive();
}

CWatcherMonkey::CWatcherMonkey(RFs& aFs)
: CActive(CActive::EPriorityStandard), iFs(aFs)
{
}

void CWatcherMonkey::ConstructL()
{
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);

	RProperty::Define(KUidStarterExe, KUidWatcherMonkey.iUid, RProperty::EInt);
	User::LeaveIfError(iProperty.Attach(KUidStarterExe, KUidWatcherMonkey.iUid));

	CActiveScheduler::Add(this);
	iProperty.Subscribe(iStatus);
	SetActive();
}


CAutoStartWatcher* CAutoStartWatcher::NewL(RFs& aFs)
{
	CAutoStartWatcher* self = new CAutoStartWatcher(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CAutoStartWatcher::~CAutoStartWatcher()
{
	Cancel();
	iProperty.Close();
}
	
void CAutoStartWatcher::DoCancel()
{
	iProperty.Cancel();
}

void CAutoStartWatcher::RunL()
{
	TInt value = 0;
	iProperty.Get(KUidStarterExe, KUidAutoStart.iUid, value);
	if (value == 0)
	{
		iFs.Delete(iAutoStartFileName);
	}
	else
	{
		RFile file;
		iFs.MkDirAll(iAutoStartFileName);
		if (file.Create(iFs, iAutoStartFileName, EFileShareAny | EFileWrite) == KErrNone)
		{
			file.Close();
		}
	}

	iProperty.Subscribe(iStatus);
	SetActive();
}

TBool CAutoStartWatcher::OkToStart()
{
	TInt value = 0;
	iProperty.Get(KUidStarterExe, KUidAutoStart.iUid, value);
	return value;
}

CAutoStartWatcher::CAutoStartWatcher(RFs& aFs)
: CActive(CActive::EPriorityStandard), iFs(aFs)
{
}

void CAutoStartWatcher::ConstructL()
{	
	TBuf<2> drive;
#ifdef __WINSCW__
	drive = _L("c:"); 
#else
	iAutoStartFileName = RProcess().FileName(); // Get the drive letter
	TParsePtrC parse(iAutoStartFileName);
	drive = parse.Drive();
#endif
	_LIT(KLitPrivatePath, "\\private\\%08x\\");
	TBuf<18> pathToUid;
	pathToUid.Format(KLitPrivatePath, KUidStarterExe);
	
	iAutoStartFileName = drive;
	iAutoStartFileName.Append(pathToUid);
	iAutoStartFileName.Append(KAutostartFile);	

	RProperty::Define(KUidStarterExe, KUidAutoStart.iUid, RProperty::EInt);
	User::LeaveIfError(iProperty.Attach(KUidStarterExe, KUidAutoStart.iUid));
	if (BaflUtils::FileExists(iFs, iAutoStartFileName))
	{
		iProperty.Set(KUidStarterExe, KUidAutoStart.iUid, 1);
	}
	else
	{
		iProperty.Set(KUidStarterExe, KUidAutoStart.iUid, 0);
	}

	CActiveScheduler::Add(this);
	iProperty.Subscribe(iStatus);
	SetActive();
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__) || (!defined __WINSCW__)
	RProcess().Rendezvous(KErrCompletion);
#else
	RThread().Rendezvous(KErrCompletion);
#endif
}




CBlueWhaleStarter* CBlueWhaleStarter::NewL()
{
	CBlueWhaleStarter* self = new (ELeave) CBlueWhaleStarter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CBlueWhaleStarter::ConstructL()
{
	CTimer::ConstructL();

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iWsSession.Connect());
	
	iAutoStartWatcher = CAutoStartWatcher::NewL(iFs);
	iWatcherMonkey = CWatcherMonkey::NewL(iFs);

	if (!AppIsRunning(KUidInstallerApp))
	{
		iDeviceStartup = ETrue;
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#else
		// ensure system has booted - wait until phone app (menu app on emulator) has started
		TApaTaskList taskList(iWsSession);
		while (1)
		{
#ifdef __WINS__
			TApaTask task = taskList.FindApp(KUidMenuApp);
#else
			TApaTask task = taskList.FindApp(KUidPhoneApp);
#endif
			if (task.Exists())
			{
				break;
			}
			User::After(KSleepTimeUntilNextBootCheck);
		}

		// give system time to start up
		User::After(KSleepTimeAfterBooted);
#endif
	}

	// start the main monitor loop
	iLaunched = EFalse;
	iStarterState = ELaunchingPlatform;
	CActiveScheduler::Add(this);
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
}

CBlueWhaleStarter::CBlueWhaleStarter()
: CTimer(CActive::EPriorityStandard)
{
}

CBlueWhaleStarter::~CBlueWhaleStarter()
{
	Cancel();
	iWsSession.Close();
	iFs.Close();
	delete iAutoStartWatcher;
	delete iWatcherMonkey;
}

void CBlueWhaleStarter::MonitorAppL()
{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CBlueWhaleStarter* starter = CBlueWhaleStarter::NewL();
	CleanupStack::PushL(starter);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(starter);
	CleanupStack::PopAndDestroy(scheduler);
}

TBool CBlueWhaleStarter::AppIsRunning(const TUid aUid)
{
	TBool result = EFalse;
	TFullName res;
	TFindProcess find;
	while (find.Next(res) == KErrNone)
	{
		RProcess process;
		if (process.Open(find) == KErrNone)
		{
			if (process.Type().MostDerived() == aUid)
			{
				process.Close();
				result = ETrue;
				break;
			}
			process.Close();
		}
	}

	return result;
}

void CBlueWhaleStarter::FindAndLaunchPlatformL(RFs& aFs, TBool aBackground)
{
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	cmdLine->SetExecutableNameL(_L("\\sys\\bin\\BlueWhalePlatform.exe"));	
#else
	TFindFile findFile(aFs);
	User::LeaveIfError(findFile.FindByDir(_L("\\system\\apps\\bluewhaleplatform\\bluewhaleplatform.app"), KNullDesC));
	cmdLine->SetLibraryNameL(findFile.File());
#endif
	if (aBackground)
	{
		cmdLine->SetCommandL(EApaCommandBackground);
	}
	else
	{
		cmdLine->SetCommandL(EApaCommandRun);
	}

	RApaLsSession lsSession;
	User::LeaveIfError(lsSession.Connect());
	lsSession.StartApp(*cmdLine);
	lsSession.Close();
	CleanupStack::PopAndDestroy(cmdLine);
}

void CBlueWhaleStarter::RunL()
{
	switch (iStarterState)
	{
	case ELaunchingPlatform:
	{
		if (AppIsRunning(KUidBlueWhalePlatformApp))
		{
			iLaunched = ETrue;
			iTimeToNextCheck = KSleepTimePlatformRunning;
		}
#if (__S60_VERSION__ > __S60_V2_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#else
		else if (AppIsRunning(KUidBrowserApp))
		{
			iStarterState = EWaitingForBrowserClose;
			iTimeToNextCheck = KSleepTimeUntilNextBrowserAppCheck;
		}
#endif
		else if (AppIsRunning(KUidInstallerApp))
		{
#if (__S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__)
			iStarterState = EWaitingForInstallerClose;
			iTimeToNextCheck = KSleepTimeUntilNextInstallerAppCheck;
#elif (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
			// the installer takes care of launching the app so just finish
			iLaunched = ETrue;
			iTimeToNextCheck = KSleepTimePlatformRunning;
#endif
		}
		else if (!iLaunched)
		{
			if (iAutoStartWatcher->OkToStart())
			{
				FindAndLaunchPlatformL(iFs, iDeviceStartup);
			}
			iTimeToNextCheck = KSleepTimePlatformRunning;
			iLaunched = ETrue;
		}
		else
		{
			iTimeToNextCheck = KSleepTimePlatformRunning;
		}
		break;
	}
	case EWaitingForBrowserClose:
	{
		if (!AppIsRunning(KUidBrowserApp))
		{
			iTimeToNextCheck = 0;
			iStarterState = ELaunchingPlatform;
			iLaunched = EFalse;
		}
		break;
	}
	case EWaitingForInstallerClose:
	{
		if (!AppIsRunning(KUidInstallerApp))
		{
			iTimeToNextCheck = 0;
			iStarterState = ELaunchingPlatform;
		}
		break;
	}
	}

#if (__S60_VERSION__ > __S60_V2_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	if (!iLaunched)
	{
		CTimer::After(iTimeToNextCheck);
	}
#else
	CTimer::After(iTimeToNextCheck);
#endif
}
	
#if !defined(EKA2) && defined(__WINS__)

EXPORT_C TInt InitEmulator()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CBlueWhaleStarter::MonitorAppL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	User::Exit(0);	
	return KErrNone;
}

GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}

#else

GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CBlueWhaleStarter::MonitorAppL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	User::Exit(0);
	return KErrNone;
}

#endif


