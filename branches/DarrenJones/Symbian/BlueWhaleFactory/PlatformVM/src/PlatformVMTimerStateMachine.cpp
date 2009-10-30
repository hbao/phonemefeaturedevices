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
#include <e32base.h>
#include <e32property.h>
#include <hal.h>
#include <hal_data.h>
#include <os_symbian.hpp>
#include "debugutilities.h"
#include <unknown.h>
#include "PlatformVMTimerStateMachine.h"	
#include "Application.h"
#include "DirectoryList.h"
#include "CommandMessage.h"
#include "DataAccess.h"
#include "WritableConnection.h"
#include "ViewInterface.h"
#include "SocketEngine.h"
#include "buildversion.h"
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__ || __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#else
#include <eikdll.h>
#endif

//#define __DEBUGTIMER__
//#define __DEBUGSTATEMACHINE__

#ifdef __DEBUGSTATEMACHINE__
#define DEBUGSTATEMACHINEMSG(_AA)RDebug::Print(_AA)
#else
#define DEBUGSTATEMACHINEMSG
#endif

#ifdef __DEBUGTIMER__
#define DEBUGMESSAGE(_AA) RDebug::Print(_AA)
#define DEBUGMESSAGE1(_AA,_BB) RDebug::Print(_AA,_BB)
#define DEBUGMESSAGE2(_AA,_BB,_CC) RDebug::Print(_AA,_BB,_CC)
#define DEBUGMESSAGE3(_AA,_BB,_CC,_DD) RDebug::Print(_AA,_BB,_CC,_DD)
#else
#define DEBUGMESSAGE(_AA)
#define DEBUGMESSAGE1(_AA,_BB)
#define DEBUGMESSAGE2(_AA,_BB,_CC)
#define DEBUGMESSAGE3(_AA,_BB,_CC,_DD)
#endif

_LIT(KVMThreadName,"_BWMVMT%d");
_LIT(KManThreadName,"_BWMMan%d");

class CSymbianSocket;
class CTicker;
class CSocketFactoryServer;
class CDirectoryWatcherFactory;
class CDirectoryWatcher;
class CThreadStopper;


////////////////////////////////////////////////////////////////////////////////////////
MUnknown * CVMTimerStateMachine::NewL( TAny * aConstructionParameters )
{
	CVMTimerStateMachine* self = new (ELeave) CVMTimerStateMachine(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MUnknown * unknown = self->QueryInterfaceL( KIID_MUnknown );
	CleanupStack::Pop(self);
	return unknown;
}

CVMTimerStateMachine::CVMTimerStateMachine(TAny * aConstructionParameters ) : CEComPlusRefCountedBase(aConstructionParameters)
{
	iProperties = static_cast<MProperties*>(aConstructionParameters);
	if(iProperties)
	{
		iProperties->AddRef();
	}
}

CVMTimerStateMachine::~CVMTimerStateMachine()
{
	DEBUGSTATEMACHINEMSG(_L("->CVMTimerStateMachine::~CVMTimerStateMachine"));
#if _DEBUGTERMINAL_
	if(iDebugThread)
	{
		iDebugThread->Release();
		iDebugThread = NULL;
	}
/*	if(iDebugTerminal)
	{
		iDebugTerminal->Release();
		iDebugTerminal = NULL;
	}
*/
#endif	
	if(iVMThread)
	{
		iVMThread->Release();
	}
	delete iJVM;
	if(iVMManager)
	{
		iVMManager->Release();
	}
	delete iApp;
	iRunningThreads.Close();
	delete iDebugTimer;
	delete iRestartTimer;
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	DEBUGSTATEMACHINEMSG(_L("<-CVMTimerStateMachine::~CVMTimerStateMachine"));
}

MUnknown * CVMTimerStateMachine::QueryInterfaceL( TInt aInterfaceId )
{	
	if( KIID_MTimerStateMachine == aInterfaceId )
	{
		AddRef();
		return static_cast<MTimerStateMachine*>(this);
	}
	else if(KIID_MVMObjectFactoryClient == aInterfaceId)
	{
		AddRef();
		return static_cast<MVMObjectFactoryClient*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

const TTimeIntervalMicroSeconds32 KDebugTimerTimeout = 10 * 1000000;

TInt CVMTimerStateMachine::DebugTimerFunction(TAny* aSelf)
{
	CVMTimerStateMachine* This = static_cast<CVMTimerStateMachine*>(aSelf);
	This->iApp->DebugResources();
	return KErrNone;
}

void CVMTimerStateMachine::DoInitialStateL(TCommand aCommand, MProperties * /*aCommandProperties*/)
{
	if(aCommand == KCommandOpen) // the only command we accept in this state
	{
		iState = EReady;
#if _DEBUGTERMINAL_
		MProperties* props = DiL(MProperties);
		CleanupReleasePushL(*props);
		props->SetStringL(KPropertyThreadNameString,_L("BWMDEBUG"));
		iDebugThread = static_cast<MThread*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MThread),TUid::Uid(KIID_MThread),props));
		iDebugThread->Thread().SetPriority(EPriorityMuchLess);
		CleanupStack::PopAndDestroy(props);
		
		iDebugTerminal =  new (ELeave)CDebugTerminal(NULL,this);
		iDebugTerminal->ConstructL();
		
		iDebugThread->AddL(iDebugTerminal);
		CMyUndertaker* undertaker = new (ELeave)CMyUndertaker(&iDebugThread->Thread(),this);
		CleanupStack::PushL(undertaker);
		undertaker->StartL();
		User::LeaveIfError(iRunningThreads.Append(TThreadInfo(iDebugThread,undertaker,iDebugThread->Thread().Id())));
		CleanupStack::Pop(undertaker);
		iDebugThread->StartL();
#endif
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
}
void CVMTimerStateMachine::DoReadyStateL(TCommand aCommand, MProperties* aCommandProperties)
{
	if(aCommand == KCommandOnline)
	{
		delete iRestartTimer;
		iRestartTimer = NULL;
		delete iDebugTimer;
		iDebugTimer = NULL;
		iDebugTimer = CPeriodic::NewL(CActive::EPriorityIdle);
		if (aCommandProperties)
		{		
			CreateVML(aCommandProperties->GetString8L(KPropertyString8ShortcutName), aCommandProperties->GetIntL(KPropertyIntAutoStarted));
		}
		else
		{
			CreateVML(KNullDesC8, EFalse);
		}
		iVMManager->StartL();
		iVMThread->StartL();
		TCallBack callback(DebugTimerFunction,this);
		iDebugTimer->Start(KDebugTimerTimeout,KDebugTimerTimeout,callback);
		iState = EOnline;
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
}

void CVMTimerStateMachine::DoOnlineStateL(TCommand aCommand, MProperties * /*aCommandProperties*/)
{	
	if(aCommand == KCommandOffline)
	{
		iState = EOffline;
	}
	else if(aCommand == KCommandClose)
	{
		iState = EClosing;
	}
	else if(aCommand == KCommandKill)
	{
		delete iDebugTimer;
		iDebugTimer = NULL;
		iVMThread->Kill(0);
		iVMManager->Kill(0);
#if _DEBUGTERMINAL_
		iDebugThread->Kill(0);
#endif
		iState = EKilling;
	}
	else if(aCommand == KCommandOnline)
	{
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
}

void CVMTimerStateMachine::DoOfflineStateL(TCommand aCommand, MProperties * /*aCommandProperties*/)
{
	if(aCommand == KCommandOnline)
	{
		iState = EOnline;
	}
	else if(aCommand == KCommandClose)
	{
		iState = EClosing;
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
}

void CVMTimerStateMachine::DoClosingStateL(TCommand aCommand, MProperties * /*aCommandProperties*/)
{
	if(aCommand == KCommandKill)
	{
		iState = EKilling;
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
	
}

void CVMTimerStateMachine::DoKillingStateL(TCommand aCommand, MProperties * /*aCommandProperties*/)
{
	if(aCommand == KCommandOpen)
	{
		iState = EReady;
		if(iRestartTimer)
		{
			const TTimeIntervalMicroSeconds32 KRestartDelay = 10 * 1000000;
			TCallBack callback(RestartTimerFunction,this);
			iRestartTimer->Start(KRestartDelay,KRestartDelay,callback);
		}
	}
	else
	{
		User::Leave(KErrCommandNotExpected);
	}
}

void CVMTimerStateMachine::DoEndStateL(TCommand /*aCommand*/, MProperties * /*aCommandProperties*/)
{
	User::Leave(KErrCommandNotExpected);
}

TInt CVMTimerStateMachine::RestartTimerFunction(TAny* aSelf)
{
	CVMTimerStateMachine* self = static_cast<CVMTimerStateMachine*>(aSelf);
	TRAPD(err,self->AcceptCommandL(KCommandOnline,NULL));
	return err;
}

TBool CVMTimerStateMachine::AcceptCommandL(TCommand aCommand, MProperties * aCommandProperties)
{	
	// validate the given command.
	if(aCommand != KCommandOpen && aCommand != KCommandOnline && aCommand != KCommandOffline && aCommand != KCommandClose
			&& aCommand != KCommandKill)
	{
		User::Leave(KErrArgument);
	}
	switch(iState)
	{
	case EInitial:
		DoInitialStateL(aCommand,aCommandProperties);
		break;
	case EReady:
		DoReadyStateL(aCommand,aCommandProperties);
		break;
	case EOnline:
		DoOnlineStateL(aCommand,aCommandProperties);
		break;
	case EOffline:
		DoOfflineStateL(aCommand,aCommandProperties);
		break;
	case EKilling:
		DoKillingStateL(aCommand,aCommandProperties);
		break;
	case EEnd:
		DoEndStateL(aCommand,aCommandProperties);
		break;
	default:
		User::Leave(KErrInvalidState); 
	}
	return ETrue;
}

void CVMTimerStateMachine::Reset()
{}

void CVMTimerStateMachine::CreateVML(const TDesC8& aShortcutName, const TBool aAutoStarted)
{
	TBuf<32> name;
	name.Format(KManThreadName(),iInstanceCount);

	iVMManager = iFactory->CreateVMManagerThreadObject(name);
	iApp = iFactory->CreateVMManagerObject();
	CMyUndertaker* undertaker = new (ELeave)CMyUndertaker(&iVMManager->Thread(),this);
	CleanupStack::PushL(undertaker);
	undertaker->StartL();
	User::LeaveIfError(iRunningThreads.Append(TThreadInfo(iVMManager,undertaker,iVMManager->Thread().Id())));
	CleanupStack::Pop(undertaker);
	
	name.Format(KVMThreadName(),iInstanceCount);

	iVMThread = iFactory->CreateVMThreadObject(name);
	
	iJVM = new (ELeave) CJVMRunner(reinterpret_cast<MApplication*>(iApp), aShortcutName, aAutoStarted);
	iVMThread->AddL(iJVM);
	undertaker = new (ELeave)CMyUndertaker(&iVMThread->Thread(),this);
	CleanupStack::PushL(undertaker);
	undertaker->StartL();
	User::LeaveIfError(iRunningThreads.Append(TThreadInfo(iVMThread,undertaker,iVMThread->Thread().Id())));
	CleanupStack::Pop(undertaker);

	iInstanceCount++;
}

void CVMTimerStateMachine::InitializeL( MProperties * /*aInitializationProperties*/ /* IN */)
{
}

TInt CVMTimerStateMachine::StopRunningThreads(TThreadId aId)
{
	TInt stoppedThreads = 0;
	TInt count = iRunningThreads.Count();
	// clear down thread that just stopped
	for(TInt i=0;i<count;i++)
	{
		if(iRunningThreads[i].iId == aId)
		{
			delete iRunningThreads[i].iUndertaker;
			iRunningThreads[i].iUndertaker = NULL;
			iRunningThreads[i].iId = 0;
			iRunningThreads[i].iThread = NULL;
		}
	}
	for(TInt i=0;i<count;i++)
	{
		DEBUGMESSAGE1(_L("Checking %d "),iRunningThreads[i].iId);
		if(iRunningThreads[i].iId == TThreadId(0))
		{
			stoppedThreads++;
		}
		else if(iRunningThreads[i].iId != aId)
		{
			iRunningThreads[i].iThread->StopL();
			break;
		}
	}
	return stoppedThreads;
}

void CVMTimerStateMachine::ThreadTerminated(TThreadId aId,TInt /*aReason*/)
{
	DEBUGMESSAGE2(_L("ThreadTerminated %d %d"),aId,aReason);
	if(iDebugTimer)
	{
		iDebugTimer->Cancel();
		delete iDebugTimer;
		iDebugTimer = NULL;
	}
	// a thread has stopped. Ask the other to stop
	TInt stoppedThreads = StopRunningThreads(aId);

	if(iState == EOnline)
	{
		// create the restart timer.
		delete iRestartTimer;
		iRestartTimer = NULL;
		iRestartTimer = CPeriodic::NewL(CActive::EPriorityStandard);
		iState = EKilling;
	}
	else if(iState == EClosing)
	{
		iState = EKilling;
	}

	TInt count = iRunningThreads.Count();
	if(stoppedThreads == count) // all threads now stopped !
	{
		TInt vmCompleteStatus = KErrNone;
		if(iVMThread)
		{
			vmCompleteStatus = iVMThread->CompleteStatus();
			iVMThread->Release();
		}
		iVMThread = NULL;
		delete iJVM;
		iJVM = NULL;
		if(iVMManager)
		{
			iVMManager->Release();
		}
		iVMManager = NULL;
		delete iApp;
		iApp = NULL;
		iRunningThreads.Reset();
#if _DEBUGTERMINAL_
		if(iDebugThread)
		{
			iDebugThread->Release();
			iDebugThread = NULL;
		}
/*		if(iDebugTerminal)
		{
			iDebugTerminal->Release();
			iDebugTerminal = NULL;
		}
*/
#endif
		MViewAppUiCallback* appUi = NULL;
		if(iProperties)
		{
			TRAPD(ignore,appUi = static_cast<MViewAppUiCallback*>(iProperties->GetObjectL(KPropertyObjectViewAppUiCallback,KIID_MViewAppUiCallback)));
			appUi->RequestExit(vmCompleteStatus);
		}
	}
}

TBool CVMTimerStateMachine::Online() const
{
	return (iState == EOnline);
}

TBool CVMTimerStateMachine::Offline() const
{
	return (iState == EOffline);
}

//////////////////////////////////////////////////////////////////////////////////////////////
CJVMRunner::CJVMRunner(MApplication* aApplication, const TDesC8& aShortcutName, const TBool aAutoStarted)
: CActive(EPriorityNormal), iApplication(aApplication), iShortcutName(aShortcutName), iAutoStarted(aAutoStarted)
{
}

CJVMRunner::~CJVMRunner()
{}

void CJVMRunner::ConstructL()
{}

//#define __VMUNITTESTS__

#ifdef __VMUNITTESTS__
_LIT8(KAMSRunner,"com.bluewhalesystems.midp.AMSTestRunner");
#else
//_LIT8(KAMSRunner,"com.bluewhalesystems.midp.AMSRunner");
_LIT8(KAMSRunner,"com.sun.midp.main.MIDletSuiteLoader");
#endif

_LIT8(KTestRMS,"javax.microedition.lcdui.TestGraphics");

_LIT8(KUserAgentKey,"user-agent");
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
_LIT8(KUserAgentBase,"bluewhale_%S Profile/MIDP-2.1 Configuration/CLDC-1.1 SymbianOS/ %S 0x%08x-0x%08x bluewhale_platform_%S");
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
_LIT8(KUserAgentBase,"bluewhale_%S Profile/MIDP-2.1 Configuration/CLDC-1.1 Symbian OS; %S 0x%08x-0x%08x bluewhale_platform_%S");
#endif

_LIT8(KPlatformKey,"microedition.platform");

_LIT8(KprotocolpathKey ,"javax.microedition.io.Connector.protocolpath");
_LIT8(KprotocolpathValue ,"com.sun.midp.io");

_LIT8(KAutoStartKey,"autostart://:");
_LIT8(KAutoStartOnValue,"1");
_LIT8(KAutoStartOffValue,"0");

_LIT8(KInstallFileNameKey,"x-bw-vm-install-filename");

_LIT8(KHostVersion,"Host-Version");

_LIT8(KQuitReasonKey,"x-bw-quit-reason");

_LIT8(KPlatformnameKey,"x-bw-platform-name");
_LIT8(KPlatformname,"BlueWhale");

_LIT8(KAppnameKey,"x-bw-app-name");
_LIT8(KAppFullNameKey, "x-bw-app-full-name");

_LIT8(KDefaultAppName,"BlueWhaleMail");
_LIT8(KDefaultAppFullName,"com.bluewhalesystems.client.midlet.BlueWhaleMail");

const TInt KArgCount = 17;

TFileName CJVMRunner::VMInstallFileName()
{
	TFileName sisFileName;

	_LIT(KBlueWhaleSisReaderExe, "bluewhalesisreader.exe");
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__ || __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	TFileName sisReaderExe(KBlueWhaleSisReaderExe);

	TUidType uidType(KNullUid, KNullUid, KUidSisReaderExe);
	RProcess proc;
	if (proc.Create(sisReaderExe, KNullDesC, uidType) == KErrNone)
	{
		proc.Resume();
		TRequestStatus requestStatus;
		proc.Logon(requestStatus);
		User::WaitForRequest(requestStatus);
		proc.Close();
	}
#elif !defined(__WINSCW__)
	TFileName drive;
	Dll::FileName(drive); // Get the drive letter
	TParsePtrC parse(drive);
	TFileName sisReaderExe(parse.Drive());

	sisReaderExe.Append(_L("\\system\\apps\\BlueWhalePlatform\\"));
	sisReaderExe.Append(KBlueWhaleSisReaderExe);

	EikDll::StartExeL(sisReaderExe);

	TFileName matchName(_L("BlueWhaleSisReader*"));
	TFindProcess finder(matchName);
	TFileName result;
	if (finder.Next(result) == KErrNone)
	{
		RProcess proc;
		if (proc.Open(finder) == KErrNone)
		{
			TRequestStatus requestStatus;
			proc.Logon(requestStatus);
			User::WaitForRequest(requestStatus);
			proc.Close();
		}
	}
#endif

	TInt err = RProperty::Get(KUidSisReaderExe, KUidSisFileName.iUid, sisFileName);

	return sisFileName;
}

TBuf8<128> CJVMRunner::QuitReasonText()
{
	TBuf8<128> result;
	TFileName quitReasonFileName = CVMManager::FullPath(KQuitReasonFile);
	
	RFs fs;
	if (fs.Connect() == KErrNone)
	{
		RFile quitReasonFile;
		TInt err = quitReasonFile.Open(fs, quitReasonFileName, EFileRead);
		if (err == KErrNone)
		{
			quitReasonFile.Read(result, result.MaxLength() - 1);
			quitReasonFile.Close();
		}
		TInt ignore = fs.Delete(quitReasonFileName);
		fs.Close();
	}
	return result;
}

TBuf8<32> CJVMRunner::ManufacturerName(TInt aManufacturer)
{
	TBuf8<32> result;
	switch (aManufacturer)
	{
		case HALData::EManufacturer_Ericsson:
			result = KManufacturerEricsson;
			break;
		case HALData::EManufacturer_Motorola:
			result = KManufacturerMotorola;
			break;
		case HALData::EManufacturer_Nokia:
			result = KManufacturerNokia;
			break;
		case HALData::EManufacturer_Panasonic:
			result = KManufacturerPanasonic;
			break;
		case HALData::EManufacturer_Psion:
			result = KManufacturerPsion;
			break;
		case HALData::EManufacturer_Intel:
			result = KManufacturerIntel;
			break;
		case HALData::EManufacturer_Cogent:
			result = KManufacturerCogent;
			break;
		case HALData::EManufacturer_Cirrus:
			result = KManufacturerCirrus;
			break;
		case HALData::EManufacturer_Linkup:
			result = KManufacturerLinkup;
			break;
		case HALData::EManufacturer_TexasInstruments:
			result = KManufacturerTexasInstruments;
			break;
		case KUidValueManufacturer_SonyEricsson:
			result = KManufacturerSonyEricsson;
			break;
		case KUidValueManufacturer_ARM:
			result = KManufacturerARM;
			break;
		case KUidValueManufacturer_Samsung:
			result = KManufacturerSamsung;
			break;
		case KUidValueManufacturer_Siemens:
			result = KManufacturerSiemens;
			break;
		case KUidValueManufacturer_Sendo:
			result = KManufacturerSendo;
			break;
		case KUidValueManufacturer_BENQ:
			result = KManufacturerBENQ;
			break;
		case KUidValueManufacturer_LG:
			result = KManufacturerLG;
			break;
		case KUidValueManufacturer_Lenovo:
			result = KManufacturerLenovo;
			break;
		case KUidValueManufacturer_NEC:
			result = KManufacturerNEC;
			break;
		default:
			result = KManufacturerUnknown;
			break;
	}
	return result;
}

void CJVMRunner::EnsureAutoStartController()
{
	TFileName matchName(_L("BlueWhaleStarter"));
	matchName.Append(_L("*"));

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__) || (!defined __WINSCW__)
	TFindProcess finder(matchName);
#else
	TFindThread finder(matchName);
#endif

	TFileName result;
	if (finder.Next(result) != KErrNone)
	{
		TUidType uidType(KNullUid, KNullUid, KUidStarterExe);

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		RProcess proc;
		TInt err = proc.Create(KBlueWhaleStarterExe, KNullDesC, uidType);
#elif defined __WINSCW__
		RThread proc;
		RLibrary lib;
		TInt err = lib.Load(_L("bluewhalestarter.app"), _L("z:\\system\\apps\\BlueWhaleStarter\\"), uidType);
		if (err == KErrNone)
		{
			TThreadFunction threadFunction = (TThreadFunction)lib.Lookup(1);
			err = proc.Create(_L("BlueWhaleStarter"), threadFunction, KDefaultStackSize, NULL, &lib, NULL, 0x400, 0x100000, EOwnerProcess);
			lib.Close();
		}
#else
		RProcess proc;
		TFileName drive;
		Dll::FileName(drive); // Get the drive letter
		TParsePtrC parse(drive);

		TFileName starterFileName(parse.Drive());
		starterFileName.Append(_L("\\system\\apps\\BlueWhalePlatform\\"));
		starterFileName.Append(KBlueWhaleStarterExe);
		TInt err = proc.Create(starterFileName, KNullDesC, uidType);
#endif

		if (err == KErrNone)
		{
			TRequestStatus status;
			proc.Rendezvous(status);
			if (status == KRequestPending)
			{
				proc.Resume();
				User::WaitForRequest(status);
			}
			proc.Close();
		}
	}
}

TInt CJVMRunner::RunVML()
{
	DEBUGMESSAGE(_L("Starting VM"));
	TBuf8<128> mainClass;
	TBuf8<160> userAgent;
	CVMProperties* properties = CVMProperties::NewLC();
	CVMArguments* arguments = CVMArguments::NewLC();
	mainClass =  KAMSRunner;

	TInt uid = 0;
	TInt manufacturer = 0;
	HAL::Get(HALData::EMachineUid, uid);
	HAL::Get(HALData::EManufacturer, manufacturer);
	TBuf8<32> manufacturerName(ManufacturerName(manufacturer));
	userAgent.Format(KUserAgentBase, &BLUEWHALEPLATFORM_SOFTWARE_BUILD_VERSION(), &manufacturerName, manufacturer, uid, &BLUEWHALEPLATFORM_SOFTWARE_BUILD_VERSION());

	properties->AddL(KUserAgentKey(),userAgent);
	properties->AddL(KPlatformKey(),userAgent);
	properties->AddL(KprotocolpathKey(),KprotocolpathValue);

			
	EnsureAutoStartController();

	TInt val = 0;
	TInt ignore = RProperty::Get(KUidStarterExe, KUidAutoStart.iUid, val);
	if(val != 0)
	{
		properties->AddL(KAutoStartKey(),KAutoStartOnValue());
	}
	else
	{
		properties->AddL(KAutoStartKey(),KAutoStartOffValue());
	}
	
	properties->AddL(KHostVersion(),userAgent);
		
	TBuf8<KMaxFileName + 1> installFileName;
	installFileName.Copy(VMInstallFileName());
	
	properties->AddL(KInstallFileNameKey(),installFileName);
		
	properties->AddL(KQuitReasonKey(),QuitReasonText());
		
	properties->AddL(KPlatformnameKey(),KPlatformname());
	
	properties->AddL(KAppFullNameKey(), KDefaultAppFullName());
	if (iShortcutName.Length() == 0)
	{
		properties->AddL(KAppnameKey(), KDefaultAppName());
	}
	else
	{
		properties->AddL(KAppnameKey(), iShortcutName);
	}

#ifdef __WINSCW__
	_LIT8(KPrintIsolateMemoryUsage,"+PrintIsolateMemoryUsage");
	arguments->AddL(KPrintIsolateMemoryUsage());
#endif

#ifdef __BYTECODETRACE__
	_LIT8(KTraceBytecodesStart,"=TraceBytecodesStart4000000");
    _LIT8(KTraceBytecodes,"+TraceBytecodes");
    _LIT8(KTraceBytecodesStop,"=TraceBytecodesStop5000000");
    arguments->AddL(KTraceBytecodesStart());
    arguments->AddL(KTraceBytecodes());
    arguments->AddL(KTraceBytecodesStop());
#endif
	TBool nothingInstalled = ETrue;
	RFs fs;
	if (fs.Connect() == KErrNone)
	{
		CleanupClosePushL(fs);
		TFindFile findFile(fs);

		TBuf<2> drive;
#ifdef __WINSCW__
		drive = _L("c:"); 
#else
		TParsePtrC parse(RProcess().FileName());
		drive = parse.Drive();
#endif
		_LIT(KLitJarsPath, "%S\\private\\%08x\\app\\");
		TFileName pathToJars;
		pathToJars.Format(KLitJarsPath, &drive, KUidBlueWhalePlatformApp);
		CDir* dir = NULL;
		if (findFile.FindWildByPath(_L("*.jar"), &pathToJars, dir) == KErrNone)
		{
			delete dir;
			nothingInstalled = EFalse;
		}
		CleanupStack::PopAndDestroy(&fs);
	}
    
	const char* launcher = "com.bluewhalesystems.midp.Launcher";
	const char* browser = "com.sun.midp.appmanager.MVMManager";
	const char* midlet = browser;
	if (iShortcutName.Length() || nothingInstalled || iAutoStarted)
	{
		midlet = launcher;
	}
	
	TInt ret = RunVMCode(NULL,(char*)mainClass.PtrZ(),*properties,*arguments,iApplication,midlet);
	DEBUGMESSAGE1(_L("VM exited %d"),ret);
	CleanupStack::PopAndDestroy(arguments);
	CleanupStack::PopAndDestroy(properties);
	return ret;
}

void CJVMRunner::RunL()
{
	TInt ret = RunVML();
	if(iThread)
	{
		iThread->RunnableComplete(this,ret);
	}
}

void CJVMRunner::DoCancel()
{}

TInt CJVMRunner::RunError(TInt aError)
{
	if(iThread)
	{
		iThread->RunnableComplete(this,aError);
	}
	return KErrNone;
}

void CJVMRunner::StartL(RThread& aThread)
{
	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::After(10000);
	aThread.RequestComplete(status,KErrNone);
}

void CJVMRunner::StopL()
{}

