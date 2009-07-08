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


#ifndef __THREADUTILS_H__
#define __THREADUTILS_H__
#include <e32msgqueue.h>
#include <flogger.h>

typedef void(*CallbackFunc)(TAny*);
const TInt KMaxQueueSize = 10;

class CThreadRunner : public CActive
{
	public:
		CThreadRunner() : CActive(CActive::EPriorityStandard)
		{
			CActiveScheduler::Add(this);
		}
		virtual ~CThreadRunner()
		{
			Cancel();
			iRunQueue.Close();
			iSync.Close();
			iServerThread.Close();
		}
		// called from owner thread
		void ConstructL()
		{
			User::LeaveIfError(iServerThread.Open(RThread().Id()));
			User::LeaveIfError(iRunQueue.CreateLocal(KMaxQueueSize));
			User::LeaveIfError(iSync.CreateLocal(0));
			iRunQueue.NotifyDataAvailable(iStatus);
			SetActive();
		}

		// called from client thread
		TInt DoCallback(CallbackFunc aCallback,TAny* aData)
		{
			TRunnableData callbackData;
			callbackData.iCallback = aCallback;
			callbackData.iData = aData;
			callbackData.iSyncSignal = NULL;
			iRunQueue.SendBlocking(callbackData);
			return KErrNone;
		}
		TInt DoSyncCallback(CallbackFunc aCallback,TAny* aData)
		{
			TRunnableData callbackData;
			callbackData.iCallback = aCallback;
			callbackData.iData = aData;
			callbackData.iSyncSignal = &iSync;
			iRunQueue.SendBlocking(callbackData);
			iSync.Wait();
			return KErrNone;
		}
		TInt DoCallbackNoBlock(CallbackFunc aCallback,TAny* aData)
		{
			TRunnableData callbackData;
			callbackData.iCallback = aCallback;
			callbackData.iData = aData;
			callbackData.iSyncSignal = NULL;						
			return iRunQueue.Send(callbackData);
		}

	private:
		struct TRunnableData
		{
			CallbackFunc iCallback;
			TAny* iData;
			RSemaphore* iSyncSignal;
		};
		void RunL()
		{
			TRunnableData callbackData;
			User::LeaveIfError(iRunQueue.Receive(callbackData));
			
			(*callbackData.iCallback)(callbackData.iData);
			
			iRunQueue.NotifyDataAvailable(iStatus);
			SetActive();
			if(callbackData.iSyncSignal)
			{
				callbackData.iSyncSignal->Signal();
			}
		}
		void DoCancel()
		{
			iRunQueue.CancelDataAvailable();
		}

		TInt RunError(TInt aError)
		{
			return aError;
		}
	RThread iServerThread;
	RMsgQueue<TRunnableData> iRunQueue;
	RSemaphore iSync;
};

class MThreadSupervisor
{
public:
	virtual void ThreadTerminated(TThreadId aId,TInt aReason) = 0;
protected:
	virtual ~MThreadSupervisor(){}
};



class CMyUndertaker : public CActive
{
public:
	CMyUndertaker():  CActive(EPriorityStandard)
		{}
	CMyUndertaker(RThread* aWatchee,MThreadSupervisor* aSupervisor) :  CActive(EPriorityStandard), iWatchee(aWatchee),iSupervisor(aSupervisor)
		{
			CActiveScheduler::Add(this);
		}
	~CMyUndertaker()
	{
		Cancel();
	}
	void StartL()
		{
			iWatchee->Logon(iStatus);
			SetActive();
		}
	void RunL();
	TInt RunError(TInt aError)
		{
		return aError;
		}
	void DoCancel()
	{
		iWatchee->LogonCancel(iStatus);
	}
private:
	MThreadSupervisor* iSupervisor;
	RThread* iWatchee;
	TInt iId;
};

#endif // __THREADUTILS_H__ 
