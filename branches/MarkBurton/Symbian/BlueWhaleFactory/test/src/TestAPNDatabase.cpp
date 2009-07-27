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
#include "TestAPNDatabase.h"

#include "APNDatabase.h"

void CTestAPNDatabase::setUp()
{}

void CTestAPNDatabase::tearDown()
{}

void CTestAPNDatabase::testLoad()
{
	CAPNDatabase db;
	db.LoadDatabaseL();
	TS_ASSERT(db.Count() > 0 );
	db.Close();
}

void CTestAPNDatabase::testFailToFind()
{
	CAPNDatabase db;
	db.LoadDatabaseL();
	TInt found = db.GetEntry(_L("123"),_L("aa"));
	TS_ASSERT(found == KErrNotFound);
	db.Close();
}

void CTestAPNDatabase::testFind()
{
	CAPNDatabase db;
	db.LoadDatabaseL();
	// find Vodafone UK
	TInt found = db.GetEntry(_L("234"),_L("15"));
	TS_ASSERT(found != KErrNotFound);
	db.Close();
}
