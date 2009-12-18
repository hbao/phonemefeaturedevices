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
#include "TestOOMTest.h"
#include "EcomPlus.h"
#include "PropertiesImpl.h"
//#include "LoggerImpl.h"
#include "DataAccessConnectionImpl.h"
#include "SocketEngineImpl.h"
#include "TimeOutTimerImpl.h"
#include "PlatformVMTimerStateMachine.h"
#include "Thread.h"
#include "BaseMIDPView.h"
#include "NetworkInfo.h"
#include "IAPInfo.h"

void CTestOOMTest::setUp()
{}

void CTestOOMTest::tearDown()
{}

void CTestOOMTest::testObjectCreate()
{
#if 0
	DoOOMObjectCreate(KCID_MProperties);
	//DoOOMObjectCreate(KCID_MLogger);
	DoOOMObjectCreate(KCID_MMessagingDataAccessConnection);
	DoOOMObjectCreate(KCID_MSocketEngine);
	DoOOMObjectCreate(KCID_MTimeOutTimer);
	DoOOMObjectCreate(KCID_MVMTimerStateMachine);
	
	MProperties* props = DiL(MProperties);
	CleanupReleasePushL(*props);
	props->SetStringL(KPropertyThreadNameString,_L("TEST"));
	DoOOMObjectCreate(KCID_MThread,props);
	CleanupStack::PopAndDestroy(props);
	
	DoOOMObjectCreate(KCID_MVMObjectFactory);
	//DoOOMObjectCreate(KCID_MBaseMIDPView);
	//DoOOMObjectCreate(KCID_MTelephonyWrapper);
	DoOOMObjectCreate(KCID_MConnectionMonitorWrapper);
	DoOOMObjectCreate(KCID_MCommDBWrapper);
#endif
}

void CTestOOMTest::DoOOMObjectCreate(TInt aObjectId,TAny * aConstructionParameters )
{
	MUnknown * unknown = NULL;
	__UHEAP_MARK;
	TInt err = KErrNone;
	for(TInt k=1;;k++)
	{
		RDebug::Print(_L("checking level %d"),k);
		__UHEAP_SETFAIL(RHeap::EDeterministic,k);
		__UHEAP_MARK;
		TRAP(err,unknown = REComPlusSession::CreateImplementationL(TUid::Uid(aObjectId), TUid::Uid(KIID_MUnknown), aConstructionParameters));
		if(err == KErrNone)
		{
			unknown->Release();
		}
		__UHEAP_MARKEND;
		User::Heap().Check();
		if(err == KErrNone)
		{
			break;
		}
	}
	__UHEAP_MARKEND;
	__UHEAP_RESET;
	User::Heap().Check();
}
