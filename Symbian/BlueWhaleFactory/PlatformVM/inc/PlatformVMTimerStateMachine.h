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


#ifndef __PLATFORMVMTIMERSTATEMACHINE_H__
#define __PLATFORMVMTIMERSTATEMACHINE_H__

#include <EcomPlusRefCountedBase.h>
#include <os_symbian.hpp>
#include "logger.h"
#include "TimerStateMachine.h"
#include "PlatformVMRunner.h"
#include "ManagementObjectFactory.h"
#include "Application.h"
#include "DebugTerminal.h"


class CMyUndertaker;
class CVMManager;

// might as well have our own error codes
const TInt KErrBWMTimerBaseError = -10000;
const TInt KErrCommandNotExpected = KErrBWMTimerBaseError - 1;
const TInt KErrInvalidState = KErrBWMTimerBaseError - 2;

class CVMTimerStateMachine : public CEComPlusRefCountedBase, public MTimerStateMachine, public MVMObjectFactoryClient, public MThreadSupervisor
{
public:
#ifdef __WINSCW__
	EXPORT_C
#endif
	static MUnknown * NewL( TAny * aConstructionParameters );
    const RThread& Thread();
    CActiveScheduler* Scheduler() const;

public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() {CEComPlusRefCountedBase::AddRef();}
	void Release() {CEComPlusRefCountedBase::Release();}
	

public: // MStateMachine implementation.
	virtual TBool AcceptCommandL(TCommand aCommand, MProperties * aCommandProperties);
	virtual void Reset();

public: // MVMObjectFactoryClient
	void SetClient(MVMObjectFactory* aFactory) {iFactory = aFactory;}
	
protected: // state handling functions
	void DoInitialStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoReadyStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoOnlineStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoOfflineStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoClosingStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoKillingStateL(TCommand aCommand, MProperties * aCommandProperties);
	void DoEndStateL(TCommand aCommand, MProperties * aCommandProperties);
	
public: // MInitialize implementation.
	virtual void InitializeL( MProperties * aInitializationProperties /* IN */);

public: // MTimerStateMachine implementation.
	TBool Online() const;
	TBool Offline() const;

#if _DEBUGTERMINAL_
public: // for debug terminal
	virtual MDebugApplication* Application() { return iApp;}
#endif
	
protected: //MThreadSupervisor
    void ThreadTerminated(TThreadId aId,TInt aReason);
    
private:
	CVMTimerStateMachine(TAny * aConstructionParameters );
	~CVMTimerStateMachine();

	void CreateVML();
	TInt StopRunningThreads(TThreadId aId);
	static TInt DebugTimerFunction(TAny* aSelf);
	static TInt RestartTimerFunction(TAny* aSelf);
		
private:
	MVMObjectFactory* iFactory;
	MThread* iVMThread;
	CJVMRunner* iJVM;
	MThread* iVMManager;
	MDebugApplication* iApp;
	RArray<TThreadInfo> iRunningThreads;
	
	typedef enum TState
	{
		EInitial,
		EReady,
		EOnline,
		EOffline,
		EKilling,
		EClosing,
		EEnd
	};

	TState iState;
	TInt iInstanceCount;
	CPeriodic* iDebugTimer;
	CPeriodic* iRestartTimer;
	MProperties* iProperties;
#if _DEBUGTERMINAL_
	MThread* iDebugThread;
	CDebugTerminal* iDebugTerminal;
#endif
};
#endif /* __PLATFORMVMTIMERSTATEMACHINE_H__ */
