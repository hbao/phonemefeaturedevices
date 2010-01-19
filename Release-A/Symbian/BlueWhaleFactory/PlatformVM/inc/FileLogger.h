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

#ifndef __FILELOGGER_H__
#define __FILELOGGER_H__
#define __DEBUG_IAPINFO__

#ifdef __ENABLE_LOG__
#include <flogger.h>
#include <e32svr.h>
_LIT(KBlueWhale,"BlueWhale");

static void WriteLog(const TDesC& aBuffer)
{
#ifdef __WINSCW__
	RDebug::Print(aBuffer);
#else
	RFileLogger::Write(KBlueWhale(), KLogFileName(), EFileLoggingModeAppend,aBuffer);
#endif
	
}

static void Log(const TDesC& aStr)
{
	TBuf<256> buffer;
	buffer.Format(_L("%S"),&aStr);
	WriteLog(buffer);
}

template<class T> static void Log(const TDesC& aFormat,const T& aVal)
{
	TBuf<256> buffer;
	buffer.Format(aFormat,aVal);
	WriteLog(buffer);
}

template<class T1,class T2> static void Log(const TDesC& aFormat,const T1& aVal1,const T2& aVal2)
{
	TBuf<256> buffer;
	buffer.Format(aFormat,aVal1,aVal2);
	WriteLog(buffer);
}

template<class T1,class T2,class T3> static void Log(const TDesC& aFormat,const T1& aVal1,const T2& aVal2,const T3& aVal3)
{
	TBuf<256> buffer;
	buffer.Format(aFormat,aVal1,aVal2,aVal3);
	WriteLog(buffer);
}

template<class T1,class T2,class T3,class T4> static void Log(const TDesC& aFormat,const T1& aVal1,const T2& aVal2,const T3& aVal3,const T4& aVal4)
{
	TBuf<256> buffer;
	buffer.Format(aFormat,aVal1,aVal2,aVal3,aVal4);
	WriteLog(buffer);
}

#define DEBUGMSG(_AA) Log(_AA);
#define DEBUGMSG1(_AA,_BB) Log(_AA,_BB);
#define DEBUGMSG2(_AA,_BB,_CC) Log(_AA,_BB,_CC);
#define DEBUGMSG3(_AA,_BB,_CC,_DD) Log(_AA,_BB,_CC,_DD);
#define DEBUGMSG4(_AA,_BB,_CC,_DD,_EE) Log(_AA,_BB,_CC,_DD,_EE);
#else
#define DEBUGMSG(_AA)
#define DEBUGMSG1(_AA,_BB)
#define DEBUGMSG2(_AA,_BB,_CC)
#define DEBUGMSG3(_AA,_BB,_CC,_DD)
#define DEBUGMSG4(_AA,_BB,_CC,_DD,_EE)
#endif


#endif /*__FILELOGGER_H__*/
