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
#ifdef _DEBUGTERMINAL_

#ifndef __DEBUGTERMINAL_H__
#define __DEBUGTERMINAL_H__

#include <EcomPlusRefCountedBase.h>
#include "TimerStateMachine.h"
#include "Connection.h"
#include "WritableConnection.h"
#include "Thread.h"

class CDebugTerminal;

class MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal) = 0;
	virtual void WriteHelpText(CDebugTerminal* aTerminal) = 0;
	virtual ~MCommand(){}
};

class CVersion : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};

class CKill : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};

class CResource : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};

class CKey : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};

class CCommand : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};

class CNetworkFail : public CBase, public MCommand
{
public:
	virtual TBool AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal);
	virtual void WriteHelpText(CDebugTerminal* aTerminal);
private:
};


class CDebugTerminal : public CEComPlusRefCountedBase, public MConnectionCallback, public MRunnable
{
public:
	CDebugTerminal(TAny * aConstructionParameters,MTimerStateMachine* aParent);

	// MUnknown
	MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef()  {CEComPlusRefCountedBase::AddRef();}
	void Release() {CEComPlusRefCountedBase::Release();}
	
	// MStateMachineCallback
	void CallbackCommandL( MStateMachine::TCommand aCommand );
	void ReportStateChanged( TInt aComponentId, MStateMachine::TState aState );

	// MConnectionCallback
	void ReportError(TErrorType aErrorType, TInt aErrorCode);

	// MRunnable
	virtual void ConstructL();
	virtual void StartL(RThread& aThread);
	virtual void StopL();
	virtual void SetThread(MThread* aThread);

	
	// for command objects
	MWritableConnection* Connection() { return iConnection;}
	MTimerStateMachine* Parent() { return iParent;}
protected:
	virtual ~CDebugTerminal();
private:
	MProperties* iProperties;
	MWritableConnection* iConnection;
	HBufC8* iCommand;
	MTimerStateMachine* iParent;
	RPointerArray<MCommand> iCommands;
};

#endif /*__DEBUGTERMINAL_H__*/
#endif // _DEBUGTERMINAL_