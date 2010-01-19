/*
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

#include <eikenv.h>
#include "BlueWhaleRecog.h"
#include "Application.h"
#include <apgcli.h>
#include <apgtask.h>

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <ImplementationProxy.h>
#include <hal.h>
#include <hal_data.h>
#else
#include <apacmdln.h>
#include <apparc.h>
#include <eikdll.h>
#include <w32std.h>
#endif

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
const TInt KBlueWhaleRecognizerValue=0x2000E27D;
const TInt KImplementationUid = 0x2000E27E;
#else
const TInt KBlueWhaleRecognizerValue=0xA0003F4F;
#endif
const TUid KUidBlueWhaleRecognizer = {KBlueWhaleRecognizerValue};

_LIT(KBlueWhaleRecogPanic, "BlueWhaleRecog");
_LIT(KBootUpThreadName, "BlueWhaleRecogThr");

const TInt KMaxBufferLength=256;

enum TBlueWhaleRecogPanic
{
	EDInvalidIndex
};

GLDEF_C void Panic(TBlueWhaleRecogPanic aPanic)
{
	User::Panic(KBlueWhaleRecogPanic, aPanic);
}

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(KImplementationUid, CBlueWhaleRecognizer::CreateRecognizerL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
}
#endif

CBlueWhaleRecognizer::CBlueWhaleRecognizer()
	:CApaDataRecognizerType(KUidBlueWhaleRecognizer, CApaDataRecognizerType::ENormal)
{
	iCountDataTypes = 0;
}

TUint CBlueWhaleRecognizer::PreferredBufSize()
{
	return KMaxBufferLength;
}

TDataType CBlueWhaleRecognizer::SupportedDataTypeL(TInt aIndex) const
{
	__ASSERT_DEBUG(aIndex == 0, Panic(EDInvalidIndex));
	return TDataType(_L8(""));
}

void CBlueWhaleRecognizer::DoRecognizeL(const TDesC&, const TDesC8&)
{
}

TInt CBlueWhaleRecognizer::StartExe(TAny* /*aParam*/)
{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
	{
		TRAP(err, CBlueWhaleRecognizer::StartExeL());
		delete cleanup;
	}
	return err;
}

void CBlueWhaleRecognizer::StartExeL()
{
	TFullName res;
	TFindProcess find;
	while (find.Next(res) == KErrNone)
	{
		RProcess process;
		if (process.Open(find) == KErrNone)
		{
			if (process.Type().MostDerived() == KUidStarterExe)
			{
				process.Close();
				return;
			}
		process.Close();
		}
	}

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__

	const TInt KUidManufacturerSonyEricssonValue = 0x101f6ced;
	TInt manufacturer = -1;
	HAL::Get(HALData::EManufacturer, manufacturer);
	if ((manufacturer != HALData::EManufacturer_Ericsson) && (manufacturer != KUidManufacturerSonyEricssonValue))
	{
		CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
		cmdLine->SetExecutableNameL(_L("\\sys\\bin\\BlueWhaleStarter.exe"));	
		cmdLine->SetCommandL(EApaCommandRun);
	
		RApaLsSession lsSession;
		User::LeaveIfError(lsSession.Connect());
		lsSession.StartApp(*cmdLine);
		lsSession.Close();
		CleanupStack::PopAndDestroy(cmdLine);
	}
	
#else

	TFileName starterFileName;
#ifdef __WINSCW__
	starterFileName.Append(_L("z:\\system\\apps\\BlueWhaleStarter\\BlueWhaleStarter.app"));
#else	  
	TFileName drive;
	Dll::FileName(drive); // Get the drive letter
	TParsePtrC parse(drive);

	starterFileName.Append(parse.Drive());
	starterFileName.Append(_L("\\system\\apps\\BlueWhalePlatform\\BlueWhaleStarter.exe"));
#endif
	EikDll::StartExeL(starterFileName);
	
#endif
}

void CBlueWhaleRecognizer::LaunchBlueWhaleStarter()
{
	RThread* bootThread = new RThread();
	if (bootThread)
	{
		TInt res = KErrNone;
		res = bootThread->Create(	KBootUpThreadName,
									CBlueWhaleRecognizer::StartExe,
									KDefaultStackSize,
									KMinHeapSize,
									KMinHeapSize,
									NULL,
									EOwnerThread);
	       
		if (res == KErrNone)
		{
			bootThread->Resume();
			bootThread->Close();
		}
		else
		{
			delete bootThread;
		}
	}
}	

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__

CApaDataRecognizerType* CBlueWhaleRecognizer::CreateRecognizerL()
{
	CBlueWhaleRecognizer* recognizer = new (ELeave) CBlueWhaleRecognizer();
	recognizer->LaunchBlueWhaleStarter();
	return recognizer;
}

#else

EXPORT_C CApaDataRecognizerType* CreateRecognizer()
// The gate function - ordinal 1
//
{
	CBlueWhaleRecognizer* recognizer = new CBlueWhaleRecognizer();
	if (recognizer)
	{
		recognizer->LaunchBlueWhaleStarter();
	}
	return recognizer;
}

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//
// DLL entry point
//
{
	return KErrNone;
}

#endif

