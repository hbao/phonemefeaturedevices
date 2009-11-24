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

#include "Application.h"
#include <e32property.h>
#include <OSVersion.h>
#if __S60_VERSION__ > __S60_V2_FP1_VERSION_NUMBER__
#include <aknglobalconfirmationquery.h>
#include <avkon.rsg>
#endif

void DoQueryL()
{
	_LIT(KAutoStartText, "Allow application to start automatically?");
	TRequestStatus status;
	TBool autoStart = EFalse;

#if __S60_VERSION__ > __S60_V2_FP1_VERSION_NUMBER__
	CAknGlobalConfirmationQuery* query = CAknGlobalConfirmationQuery::NewLC(); 
	query->ShowConfirmationQueryL(status, KAutoStartText, R_AVKON_SOFTKEYS_YES_NO);
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(query);
	autoStart = (status.Int() == EAknSoftkeyYes);
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	RNotifier notifier;
	if (notifier.Connect() != KErrNone)
	{
		return;
	}
	TInt result = 0;
	notifier.Notify(_L("Autostart"), KAutoStartText, _L("Yes"), _L("No"), result, status); // "Yes" => result is 0
	User::WaitForRequest(status);
	notifier.Close();
	autoStart = !result;
#endif

	RProperty autostartProperty;
	if (autostartProperty.Attach(KUidStarterExe, KUidAutoStart.iUid) == KErrNone)
	{
		autostartProperty.Set(KUidStarterExe, KUidAutoStart.iUid, autoStart);	
		autostartProperty.Close();
	}
}

GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err, DoQueryL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	User::Exit(0);
	return KErrNone;
}



