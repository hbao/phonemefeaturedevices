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
 
 
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>            // Console
#include <e32debug.h>
#include <OSVersion.h>
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include "TestDriver.h"
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include "TestDriver_UIQ.h"
#endif

_LIT(KTextConsoleTitle, "Bluewhalefactory test");
_LIT(KTextFailed, " failed, leave code = %d");


LOCAL_D CConsoleBase* console;  // write all messages to this

class CMyTestFrameWork : public CBase, public MUiUpdater
{
	public:
		CMyTestFrameWork(){}
		void Run()
		{
			WriteMessage(_L("Test run starting\n"));
			iBuffer = HBufC::NewL(1024);
			TestDriver driver(iBuffer,this);
			TRAPD(status,driver.runAllSuitesL());
			WriteMessage(_L("Test run complete\n"));

		}
		~CMyTestFrameWork()
		{
			delete iBuffer;
		}
	void DisplayEachTestResult()
		{
		WriteMessage(iBuffer->Des());
    		WriteMessage(_L("\n"));
		}

	void FailedTest()
		{
		WriteMessage(_L("A test Failed\n"));
		}
	protected:
		void WriteMessage(const TDesC& aMsg)
		{
			console->Write(aMsg);
			RDebug::Print(aMsg);
		}
	private:
		HBufC* iBuffer;

};

LOCAL_C void MainL()
    {
	CMyTestFrameWork framework;
	framework.Run();
    }


LOCAL_C void DoStartL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
    }


GLDEF_C TInt E32Main()
    {
    // Create cleanup stack
	RDebug::Print(_L("Bluewhalefactory tests starting..."));
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Create output console
    TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen)));
    if (createError)
	{
		RDebug::Print(_L("Error creating console %d"),createError);
        return KErrNone;
	}

    TRAPD(mainError, DoStartL());
    if (mainError)
	{
        RDebug::Print(KTextFailed, mainError);
    }
    delete console;
    delete cleanup;
    __UHEAP_MARKEND;
	RDebug::Print(_L("Bluewhalefactory tests complete"));
    return KErrNone;
    }
