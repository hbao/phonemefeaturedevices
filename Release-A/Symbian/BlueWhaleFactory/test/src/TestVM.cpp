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

 
#include <e32std.h>
//#include "EcomPlus.h"
#include "application.h"
#include "PlatformVMTimerStateMachine.h"
#include "ManagementObjectFactory.h"
#include "TestVM.h"

void CTestVM::setUp()
{}

void CTestVM::tearDown()
{}

MUnknown * CTestVM::CreateL(TUid /*aImplementationUid*/, TUid aInterfaceUid, TAny* /*aConstructionParameters*/)
{
	RDebug::Print(_L("Create 0x%08x"),aInterfaceUid);
	return NULL;
}

const MStateMachine::TCommand KDuffCommand = {0x12345678, 0xff };

void CTestVM::testRunVMWithInWrongState()
{
	RDebug::Print(_L("testRunVMWithInWrongState"));
	MStateMachine* stateM = static_cast<MStateMachine*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MVMTimerStateMachine), TUid::Uid(KIID_MStateMachine),NULL));
	CleanupReleasePushL(*stateM);
	RDebug::Print(_L("Accepting incorrect command"));
	TS_ASSERT_THROWS(stateM->AcceptCommandL(KDuffCommand,NULL),KErrArgument);
	CleanupStack::PopAndDestroy(stateM);
}

void CTestVM::testOpenCommandIncorrectState()
{
	RDebug::Print(_L("testOpenCommandIncorrectState"));
	MStateMachine* stateM = static_cast<MStateMachine*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MVMTimerStateMachine), TUid::Uid(KIID_MStateMachine),NULL));
	CleanupReleasePushL(*stateM);
	RDebug::Print(_L("Accepting KCommandOpen"));
	stateM->AcceptCommandL(KCommandOpen,NULL);
	TS_ASSERT_THROWS(stateM->AcceptCommandL(KCommandOpen,NULL),KErrCommandNotExpected);
	CleanupStack::PopAndDestroy(stateM);
}

class CActiveRunner : public CActive
{
public:
	CActiveRunner(CTestVM* aParent) : CActive(0),iParent(aParent)
	{
		CActiveScheduler::Add(this);
	}
	void Start()
	{
		iStatus = KRequestPending;
		TRequestStatus* status = &iStatus;
		SetActive();
		User::RequestComplete(status,KErrNone);
	}
	void DoCancel()
	{}
	
	void RunL()
	{
		MStateMachine* stateM = static_cast<MStateMachine*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MVMTimerStateMachine), TUid::Uid(KIID_MStateMachine),NULL));
		CleanupReleasePushL(*stateM);
		MVMObjectFactoryClient* client = QiL(stateM,MVMObjectFactoryClient);
		CleanupReleasePushL(*client);
		MVMObjectFactory* factory = DiL(MVMObjectFactory);
		client->SetClient(factory);
		CleanupStack::PopAndDestroy(client);

		RDebug::Print(_L("Accepting KCommandOpen"));
		stateM->AcceptCommandL(KCommandOpen,NULL);
	}
	
	TInt RunError(TInt /* aError */)
	{
		return KErrNone;
	}
	CTestVM* iParent;
};

void CTestVM::testSetObjectClient()
{
	MVMObjectFactory * factory = NULL;
	MStateMachine* stateM = static_cast<MStateMachine*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MVMTimerStateMachine), TUid::Uid(KIID_MStateMachine),NULL));
	CleanupReleasePushL(*stateM);
	MVMObjectFactoryClient* client = QiL(stateM,MVMObjectFactoryClient);
	CleanupReleasePushL(*client);
	client->SetClient(factory);
	CleanupStack::PopAndDestroy(client);
	CleanupStack::PopAndDestroy(stateM);

}
void CTestVM::testStartVM()
{
	RDebug::Print(_L("testOpenCommandIncorrectState"));
	CActiveRunner runner(this);
//	runner.Start();
//	CActiveScheduler::Start();
//	TS_ASSERT(status != KErrNone);
//	CleanupStack::PopAndDestroy(stateM);
//	CleanupStack::PopAndDestroy(unknown);
}
