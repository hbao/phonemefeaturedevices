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
#include "TestAPNManagerWithPorts.h"
#include "APNManager.h"
#include "DummyCommDb.h"
#include "TestAPNManagerCommDBWrapper.h"
#include "TestAPNManagerTelephonyWrapper.h"
#include "TestWithPortConnectionMonitorWrapper.h"

_LIT(KNetwork2,"33"); // Orange

static CTestSimpleCommDBWrapper* gCurrentDB = NULL;

MUnknown * CTestAPNManagerWithPorts::Create(TUid aImplementationUid, TUid /*aInterfaceUid*/, TAny* /*aConstructionParameters*/)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MCommDBWrapper:
			gCurrentDB = new (ELeave) CTestSimpleCommDBWrapper;
			gCurrentDB->ConstructL();
			ret = gCurrentDB;
			break;
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CBaseTelephonyWrapper;
			break;
		case KCID_MConnectionMonitorWrapper:
			ret = new (ELeave) CTestConnectionMonitorWrapperWithTimeout(5000000);
			break;
		default:
			User::Invariant();
	}
	return ret;
}

void CTestAPNManagerWithPorts::setUp()
{}
void CTestAPNManagerWithPorts::tearDown()
{
	REComPlusSession::SetDelegate(NULL);
}

void CTestAPNManagerWithPorts::testSucess()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(session);
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);

	for(TInt i=0;i<20;i++)
	{
		TInt iap = session->GetNextIAP(80);
		TS_ASSERT(iap == 1);
		session->ReportStatus(ETrue,iap,80);
		RDebug::Print(_L("Got iap %d at index %d"),iap,i);
	}
	
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

static void DumpIAPS(MProperties* aProperties)
{
	TInt count = 0;
	TRAPD(ignore,count = aProperties->GetIntL(PROPERTY_INT_DB_COUNT,0));
	RDebug::Print(_L("Runtime DB count %d"),count);
	for(TInt i=0;i<count;i++)
	{
		TInt iap = aProperties->GetIntL(PROPERTY_INT_IAP,i);
		TInt port = aProperties->GetIntL(PROPERTY_INT_PORT,i);
		TInt priority = aProperties->GetIntL(PROPERTY_INT_PRIORITY,i);
		RDebug::Print(_L("IAP %d port %d priority %d"),iap,port,priority);
	}
	
}

void CTestAPNManagerWithPorts::testRunTimeDatabase()
{
	/*MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	properties->SetIntL(PROPERTY_INT_DB_COUNT,3,0);
	
	properties->SetIntL(PROPERTY_INT_IAP,1,0);
	properties->SetIntL(PROPERTY_INT_PORT,80,0);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x100,0);
	
	properties->SetIntL(PROPERTY_INT_IAP,2,1);
	properties->SetIntL(PROPERTY_INT_PORT,90,1);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x200,1);
	
	properties->SetIntL(PROPERTY_INT_IAP,3,2);
	properties->SetIntL(PROPERTY_INT_PORT,100,2);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x300,2);
			
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	
	DumpIAPS(properties);
	TInt count = properties->GetIntL(PROPERTY_INT_DB_COUNT,0);
	TS_ASSERT(count > 0);
	properties->Reset();
	DumpIAPS(properties);
	count = 0;
	TS_ASSERT_THROWS(count = properties->GetIntL(PROPERTY_INT_DB_COUNT,0),KErrNotFound);
	TS_ASSERT(count == 0);
		
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
	*/
}

void CTestAPNManagerWithPorts::testRunTimePriority()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	REComPlusSession::SetDelegate(Create);

	properties->SetIntL(PROPERTY_INT_DB_COUNT,3,0);
	
	properties->SetIntL(PROPERTY_INT_IAP,1,0);
	properties->SetIntL(PROPERTY_INT_PORT,80,0);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x300,0);
	
	properties->SetIntL(PROPERTY_INT_IAP,2,1);
	properties->SetIntL(PROPERTY_INT_PORT,80,1);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x200,1);
	
	properties->SetIntL(PROPERTY_INT_IAP,3,2);
	properties->SetIntL(PROPERTY_INT_PORT,80,2);
	properties->SetIntL(PROPERTY_INT_PRIORITY,0x100,2);
			
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetBeingTested();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	DumpIAPS(properties);
	TInt iap = session->GetNextIAP(80);
	TS_ASSERT(iap == 3);
	session->ReportStatus(EFalse,iap,80);
	iap = session->GetNextIAP(80);
	
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

void CTestAPNManagerWithPorts::testFail()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(session);
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	MIAPSession* session2 = manager->StartIAPSession();
	CleanupReleasePushL(*session2);
		
	TInt iap = session->GetNextIAP(80);
	session->ReportStatus(EFalse,iap,80);
	TInt iap2 = session->GetNextIAP(80);
	TS_ASSERT(iap != iap2);
	
	TInt iap3 = session2->GetNextIAP(993);
	session2->ReportStatus(ETrue,iap,993);
	TInt iap4 = session2->GetNextIAP(993);
	TS_ASSERT(iap3 == iap4);
	
	
	CleanupStack::PopAndDestroy(session2);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
	
}

void CTestAPNManagerWithPorts::testFailedPortIgnore()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(session);
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);

	MIAPSession* session2 = manager->StartIAPSession();
	CleanupReleasePushL(*session2);
		
	TInt iap = session->GetNextIAP(80);
	session->ReportStatus(EFalse,iap,80);
	TInt iap2 = session->GetNextIAP(80);
	TS_ASSERT(iap != iap2);
	session->ReportStatus(ETrue,iap2,80);

	TInt iap3 = session2->GetNextIAP(80);
	TS_ASSERT(iap3 == iap2);
	
	CleanupStack::PopAndDestroy(session2);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}

void CTestAPNManagerWithPorts::testGroupedPort()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	
	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork(),0);

	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetBeingTested();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(session);
	session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
	}
	TInt iap = session->GetNextIAP(80);
	session->ReportStatus(ETrue,iap,80);
	TInt iap2 = session->GetNextIAP(993);
	session->ReportStatus(EFalse,iap2,993);
	TInt iap3 = session->GetNextIAP(993);
	MIAPSession* session2 = manager->StartIAPSession();
	CleanupReleasePushL(*session2);

	TInt iap4 = session2->GetNextIAP(80);
	TS_ASSERT(iap == iap4);
	
	CleanupStack::PopAndDestroy(session2);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
}
