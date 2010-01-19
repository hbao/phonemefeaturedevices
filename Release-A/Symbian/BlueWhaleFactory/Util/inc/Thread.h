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


#ifndef __THREAD_CPP__
#define __THREAD_CPP__

#include <unknown.h>
#include "Properties.h"

class MThread;

class MRunnable
{
	public:
		virtual void ConstructL() = 0;
		virtual void StartL(RThread& aThread) = 0;
		virtual void StopL() = 0;
		virtual void SetThread(MThread* aThread) = 0;
	protected:
		virtual ~MRunnable(){}
};

class MThread : public MUnknown
{
	public:
		virtual TInt ThreadMainL() = 0;
		virtual void StartL() =0;
		virtual void StopL() = 0;
		virtual void Kill(TInt aReason) = 0;
		virtual void AddL(MRunnable* aRunnable) =0;
		virtual RThread& Thread() = 0;
		virtual CActiveScheduler* Scheduler() = 0;

		virtual void RunnableComplete(MRunnable* aRunnable,TInt aStatus) = 0;
		virtual TInt CompleteStatus() = 0;
	protected:
	 virtual ~MThread(){}
};

const TInt KCID_MThread = 0xA0003F51;
const TInt KIID_MThread = 0xA0003F52;
const TInt KIID_MRunnable = 0xA0003F57;

const TStringUniqueKey KPropertyThreadNameString = {KCID_MThread, 0x00000001};

class CThread : public CBase, public MThread
{
	public:
		static MUnknown * NewL( TAny * aConstructionParameters );

		void StartL();
		void StopL();
		void AddL(MRunnable* aRunnable);
		void Kill(TInt aReason);
		RThread& Thread(){return iThread;}
		CActiveScheduler* Scheduler(){return iScheduler;}
		void RunnableComplete(MRunnable* aRunnable,TInt aStatus);
		TInt CompleteStatus() {return iCompleteStatus;}
		virtual MUnknown * QueryInterfaceL( TInt aInterfaceId )
		{
			if( KIID_MUnknown == aInterfaceId )
			{
				AddRef(); // Remember to do this in your subclasses for every QI.
				return static_cast<MUnknown*>( this );
			}
			else if(KIID_MThread == aInterfaceId)
			{
				AddRef();
				return static_cast<MThread*>( this );
			}
			else
			{
				User::Leave(KErrNotSupported);
				return 0;
			}
		}

		virtual void AddRef()
		{
			++iRefCount;
		}

		virtual void Release()
		{
			--iRefCount;
			if( 0 == iRefCount )
			{
				delete this;
			}
		}

	protected:
		~CThread();
		RPointerArray<MRunnable>& Runnables(){return iRunnables;}
		RSemaphore& StartupSignal() { return iStartupSignal;}
	private:
		CThread();
		void ConstructL(TAny * aConstructionParameters);
		static TInt ThreadMain(TAny* aThis);
		TInt ThreadMainL();
		TInt iRefCount;
	public:
		class CThreadStopper : public CActive
		{
			public:
				CThreadStopper(CThread* aParent);
				void Stop();
			private:
				void RunL();
				void DoCancel();
				TInt RunError(TInt aError);	
			private:
				CThread* iParent;
		};
	friend class CThread::CThreadStopper;
	
	private:
		RThread iThread;
		RSemaphore iStartupSignal;
		CActiveScheduler* iScheduler;
		RPointerArray<MRunnable> iRunnables;
		CThreadStopper* iStopper;
		TInt iCompleteStatus;
};

#endif // __THREAD_CPP__
