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
#include <s32mem.h>
#include "EcomPlus.h"
#include "Properties.h"
#include "TestProperties.h"

void CTestProperties::setUp()
{}

void CTestProperties::tearDown()
{}

void CTestProperties::testCreate()
{
	MProperties* properties = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*properties);
	TS_ASSERT(properties);
	CleanupStack::PopAndDestroy(properties);
}

void CTestProperties::testInt()
{
	MProperties* properties = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*properties);
	TIntUniqueKey testKey = {0x01234567, 0x89abcdef};
	TIntUniqueKey testKey2 = {0xdeadbeef, 0xcafebabe};
	properties->SetIntL(testKey,255);
	TS_ASSERT(properties->GetIntL(testKey) == 255 );
	properties->SetIntL(testKey2,0x12345678);
	TS_ASSERT(properties->GetIntL(testKey2) == 0x12345678 );
	properties->SetIntL(testKey,128);
	TS_ASSERT(properties->GetIntL(testKey) == 128 );
	properties->SetIntL(testKey,568);
	TS_ASSERT(properties->GetIntL(testKey) == 568 );
			
	properties->SetIntL(testKey,255);
	properties->SetIntL(testKey,128,0x1000);
	properties->SetIntL(testKey,568,0x1000);
	TS_ASSERT(properties->GetIntL(testKey,0) == 255 );
	TS_ASSERT(properties->GetIntL(testKey,1) == 128 );
	TS_ASSERT(properties->GetIntL(testKey,2) == 568 );
					
	CleanupStack::PopAndDestroy(properties);
}

void CTestProperties::testInt64()
{
	MProperties* properties = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*properties);
	TInt64UniqueKey testKey64 = {0x01234567, 0x89abcdef};
	TInt64UniqueKey testKey642 = {0xdeadbeef, 0xcafebabe};
	properties->SetInt64L(testKey64,MAKE_TINT64(255,123));
	TS_ASSERT(properties->GetInt64L(testKey64) == MAKE_TINT64(255,123) );
	properties->SetInt64L(testKey642,MAKE_TINT64(0x12345678,0xcafebabe));
	TS_ASSERT(properties->GetInt64L(testKey642) == MAKE_TINT64(0x12345678,0xcafebabe));

	properties->SetInt64L(testKey64,MAKE_TINT64(0,128));
	TS_ASSERT(properties->GetInt64L(testKey64) == MAKE_TINT64(0,128) );
	properties->SetInt64L(testKey64,MAKE_TINT64(0,568));
	TS_ASSERT(properties->GetInt64L(testKey64) == MAKE_TINT64(0,568) );
				
	properties->SetInt64L(testKey64,MAKE_TINT64(0,255));
	properties->SetInt64L(testKey64,MAKE_TINT64(0,128),0x1000);
	properties->SetInt64L(testKey64,MAKE_TINT64(0,568),0x1000);
	TS_ASSERT(properties->GetInt64L(testKey64,0) == MAKE_TINT64(0,255) );
	TS_ASSERT(properties->GetInt64L(testKey64,1) == MAKE_TINT64(0,128) );
	TS_ASSERT(properties->GetInt64L(testKey64,2) == MAKE_TINT64(0,568) );

	CleanupStack::PopAndDestroy(properties);
}
_LIT8(KTestString,"Hello, Bluewhale");

void CTestProperties::testString8()
{
	MProperties* properties = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*properties);
	TString8UniqueKey testKeyStr8= {0x01234567, 0x89abcdef};
	HBufC8 * testBuffer = KTestString().AllocLC();
	properties->SetString8L(testKeyStr8,testBuffer);
	CleanupStack::Pop(testBuffer);
	TS_ASSERT(properties->GetString8L(testKeyStr8).Compare(KTestString()) == 0)
	CleanupStack::PopAndDestroy(properties);
}

void CTestProperties::testExternalize()
{
	MProperties* properties = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*properties);
	MProperties* propertiesIn = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*propertiesIn);

	MProperties* propertiesToExt = static_cast<MProperties*>(REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(KIID_MProperties), NULL));
	CleanupReleasePushL(*propertiesToExt);

	
	TIntUniqueKey testKey = {0x01234567, 0x89abcdef};
	TObjectUniqueKey testObjectKey = {0x01234567, 0xdeadbeef};
	properties->SetIntL(testKey,255);

	
	TString8UniqueKey testKeyStr8= {0x01234567, 0x89abcdef};
	HBufC8 * testBuffer = KTestString().AllocLC();
	properties->SetString8L(testKeyStr8,testBuffer);
	CleanupStack::Pop(testBuffer);

	propertiesToExt->SetIntL(testKey,123);

	properties->SetObjectL(testObjectKey,propertiesToExt);
	
	CleanupStack::PopAndDestroy(propertiesToExt);
	
	MExternalizable* ext = QiL(properties,MExternalizable);
	CleanupReleasePushL(*ext);
	HBufC8* buffer = HBufC8::NewLC(8192);
	TPtr8 bufferDes(buffer->Des());
	RDesWriteStream writeStream(bufferDes);
	CleanupClosePushL(writeStream);
	ext->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	MExternalizable* extIn = QiL(propertiesIn,MExternalizable);
	CleanupReleasePushL(*extIn);

	RDesReadStream readStream(bufferDes);
	CleanupClosePushL(readStream);
	extIn->InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	RDebug::Print(_L("getstring8"));
	const TDesC8& str(propertiesIn->GetString8L(testKeyStr8));
	TBuf<100> str16;
	str16.Copy(str);
	RDebug::Print(_L("got string %S"),&str16);
	
	TS_ASSERT(str.Compare(KTestString()) == 0);
	
	TS_ASSERT(propertiesIn->GetIntL(testKey) == 255);

	MProperties* propertiesToInt = static_cast<MProperties*>(propertiesIn->GetObjectL(testObjectKey,KIID_MProperties));
	CleanupReleasePushL(*propertiesToInt);
	
	TS_ASSERT(propertiesToInt);
	TS_ASSERT(propertiesToInt->GetIntL(testKey) == 123);
	
	CleanupStack::PopAndDestroy(propertiesToInt);
	CleanupStack::PopAndDestroy(extIn);
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(ext);
	
	CleanupStack::PopAndDestroy(propertiesIn);	
	CleanupStack::PopAndDestroy(properties);
}