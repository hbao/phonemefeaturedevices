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
#include "DirectoryList.h"
#include "ecomplus.h"
#include "debugutilities.h"
#include "Properties.h"
#include "logger.h"

//#define __DEBUGDIRLIST__

#ifdef __DEBUGDIRLIST__
#define	DEBUGMSG(_XX) RDebug::Print(_XX)
#define	DEBUGMSG1(_XX,_AA) RDebug::Print(_XX,_AA)
#else
#define	DEBUGMSG(_XX)
#define	DEBUGMSG1(_XX,_AA)
#endif

CDirectoryLister::CDirectoryLister(RThread& aThread) 
	: CActive(EPriorityStandard),iThread(aThread)
{
	CActiveScheduler::Add(this);
}
void CDirectoryLister::ConstructL()
{
	User::LeaveIfError(iSignal.CreateLocal(0));
}
CDirectoryLister::~CDirectoryLister()
{
	Cancel();
	delete iPath;
	iSignal.Close();
}
	
CDir* CDirectoryLister::GetEntries(const TDesC& aPath)
{
	delete iPath;
	iPath = NULL;
	if(aPath[aPath.Length()-1] == '\\')
	{
		// get the whole directory
		iPath = aPath.Alloc();
	}
	else
	{
		// partial search
		_LIT(KWildcard,"*.*");
		iPath = HBufC::New(aPath.Length() + KWildcard().Length());
		iPath->Des().Copy(aPath);
		iPath->Des().Append(KWildcard());
	}
	
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	SetActive();
	iThread.RequestComplete(status,KErrNone);
	iSignal.Wait();
	return iDirs;
}

void CDirectoryLister::SignalSemaphore(TAny* aPtr)
{
	static_cast<RSemaphore*>(aPtr)->Signal();
}

void CDirectoryLister::RunL()
{
	DEBUGMSG1(_L("CDirectoryLister::RunL %S"),&(iPath->Des()));
	TCleanupItem cleanupItem(SignalSemaphore, &iSignal);
	CleanupStack::PushL(cleanupItem);
	RFs fs;
	iDirs = NULL;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.GetDir(*iPath,KEntryAttNormal | KEntryAttDir,ESortByName,iDirs));
	delete iPath;
	iPath = NULL;
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy();	// cleanupItem
}
TInt CDirectoryLister::RunError(TInt /*aError*/)
{
	return KErrNone;
}
void CDirectoryLister::DoCancel()
{}
