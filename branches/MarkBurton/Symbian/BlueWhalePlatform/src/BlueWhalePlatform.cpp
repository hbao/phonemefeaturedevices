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

#ifdef __WINSCW__
//#define __DEBUGMAINPLATFORM__
#endif

#ifdef __DEBUGMAINPLATFORM__
#define DEBUGPRINT(_XX) RDebug::Print(_XX)
#define DEBUGPRINT1(_XX,_AA) RDebug::Print(_XX,_AA)
#else
#define DEBUGPRINT(_XX)
#define DEBUGPRINT1(_XX,_AA)
#endif

#include <e32std.h>
#include "BlueWhalePlatformApplication.h"
#include <OSVersion.h>

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
{
    return new CBlueWhalePlatformApplication;
}

#ifdef __WINSCW__
extern "C" {
	void _DisposeAllThreadData();
}

GLDEF_C TInt E32Main()
{
	DEBUGPRINT(_L("BlueWhalePlatform Starting"));
	TRAPD(Err, User::Leave(1)); // fix for duff Symbian code that makes it look like a memory leak
	TInt ret = EikStart::RunApplication( NewApplication );
	_DisposeAllThreadData();
	DEBUGPRINT1(_L("BlueWhalePlatform complete %d"),ret);
	return ret;
}

#else

GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}

#endif //__WINSCW__

#else // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__

// Epoc DLL entry point, return that everything is ok
GLDEF_C TInt E32Dll(TDllReason)
{
    return KErrNone;
}

// Create an BlueWhaleMail application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication()
{
    return (new CBlueWhalePlatformApplication);
}

#endif // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
