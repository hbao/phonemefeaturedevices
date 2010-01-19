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



#include "LoggerImpl.h"
#include "DebugUtilities.h"
#include "LoggerMessage.h"
#include <f32file.h>
#include <s32file.h>

_LIT(KDefaultLoggingDirectoryRelativeToLogs,"Bluewhale");
_LIT( KDefaultLoggingFileName, "platform.txt");

MLogger * CLogger::NewL(TAny * aConstructionParameters)
{
	CLogger * self = new (ELeave) CLogger(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();
	MLogger * logger = QiL( self, MLogger);
	CleanupStack::Pop(self);
	return logger;
}

CLogger::CLogger(TAny * aConstructionParameters)
	: CEComPlusRefCountedBase(aConstructionParameters)
	, iLoggingLevel( EWarn )
{
	DEBUG_CONSTRUCTOR
}

void CLogger::ConstructL()
{
	User::LeaveIfError( iFileLogger.Connect() );
	iFileLogger.CreateLog(KDefaultLoggingDirectoryRelativeToLogs, KDefaultLoggingFileName, EFileLoggingModeAppend);
}


MUnknown * CLogger::QueryInterfaceL(TInt aInterfaceId)
{
	if( KIID_MUnknown == aInterfaceId )
	{
		AddRef();
		return static_cast<MLogger*>(this);
	}
	else if( KIID_MLogger == aInterfaceId )
	{
		AddRef();
		return static_cast<MLogger*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

CLogger::~CLogger()
{
	DEBUG_DESTRUCTOR

	if(iFileLogger.Handle() != 0)
	{
		iFileLogger.CloseLog();
		iFileLogger.Close();
	}
}

void CLogger::SetLoggingLevel(TLoggingLevel aLoggingLevel)
{
	iLoggingLevel = aLoggingLevel;
}

void CLogger::Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
		TRAPD( ignoreError, LogL( aLoggingLevel, aErrorNumber, aMessage, 0, 0, 0 ) );
	}
}

void CLogger::Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
	TRAPD( ignoreError, LogL( aLoggingLevel, aErrorNumber, aMessage, aInt, 0, 1 ) );
	}
}

void CLogger::Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt1, TInt aInt2 )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
	TRAPD( ignoreError, LogL( aLoggingLevel, aErrorNumber, aMessage, aInt1, aInt2, 2 ) );
	}
}

void CLogger::Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC & aString )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
	TRAPD( ignoreError, LogL( aLoggingLevel, aErrorNumber, aMessage, aString ) );
	}
}

void CLogger::Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC8 & aString )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
	TRAPD( ignoreError, LogL( aLoggingLevel, aErrorNumber, aMessage, aString ) );
	}
}

void CLogger::LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt1, TInt aInt2, TInt aIntsToUse )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
		HBufC * buffer = HBufC::NewLC( aMessage.Length() + 40 /* fudge */ );
		TPtr ptr = buffer->Des();

		ptr.Append(aMessage);
		
		TBuf<12> numberBuf;

		if( aIntsToUse > 0 )
		{
			numberBuf.Num(aInt1);
			ptr.Append(numberBuf);
		}
		else if( aIntsToUse > 1 )
		{
			numberBuf.Num(aInt2);
			ptr.Append(numberBuf);
		}

		CleanupStack::Pop(buffer);
		CreateLogEntryL( aLoggingLevel, aErrorNumber, buffer ); // Took ownership of buffer.

	}
}

_LIT(KSeparator, ": ");

void CLogger::LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC & aString )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__
	{
		HBufC * buffer = HBufC::NewLC( aMessage.Length() + aString.Length() + KSeparator().Length() );
		TPtr ptr = buffer->Des();

		ptr.Append(aMessage);
		ptr.Append(KSeparator);
		ptr.Append(aString);

		CleanupStack::Pop(buffer);
		CreateLogEntryL( aLoggingLevel, aErrorNumber, buffer ); // Took ownership of buffer.

	}
}

void CLogger::LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC8 & aString )
{
#ifndef __WINS__   // Don't do this check just yet in __WINS__ so that we log to console in debugger.
	if( aLoggingLevel >= iLoggingLevel )
#endif // __WINS__   
	{

		HBufC * wideBuffer = HBufC::NewLC(aString.Length());
		TPtr wideBufferPtr = wideBuffer->Des();
		wideBufferPtr.Copy(aString);

		HBufC * buffer = HBufC::NewLC( aMessage.Length() + aString.Length() + KSeparator().Length() );
		TPtr ptr = buffer->Des();

		ptr.Append(aMessage);
		ptr.Append(KSeparator);
		ptr.Append(*wideBuffer);

		CleanupStack::Pop(buffer);
		CreateLogEntryL( aLoggingLevel, aErrorNumber, buffer ); // Took ownership of buffer.

		CleanupStack::PopAndDestroy(wideBuffer);
	}
}



void CLogger::CreateLogEntryL(TLoggingLevel aLoggingLevel, TInt aError, HBufC * aTextBuffer )
{
	CleanupStack::PushL(aTextBuffer); // a bit dodgey. Must ensure caller hasn't left this on the stack
	if( aLoggingLevel >= iLoggingLevel )
	{
#ifdef __WINSCW__
		{
			HBufC * newTextBufferWithErrorPrepended = HBufC::NewLC(aTextBuffer->Size() + 10 /* padding */ );
			TPtr newTextBufferWithErrorPrependedPtr = newTextBufferWithErrorPrepended->Des();
			newTextBufferWithErrorPrependedPtr.Format(_L("%d %S"), aError, aTextBuffer );

			if( newTextBufferWithErrorPrepended->Length() > KRDebugPrintGetsPissedOffSize )
			{
				DebugPrint(0, *newTextBufferWithErrorPrepended);
			}
			else
			{
				RDebug::Print(*newTextBufferWithErrorPrepended);
			}

			CleanupStack::PopAndDestroy(newTextBufferWithErrorPrepended);
		}
#endif // __WINSCW__

		iFileLogger.WriteFormat(_L(" %d %d %S"), aLoggingLevel, aError, &(*aTextBuffer));
	}
	CleanupStack::PopAndDestroy(aTextBuffer);
}
