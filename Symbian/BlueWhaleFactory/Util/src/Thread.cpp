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
#include "ecomplus.h"
#include "Properties.h"
#include "logger.h"
#include "Thread.h"
#include "DebugUtilities.h"
#include <OSVersion.h>
#include <fbs.h>
#include <coemain.h>
#include "Application.h"

//#define __DEBUGTHREAD__

#ifdef __DEBUGTHREAD__
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

const TInt KStackSize = 0x8000; // 32K

#ifdef __WINS__

#include <reent.h>

extern "C" {
	void _DisposeAllThreadData();
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

MUnknown * CThread::NewL( TAny * aConstructionParameters )
{
	CThread * self = new (ELeave) CThread();
	CleanupStack::PushL(self);
	self->ConstructL(aConstructionParameters);
	
	MThread * selfInterface = QiL( self, MThread );
	
	CleanupStack::Pop(self);
	return selfInterface;
}

CThread::CThread()
{}

CThread::~CThread()
{
	iStartupSignal.Close();
	iRunnables.Reset();
	iThread.Close();
}

void CThread::StartL()
{
	iThread.Resume();
	iStartupSignal.Wait();
}

void CThread::StopL()
{
	iStopper->Stop();
}

void CThread::Kill(TInt aReason)
{
	iThread.Kill(aReason);
}

void CThread::AddL(MRunnable* aRunnable)
{
	if(aRunnable)
	{
		aRunnable->SetThread(this);
		aRunnable->ConstructL();
		iRunnables.AppendL(aRunnable);
	}
}

void CThread::RunnableComplete(MRunnable* aRunnable,TInt aStatus)
{
	DEBUGMESSAGE2(_L("CThread::RunnableComplete 0x%08x %d"),aRunnable,aStatus);
	iCompleteStatus = aStatus;
	TInt location = iRunnables.Find(aRunnable);
	if(location >= 0)
	{
		iRunnables.Remove(location);
		if(iRunnables.Count() == 0)
		{
			//Kill(aStatus);
			StopL();
		}
	}
}

void CThread::ConstructL(TAny * aConstructionParameters)
{
	MProperties* props = QiL(static_cast<MUnknown*>(aConstructionParameters),MProperties);
	CleanupReleasePushL(*props);
	const TDesC & name(props->GetStringL(KPropertyThreadNameString,0));
	DEBUGMESSAGE1(_L("CThread::Construct %S"),&name);
	User::LeaveIfError(iThread.Create(name,CThread::ThreadMain,KStackSize,NULL,this,EOwnerProcess));
	CleanupStack::PopAndDestroy(props);
	User::LeaveIfError(iStartupSignal.CreateLocal(0));
}

TInt CThread::ThreadMain(TAny* aThis)
{
	TInt ret;
	CThread* This = static_cast<CThread*>(aThis);
    CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(ignore,ret = This->ThreadMainL());
    delete cleanup;
    return ret;
}

TInt CThread::ThreadMainL()
{
	TInt error = KErrNone;
	User::LeaveIfError(RFbsSession::Connect());
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iStopper = new (ELeave) CThread::CThreadStopper(this);
	DEBUGMESSAGE1(_L("CThread::ThreadMain starting thread func %d"),(TUint)iThread.Id());
	TInt count = iRunnables.Count();
	for(TInt i=0;i<count;i++)
	{
		MRunnable* runnable = iRunnables[i];
		TRAP(error,runnable->StartL(iThread));
		if(error != KErrNone)
		{
			break;
		}
	}
	DEBUGMESSAGE1(_L("CThread::ThreadMain startup complete %d"),error);
	iStartupSignal.Signal();
	if(error == KErrNone)
	{
		iScheduler->Start();
	}

	DEBUGMESSAGE2(_L("CThread::ThreadMain Exiting thread func %d error %d"),(TUint)iThread.Id(),error);
	iStartupSignal.Signal();
	delete iScheduler;
	delete iStopper;
	RFbsSession::Disconnect();
	return error;
}

CThread::CThreadStopper::CThreadStopper(CThread* aParent) :
	CActive(CActive::EPriorityHigh),iParent(aParent)
{
	iParent->Scheduler()->Add(this);
}

void CThread::CThreadStopper::Stop()
{
	if(RThread().Id() == iParent->Thread().Id())
	{
		RunL();
	}
	else
	{
		TExitType type = iParent->Thread().ExitType();
		if(type == EExitPending) // the thread is still running
		{
			iStatus = KRequestPending;
			TRequestStatus* status = &iStatus;
			iParent->Thread().RequestComplete(status,KErrNone);
			SetActive();
			iParent->StartupSignal().Wait();
		}
	}
}

void CThread::CThreadStopper::RunL()
{
	TInt error = KErrNone;
	RPointerArray<MRunnable>& runnables = iParent->Runnables();
	TInt count = runnables.Count();
	for(TInt i=0;i<count;i++)
	{
			TRAP(error,(runnables[i])->StopL());
	}
	iParent->Scheduler()->Stop();
}

void CThread::CThreadStopper::DoCancel()
{}

TInt CThread::CThreadStopper::RunError(TInt /*aError*/)
{
	return KErrNone;
}

void CMyUndertaker::RunL()
{
	TFileName quitReasonFileName = CVMManager::FullPath(KQuitReasonFile);

	RFile quitReasonFile;
	TInt err = quitReasonFile.Open(CCoeEnv::Static()->FsSession(), quitReasonFileName, EFileWrite);
	if (err == KErrNone)
	{
		TInt pos = 0;
		err = quitReasonFile.Seek(ESeekEnd, pos);
	}
	else
	{
		err = quitReasonFile.Create(CCoeEnv::Static()->FsSession(), quitReasonFileName, EFileWrite);
	}
	
	if (err == KErrNone)
	{
		TBuf<128> text;
		TExitCategoryName category(iWatchee->ExitCategory());
		if (!category.Length())
		{
			text.Format(_L("%S %d;"), &iWatchee->Name(), iWatchee->ExitReason());
		}
		else
		{
			text.Format(_L("%S %S %d;"), &iWatchee->Name(), &category, iWatchee->ExitReason());
		}
		TBuf8<128> text8;
		text8.Copy(text);
		quitReasonFile.Write(text8);
		quitReasonFile.Close();
	}
	iSupervisor->ThreadTerminated(iWatchee->Id(),iWatchee->ExitReason());
}

		