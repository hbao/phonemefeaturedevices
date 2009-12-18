/*
 *   
 *
 * Copyright  1990-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */
#include <e32std.h>
#include <string.h>
#include <midlet.h>
#include <midp_properties_port.h>
#include <midp_logging.h>
#include <OS_Symbian.hpp>

/**
 * @file
 *
 * Platform implementation of the platformRequest() method call.
 */

/** Property name for platform request handler */
static const char* const PLATFORM_REQUEST_KEY =
    "com.sun.midp.midlet.platformRequestCommand";

/**
 * Starts a new process to handle the given URL. The new process executes
 * the value of the <tt>com.sun.midp.midlet.platformRequestCommand</tt>
 * system property. The URL is passed as this process' sole command-line
 * argument.
 *
 * @param pszUrl The 'C' string URL
 *
 * @return true if the platform request is configured
 */
int platformRequest(char* pszUrl) 
{
	int len = strlen(pszUrl);
    if (len == 0) 
	{
        /*
         * This is a request to cancel. Since a process was already spawned
         * to handle the previous URL, it too late.
         */
        return 1;
    }
	else
	{
		HBufC8* buffer8 = HBufC8::New(len);
		if(buffer8)
		{
			buffer8->Des().Copy((unsigned char*)pszUrl,len);
			HBufC* buffer = HBufC::New(len);
			if(buffer)
			{
				buffer->Des().Copy(buffer8->Des());
				if(!static_cast<MApplication*>(Dll::Tls())->PlatformRequest(*buffer))
				{
					KNI_ThrowNew("javax.microedition.io.ConnectionNotFoundException", "Invalid platform request");
				}
				delete buffer;
			}
			delete buffer8;
	
		}
	}
    return 1;
}


