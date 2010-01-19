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

#ifndef __SOCKETDEBUG_H_
#define __SOCKETDEBUG_H_

#ifdef __WINSCW__
//#define _DEBUG_SOCKET_
//#define _DEBUG_ERROR_
/* Be careful with this. It makes the socket APIs fail at a predetermined rate */
//#define _DEBUGFAILAPI_
#endif

//#define _DEBUG_SOCKET_LEVEL_1

#ifdef _DEBUG_SOCKET_LEVEL_1
#define _DEBUG_SOCKET_
#define _DEBUG_LEVEL 1
#elif _DEBUG_SOCKET_LEVEL_2
#define _DEBUG_SOCKET_
#define _DEBUG_LEVEL 2
#endif

#ifdef _DEBUGFAILAPI_
static TInt gRate = 100;
static TInt gClientFail = 0;
static TInt gServerFail = 0;
static TBool gEnabled = ETrue;

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __DBG_FILE__ WIDEN(__FILE__)

#define DEBUGFAIL { if(gEnabled && ++gClientFail >= gRate)\
	{\
	RDebug::Print(_L("Fail %s %d"),__DBG_FILE__, __LINE__); \
	gClientFail=0;\
	return PCSL_NET_IOERROR;\
	}}
#define DEBUGLEAVE { if(gEnabled && ++gServerFail >= gRate)\
	{\
	RDebug::Print(_L("Leave %s %d"),__DBG_FILE__, __LINE__); \
	gServerFail=0;\
	User::Leave(-43);\
	}}
#else
#define DEBUGFAIL
#define DEBUGLEAVE
#endif

#ifdef _DEBUG_SOCKET_

class RSocketDebugger
{
public:	
	void Log(const TDesC& aMsg)
	{
#ifndef __WINSCW__
		TBuf<32> filename;
		filename.Format(_L("sockets-%d.txt"),(TInt)RThread().Id());
		RFileLogger::Write(_L("BlueWhale"), filename, EFileLoggingModeAppend,aMsg);
#endif
	}
	void Log(const TDesC& aMsg,TAny* aPtr,TInt aVal)
	{
		TBuf<512> buffer;
		buffer.Format(aMsg,aPtr,aVal);
		Log(buffer);
	}
	void Log(const TDesC& aMsg,TAny* aPtr,const TDesC* aStr)
	{
		TBuf<512> buffer;
		buffer.Format(aMsg,aPtr,aStr);
		Log(buffer);
	}
	void Log(const TDesC& aMsg,const TDesC* aStr1,const TDesC* aStr2)
	{
		TBuf<512> buffer;
		buffer.Format(aMsg,aStr1,aStr2);
		Log(buffer);
	}
	void Log(const TDesC& aMsg,TAny* aPtr)
	{
		TBuf<512> buffer;
		buffer.Format(aMsg,aPtr);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TAny* aPtr,TInt aVal1,TInt aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aPtr,aVal1,aVal2);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal1,TInt aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aVal2);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal1,const TDesC* aStr)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aStr);
		Log(buffer);
	}
	
	void Log(const TDesC& aMsg,TAny* aPtr,TInt aVal1,const TDesC* aStr)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aPtr,aVal1,aStr);
		Log(buffer);
	}
	
	void Log(const TDesC& aMsg,const TDesC* aStr,TUint32 aVal1)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aStr,aVal1);
		Log(buffer);
	}
	void Log(const TDesC& aMsg,TInt aVal1,TUint32 aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aVal2);
		Log(buffer);
	}
};

static RSocketDebugger gLogger;

#define DEBUGPRINT(_LEVEL_,_XX) if(_LEVEL_<=_DEBUG_LEVEL){RDebug::Print(_XX);gLogger.Log(_XX);}
#define DEBUGPRINT1(_LEVEL_,_XX,_YY) if(_LEVEL_ <= _DEBUG_LEVEL){ RDebug::Print(_XX,_YY);gLogger.Log(_XX,_YY);}
#define DEBUGPRINT2(_LEVEL_,_XX,_YY,_ZZ) if(_LEVEL_<=_DEBUG_LEVEL){ RDebug::Print(_XX,_YY,_ZZ);gLogger.Log(_XX,_YY,_ZZ);}
#define DEBUGPRINT3(_LEVEL_,_XX,_YY,_ZZ,_AA) if(_LEVEL_<=_DEBUG_LEVEL){ RDebug::Print(_XX,_YY,_ZZ,_AA);gLogger.Log(_XX,_YY,_ZZ,_AA);}
#else
#define DEBUGPRINT(_LEVEL_,_XX)
#define DEBUGPRINT1(_LEVEL_,_XX,_YY)
#define DEBUGPRINT2(_LEVEL_,_XX,_YY,_ZZ)
#define DEBUGPRINT3(_LEVEL_,_XX,_YY,_ZZ,_AA)
#endif

#ifdef _DEBUG_ERROR_
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
#include <textresolver.h>
#endif
class RSocketDebuggerErr
{
public:	
	void Log(const TDesC& aMsg)
	{
		RFileLogger::Write(_L("BlueWhale"), _L("socket-error.txt"), EFileLoggingModeAppend,aMsg);
	}
	void Log(const TDesC& aMsg,TAny* aPtr,TInt aVal)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aPtr,aVal);
		Log(buffer);
	}
	void Log(const TDesC& aMsg,TAny* aPtr)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aPtr);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TAny* aPtr,TInt aVal1,TInt aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aPtr,aVal1,aVal2);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal);
		Log(buffer);
	}
	
	void LogErr(const TDesC& aMsg,TInt aError)
	{
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		CTextResolver* resolver = NULL;
		TRAPD(err,resolver = CTextResolver::NewL());
		if(err == KErrNone)
		{
			TBuf<128> buffer;
			TBuf<128> newFormat;
			TPtrC errText =  resolver->ResolveErrorString(aError);
			newFormat.Copy(aMsg);
			newFormat.Append(_L(" Err text %S"));
			buffer.Format(newFormat,aError,&errText);
			Log(buffer);
//			CleanupStack::PopAndDestroy(resolver);
		}
		else
		{
			TBuf<128> buffer;
			buffer.Format(aMsg,aError);
			Log(buffer);
		}
#else
		TBuf<128> buffer;
		buffer.Format(aMsg,aError);
		Log(buffer);
#endif
	}
	void Log(const TDesC& aMsg,TInt aVal1,TInt aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aVal2);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal1,const TDesC* aStr)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aStr);
		Log(buffer);
	}

	void Log(const TDesC& aMsg,TInt aVal1,TUint32 aVal2)
	{
		TBuf<128> buffer;
		buffer.Format(aMsg,aVal1,aVal2);
		Log(buffer);
	}
};

static RSocketDebuggerErr gLoggerErr;
#define DEBUGERR(_XX) RDebug::Print(_XX);gLoggerErr.Log(_XX);
#define DEBUGERR1(_XX,_YY) RDebug::Print(_XX,_YY);gLoggerErr.LogErr(_XX,_YY);
#define DEBUGERR2(_XX,_YY,_ZZ) RDebug::Print(_XX,_YY,_ZZ);gLoggerErr.Log(_XX,_YY,_ZZ);
#define DEBUGERR3(_XX,_YY,_ZZ,_AA) RDebug::Print(_XX,_YY,_ZZ,_AA);gLoggerErr.Log(_XX,_YY,_ZZ,_AA);
#else
#define DEBUGERR(_XX)
#define DEBUGERR1(_XX,_YY)
#define DEBUGERR2(_XX,_YY,_ZZ)
#define DEBUGERR3(_XX,_YY,_ZZ,_AA)
#endif



#endif /*SOCKETDEBUG_H_*/
