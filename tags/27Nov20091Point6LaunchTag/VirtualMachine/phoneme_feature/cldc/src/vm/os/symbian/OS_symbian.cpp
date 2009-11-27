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

#include "incls/_precompiled.incl"
#include "incls/_OS_symbian.cpp.incl"
#include <pcsl_print.h>

void Os::initialize() 
{}

jlong Os::java_time_millis() 
{
	TTime NineteenSeventy(_L("19700000:000000.000000"));
	TTime now;
	now.UniversalTime();
 	TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom(NineteenSeventy);
	TInt64 time = interval.Int64() / 1000;
	return ((jlong)I64HIGH(time) << 32) | (jlong)I64LOW(time);
}
void Os::sleep(jlong ms) 
{
	//pcsl_print("Os::sleep");
	User::After(ms*1000);
}

TInt TickerMain(TAny* aSelf)
{
  //pcsl_print("TickerMain\n");
	if(static_cast<MApplication*>(aSelf)->TickerRunning())
	{
		real_time_tick(KInterval/1000);
	}
	return KErrNone;
}

bool Os::start_ticks() 
{
	return static_cast<MApplication*>(Dll::Tls())->StartTicker(TickerMain);
}

void Os::stop_ticks() 
{
	static_cast<MApplication*>(Dll::Tls())->StopTicker();
}

void Os::suspend_ticks() 
{
	static_cast<MApplication*>(Dll::Tls())->SuspendTicker();
}

void Os::resume_ticks() 
{
	static_cast<MApplication*>(Dll::Tls())->ResumeTicker();
}

void Os::dispose() 
{
}

void Os::start_compiler_timer() 
{
}

bool Os::check_compiler_timer() 
{
	return true;
}

#ifndef PRODUCT
jlong Os::elapsed_counter() 
{
	return User::TickCount();
}

jlong Os::elapsed_frequency() 
{
	return 64;
}
#endif

