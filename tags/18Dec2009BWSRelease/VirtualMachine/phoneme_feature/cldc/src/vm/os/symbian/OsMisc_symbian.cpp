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

/*
 * OsMisc_symbian.cpp:
 */

#include "incls/_precompiled.incl"
#include "incls/_OsMisc_symbian.cpp.incl"
#include "sni.h"
#include "jvmspi.h"
#include "pcsl_print.h"
#include "pcsl_string.h"
#include <midpservices.h>
#include <midp_thread.h>
#include <midpport_eventqueue.h>
#include <midpEventUtil.h>
#include <keymap_input.h>

#ifdef __WINSCW__
//#define __DEBUGEVENTS__
#endif

extern "C" {
#ifndef __WINS__
void _rom_linkcheck_mffd_false()
{}
#endif
void __aeabi_atexit()
{}
void __dso_handle()
{}

}
const JvmPathChar *OsMisc_get_classpath() 
{
    return NULL;
}
void OsMisc_flush_icache(address start, int size) 
{
}

#if !defined(PRODUCT) || USE_DEBUG_PRINTING

const char *OsMisc_jlong_format_specifier() {
  return "%I64d";
}

const char *OsMisc_julong_format_specifier() {
  return "%I64u";
}
#endif

void JVMSPI_CheckEvents(JVMSPI_BlockedThreadInfo * blocked_threads,int blocked_threads_count, jlong timeout_ms) 
{
	TEventInfo info;
	
	if(blocked_threads_count > 0)
	{
#ifdef __DEBUGEVENTS__
		MidpReentryData* threadReentryData;
		TBool doneHeader = EFalse;
		for(int i=0;i<blocked_threads_count;i++)
		{
			threadReentryData = (MidpReentryData*)(blocked_threads[i].reentry_data);
			if(threadReentryData)
			{
				if(!doneHeader)
				{
					doneHeader = ETrue;
					RDebug::Print(_L("========= VM event ==============="));
				}
				RDebug::Print(_L("blocked thread 0x%08x aClient 0x%08x waiting on %d"),blocked_threads[i].thread_id,threadReentryData->descriptor,threadReentryData->waitingFor);
			}
			
		}
		if(doneHeader)
		{
			RDebug::Print(_L("=================================="));
		}
		
#endif
		static_cast<MApplication*>(Dll::Tls())->GetNextEvent(timeout_ms,info);
	}
	else
	{
		static_cast<MApplication*>(Dll::Tls())->GetNextEvent(0,info);
	}
	switch(info.iEvent)
	{
		case -1: // do nothing
			break;
		case KMIDPKeyDownEvent: 
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_KEY_EVENT;
			event.CHR = info.iIntParam1;
			event.ACTION = KEYMAP_STATE_PRESSED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;
		}
		case KMIDPKeyUpEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_KEY_EVENT;
			event.CHR = info.iIntParam1;
			event.ACTION = KEYMAP_STATE_RELEASED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;
		}
		case KMIDPKeyRepeatEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_KEY_EVENT;
			event.CHR = info.iIntParam1;
			event.ACTION = KEYMAP_STATE_REPEATED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;	
		}
		case KMIDPPenPressedEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_PEN_EVENT;
			event.X_POS = info.iIntParam1;
			event.Y_POS = info.iIntParam2;
			event.ACTION = KEYMAP_STATE_PRESSED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;	
		}
		case KMIDPPenReleaseEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_PEN_EVENT;
			event.X_POS = info.iIntParam1;
			event.Y_POS = info.iIntParam2;
			event.ACTION = KEYMAP_STATE_RELEASED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;	
		}
		case KMIDPPenDragEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_PEN_EVENT;
			event.X_POS = info.iIntParam1;
			event.Y_POS = info.iIntParam2;
			event.ACTION = KEYMAP_STATE_DRAGGED;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;	
		}
		case KMIDPForegroundEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			if(info.iIntParam1)
			{
				event.type = FOREGROUND_NOTIFY_EVENT;
			}
			else
			{
				event.type = BACKGROUND_NOTIFY_EVENT;
			}
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;	
		}
		case KMIDPCommandEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = MIDP_COMMAND_EVENT;
			event.intParam4 = 1;
			event.COMMAND = info.iIntParam1;
			midpStoreEventAndSignalForeground(event);
			break;
		}
		case KMIDPScreenChangeEvent:
		{
			pcsl_print("KMIDPScreenChangeEvent\n");
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = ROTATION_EVENT;
			event.intParam4 = 1;
			midpStoreEventAndSignalForeground(event);
			break;
		}
		case KMIDPPushRegisterEvent:
		{
			MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = PUSH_REGISTER_EVENT;
			event.intParam1 = info.iIntParam1;
			event.intParam2 = info.iIntParam2;
			if(info.iStringParam1)
			{
				pcsl_string_convert_from_utf16(info.iStringParam1->Ptr(), info.iStringParam1->Length(), &event.stringParam4);
			}
			midpStoreEventAndSignalAms(event);
			break;
		}
        case KMIDPPlatformRequestInstallEvent:
        {
        	MidpEvent event;
			MIDP_EVENT_INITIALIZE(event);
			event.type = PLATFORM_REQUEST_INSTALL_EVENT;
			event.intParam1 = info.iIntParam1;
			event.intParam2 = info.iIntParam2;
			if(info.iStringParam1)
			{
				pcsl_string_convert_from_utf16(info.iStringParam1->Ptr(), info.iStringParam1->Length(), &event.stringParam1);
			}
			midpStoreEventAndSignalAms(event);
			break;
        }
		default:
			if(blocked_threads_count > 0)
			{
				midp_thread_signal((midpSignalType)info.iEvent,info.iHandle,0);
			}
	}

	if(timeout_ms > 0 && blocked_threads_count == 0)
	{
		// no Java threads blocked, but VM asked us to sleep for a while
		Os::sleep(timeout_ms);
	}
}
