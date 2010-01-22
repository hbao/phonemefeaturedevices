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
#include "Properties.h"
#include "TestAutoAPN.h"
#include "APNManager.h"
#include "TestAPNManagerCommDBWrapper.h"
#include "TestAPNManagerTelephonyWrapper.h"
#include "TestWithPortConnectionMonitorWrapper.h"

static CTestSimpleCommDBWrapper* gCurrentDB = NULL;
static CTestSimpleCommDBWrapper* gDuplicateDB = NULL;

void CTestAutoAPN::setUp()
{}

void CTestAutoAPN::tearDown()
{
    REComPlusSession::SetDelegate(NULL);
}

MUnknown * CTestAutoAPN::Create(TUid aImplementationUid, TUid /*aInterfaceUid*/, TAny* /*aConstructionParameters*/)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MCommDBWrapper:
	        gCurrentDB = new (ELeave) CTestSimpleCommDBWrapper(gDuplicateDB);
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

MUnknown * CTestAutoAPN::CreateVFIE(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CVFIETelephonyWrapper;
			break;
		default:
			ret = Create(aImplementationUid,aInterfaceUid,aConstructionParameters);
	}
	return ret;
}

MUnknown * CTestAutoAPN::ChangingCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	MUnknown * ret = NULL;
	switch(aImplementationUid.iUid)
	{
		case KCID_MTelephonyWrapper:
			ret = new (ELeave) CChangingTelephonyWrapper;
			break;
		default:
			ret = Create(aImplementationUid,aInterfaceUid,aConstructionParameters);
	}
	return ret;
}
MUnknown * CTestAutoAPN::UnknownCreate(TUid aImplementationUid, TUid aInterfaceUid, TAny* aConstructionParameters)
{
	MUnknown * ret = NULL;
		switch(aImplementationUid.iUid)
		{
			case KCID_MTelephonyWrapper:
				ret = new (ELeave) CUnknownTelephonyWrapper;
				break;
			default:
				ret = Create(aImplementationUid,aInterfaceUid,aConstructionParameters);
		}
		return ret;
}
void CTestAutoAPN::testSet()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
	gDuplicateDB = new (ELeave) CTestSimpleCommDBWrapper();
	gDuplicateDB->AddRef();
	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetAutoAPN();
	CActiveScheduler::Start();
	gCurrentDB->AddRef();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CleanupStack::PopAndDestroy(session);

	if(gDuplicateDB)
	{
	    gDuplicateDB->Dump();
		TBool found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale1"));
		TS_ASSERT(found)
	}
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
    gDuplicateDB->Release();
    gDuplicateDB = NULL;
}

void CTestAutoAPN::testNotSet()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
    gDuplicateDB = new (ELeave) CTestSimpleCommDBWrapper();
    gDuplicateDB->AddRef();
	
	REComPlusSession::SetDelegate(Create);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	CActiveScheduler::Start();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CleanupStack::PopAndDestroy(session);

    if(gDuplicateDB)
    {
        gDuplicateDB->Dump();
		TBool found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale!"));
		TS_ASSERT(!found);
	}

	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
    gDuplicateDB->Release();
    gDuplicateDB = NULL;
}

_LIT(KNetwork2,"33"); // Orange

void CTestAutoAPN::testSetWithChangingNetwork()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
    gDuplicateDB = new (ELeave) CTestSimpleCommDBWrapper();
    gDuplicateDB->AddRef();
	
	REComPlusSession::SetDelegate(ChangingCreate);

	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KNetwork2(),0);

	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetAutoAPN();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	if(gCurrentDB)
	{
		gCurrentDB->Dump();
		TBool found = gCurrentDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale1"));
		TS_ASSERT(!found);
		found = gCurrentDB->Find(_L("OutgoingGPRS"),TPtrC(GPRS_APN),_L("orangeinternet"));
		TS_ASSERT(!found);
	}

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(session);

	MIAPSession* session2 = manager->StartIAPSession();
	CleanupReleasePushL(*session2);
	CleanupStack::PopAndDestroy(session2);

	if(gDuplicateDB)
	{
	    gDuplicateDB->Dump();
		TBool found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale1"));
		TS_ASSERT(found);
		found = gDuplicateDB->Find(_L("OutgoingGPRS"),TPtrC(GPRS_APN),_L("general.t-mobile.uk"));
		TS_ASSERT(found);
		
	}

	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
    gDuplicateDB->Release();
    gDuplicateDB = NULL;
	
}

void CTestAutoAPN::testWithUnknownNetwork()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
    gDuplicateDB = new (ELeave) CTestSimpleCommDBWrapper();
    gDuplicateDB->AddRef();
	
	REComPlusSession::SetDelegate(UnknownCreate);

	properties->DeleteString(PROPERTY_STRING_COUNTRY_CODE,0);
	properties->DeleteString(PROPERTY_STRING_NETWORK_ID,0);
	properties->SetStringL(PROPERTY_STRING_COUNTRY_CODE,KUnknownCountry(),0);
	properties->SetStringL(PROPERTY_STRING_NETWORK_ID,KUnknownNetwork(),0);

	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetAutoAPN();
	CActiveScheduler::Start();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	if(gDuplicateDB)
	{
	    gDuplicateDB->Dump();
		TBool found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale1"));
		TS_ASSERT(!found);
	}


	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
    gDuplicateDB->Release();
    gDuplicateDB = NULL;
}

void CTestAutoAPN::testMultipleAPNs()
{
	MProperties* properties = DiL(MProperties);
	CleanupReleasePushL(*properties);
    gDuplicateDB = new (ELeave) CTestSimpleCommDBWrapper();
    gDuplicateDB->AddRef();
	
	REComPlusSession::SetDelegate(CreateVFIE);
	
	CAPNManager* manager = CAPNManager::NewL(properties);
	CleanupStack::PushL(manager);
	manager->SetAutoAPN();
	CActiveScheduler::Start();
	MIAPSession* session = manager->StartIAPSession();
	CleanupReleasePushL(*session);
	CleanupStack::PopAndDestroy(session);

	if(gDuplicateDB)
	{
	    gDuplicateDB->Dump();
		TBool found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale1"));
		TS_ASSERT(found)
		found = gDuplicateDB->Find(_L("IAP"),TPtrC(COMMDB_NAME),_L("BlueWhale2"));
		TS_ASSERT(found)
	}

	CleanupStack::PopAndDestroy(manager);
	CleanupStack::PopAndDestroy(properties);
    gDuplicateDB->Release();
    gDuplicateDB = NULL;
}
