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

 
#ifndef TESTFEP_H
#define TESTFEP_H

#include <cxxtest/TestSuite.h>
#include "FEPInputControl.h"

class CTestFEP : public CxxTest::TestSuite, public MFEPInputCoreObserver
{	
public:
	CTestFEP(const TDesC8& aSuiteName):CxxTest::TestSuite(aSuiteName){}

	// from MFEPInputCoreObserver
	virtual TBool PredictiveText() const;
	virtual TBool NumericText() const;
	virtual TBool FieldAllowsChar(TChar aChar) const;
	virtual void AddEvent(TEventInfo& aInfo);
	virtual TInt MachineUidValue() const;
	virtual TUint Capabilities() const;

private:
	void setUp();
	void tearDown();
	
	// wrapper methods that call CFEPInputCore methods and then assert appropriate values
	void CallOfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType, TInt aAssertLength);
	void CallUpdateFepInlineTextL(const TDesC& aNewInlineText, const TDesC& aAssertText);
	void CallDoCommitFepInlineEditL(TInt aAssertLength);

	
public:
	// the tests
	void testPredictive1();
	void testMultiTap1();
	void testNumeric1();
	
private:
	TBool iPredictiveText;
	TBool iNumericText;
	CIndexedText* iText;
	MKeyMapper* iKeyMapper;
	CFEPInputCore* iFEPInputCore;
	TInt iMachineUidValue;
};


#endif /*TESTFEP_H*/
