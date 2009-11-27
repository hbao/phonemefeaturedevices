/*
 *  
 *
 * Copyright  1990-2006 Sun Microsystems, Inc. All Rights Reserved.
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

/** @file
 *
 * This file implements all the necessary PCSL interfaces for the print service.
 */

#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>
#include <pcsl_print.h>
#include <f32file.h>
#include <string.h>
#include <OS_Symbian.hpp>

/**
 * Print contents of buffer to stdout.
 *
 */
TBuf<256> buffer;
TBuf8<256> buffer8;

void pcsl_print(const char* s) 
{
	char * ptr = (char*)s;
	while(*ptr)
	{
		if(*ptr != '\n' && *ptr != '\r' && buffer8.Length() < 256)
		{
			buffer8.Append(*ptr);
		}
		else
		{
			buffer.Copy(buffer8);
#ifdef __WINS__
			RDebug::Print(_L("%S"),&buffer);
#endif
			buffer8.Copy(_L8(""));
			static_cast<MApplication*>(Dll::Tls())->DebugMessage(buffer);
		}
		ptr++;
	}
}
