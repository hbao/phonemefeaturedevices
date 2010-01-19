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


#ifndef __DEBUG_UTILITIES_H__
#define __DEBUG_UTILITIES_H__

#include <e32std.h>
#include <e32svr.h> // For RDebug::Print
#include "Logger.h"
#include <OSVersion.h>


const TInt KRDebugPrintGetsPissedOffSize = 100;



/**
 * If you try to print too large a string with RDebug::Print, it
 * will get annoyed and you will see some kind of a KERN Panic.
 * These methods print out a buffer one chunk at a time.
 *
 * Note: We have changed to fixed parameter types only as using
 * the variable argument Format mechanism is too error-prone.
 * One wrong format specifier and you've panic'ed
 *
 */
// TODO: IMPORT_C only for test.
IMPORT_C void DebugPrint( TInt aNestingLevel, const TDesC & aMessage );
IMPORT_C void DebugPrint( TInt aNestingLevel, const TDesC8 & aMessage );



//#define DEBUG_MEMORY_LEAK 

#ifdef DEBUG_MEMORY_LEAK
// I've defined some cute macros here which use filename and line number instead of requiring
// class name as a parameter to the macro, because I know that when you copy and paste that
// inevitably gets out of date.
#define DEBUG_CONSTRUCTOR 	DebugPrint(0, TPtrC8((TText8*)("+++Constructor++[0x%08x] at line %d of: "__FILE__)), this, __LINE__ );
#define DEBUG_DESTRUCTOR 	DebugPrint(0, TPtrC8((TText8*)("---Destructor---[0x%08x] at line %d of: "__FILE__)), this, __LINE__ );

#else /* not DEBUG_MEMORY_LEAK */

#define DEBUG_CONSTRUCTOR
#define DEBUG_DESTRUCTOR

#endif /* DEBUG_MEMORY_LEAK */



//#define NO_LOGGING

#ifdef NO_LOGGING


#define LOG_ERROR( ERROR_NUMBER, STRING_LITERAL )
#define LOG_ERROR1( ERROR_NUMBER, STRING_LITERAL, PARAM1  )
#define LOG_ERROR2( ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )
#define LOG_INFO( STRING_LITERAL )
#define LOG_INFO1( STRING_LITERAL, PARAM1 )
#define LOG_INFO2( STRING_LITERAL, PARAM1, PARAM2 )
#define LOG_DEBUG( STRING_LITERAL )
#define LOG_DEBUG1( STRING_LITERAL, PARAM1 )
#define LOG_DEBUG2( STRING_LITERAL, PARAM1, PARAM2 )

#define LOG_ERROR_DESC( ERROR_NUMBER, TEXT_DESCRIPTOR )
#define LOG_ERROR1_DESC( ERROR_NUMBER, TEXT_DESCRIPTOR, PARAM1  )
#define LOG_ERROR2_DESC( ERROR_NUMBER, TEXT_DESCRIPTOR, PARAM1, PARAM2 )
#define LOG_INFO_DESC( TEXT_DESCRIPTOR )
#define LOG_INFO1_DESC( TEXT_DESCRIPTOR, PARAM1 )
#define LOG_INFO2_DESC( TEXT_DESCRIPTOR, PARAM1, PARAM2 )
#define LOG_DEBUG_DESC( TEXT_DESCRIPTOR )
#define LOG_DEBUG1_DESC( TEXT_DESCRIPTOR, PARAM1 )
#define LOG_DEBUG2_DESC( TEXT_DESCRIPTOR, PARAM1, PARAM2 )

#else /* not NO_LOGGING */

/**
 * Until I replaced the standard 8.0a GCC compiler from Symbian (2.9-psion-98r2) with the improved
 * one from the Symbian GCC improvement project (3.0-psion-98r2) available at:
 *
 *		http://www.inf.u-szeged.hu/symbian-gcc/
 *
 * I would get the following errors:
 *
 * ..\\IMAPPROCESSOR\\Imapprocessor.cpp:261: Internal compiler error 243.
 * ..\\IMAPPROCESSOR\\Imapprocessor.cpp:261: Please submit a Problem Report to Cygnus Solutions with send-pr.
 * make[1]: *** [..\..\..\..\SYMBIAN\8.0A\S60_2ND_FP2\EPOC32\BUILD\CACTUSJACK\IMAPCLIENT\SYMBIAN\LIBRARY\ECOMPLUSLITE\THUMB\UREL\IMAPPROCESSOR.o] Error 1
 *
 * These are fixed by the new gcc compiler (as well as producing an 8% reduction in code size).
 *
 */
#define __USE_DEPRECATED_L_INSTEAD_OF_LIT__

#ifndef __USE_DEPRECATED_L_INSTEAD_OF_LIT__

#define UNIQUE_LIT_MAKER( WIDE_TEXT ) _LIT( K##__LINE__, WIDE_TEXT )
#define UNIQUE_LIT_NAME( WIDE_TEXT ) K##__LINE__

#else /* not __USE_DEPRECATED_L_INSTEAD_OF_LIT__ */

#define UNIQUE_LIT_MAKER( WIDE_TEXT )
#define UNIQUE_LIT_NAME( WIDE_TEXT )  _L( WIDE_TEXT )

#endif /* __USE_DEPRECATED_L_INSTEAD_OF_LIT__ */


/**
 * DON'T USE THESE!
 * 
 * Use more convenient LOG_ERROR and LOG_ERROR_DESC macros below.  These are for the implementation of those below.
 */ 
#define LOG_DESC( ERROR_LEVEL, ERROR_NUMBER, TEXT_DESCRIPTOR )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), TEXT_DESCRIPTOR ); \
	CleanupStack::PopAndDestroy(l); }

#define LOG_DESC1( ERROR_LEVEL, ERROR_NUMBER, TEXT_DESCRIPTOR, PARAM1  )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), TEXT_DESCRIPTOR, (PARAM1) ); \
	CleanupStack::PopAndDestroy(l); }

#define LOG_DESC2( ERROR_LEVEL, ERROR_NUMBER, TEXT_DESCRIPTOR, PARAM1, PARAM2 )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), TEXT_DESCRIPTOR, (PARAM1), (PARAM2) ); \
	CleanupStack::PopAndDestroy(l); }

#define LOG_LIT( ERROR_LEVEL, ERROR_NUMBER, STRING_LITERAL )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	UNIQUE_LIT_MAKER( STRING_LITERAL ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), UNIQUE_LIT_NAME(STRING_LITERAL) ); \
	CleanupStack::PopAndDestroy(l); }

#define LOG_LIT1( ERROR_LEVEL, ERROR_NUMBER, STRING_LITERAL, PARAM1  )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	UNIQUE_LIT_MAKER( STRING_LITERAL ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), UNIQUE_LIT_NAME(STRING_LITERAL), (PARAM1) ); \
	CleanupStack::PopAndDestroy(l); }

#define LOG_LIT2( ERROR_LEVEL, ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )	if( iProperties ) { MLogger * l = GoL( iProperties, KPropertyObjectLogger, MLogger ); \
	CleanupReleasePushL( *l ); \
	UNIQUE_LIT_MAKER( STRING_LITERAL ); \
	l->Log( (ERROR_LEVEL), (ERROR_NUMBER), UNIQUE_LIT_NAME(STRING_LITERAL), (PARAM1), (PARAM2) ); \
	CleanupStack::PopAndDestroy(l); }



/**
 * Use the LOG_ERROR macros with a statement of the form:
 *
 * 		LOG_ERROR( error, "CSmtpMailFromStateChangePlugin, KStateSmtpMailWaitingToWriteMore", aSomeIntValue );
 *
 * The macro will automatically generate a _LIT for you for the text you specify as the 2 parameter.
 *
 * Note that the underlying MLogger implementation will past the additional aSomeIntValue -- we dont
 * support format strings as it is too prone to panic errors at runtime if you get the format specifiers wrong.
 * 
 */
#define LOG_TRACE( STRING_LITERAL )									LOG_LIT( MLogger::ETrace, 0, STRING_LITERAL )
#define LOG_TRACE1( STRING_LITERAL, PARAM1 )						LOG_LIT1( MLogger::ETrace, 0, STRING_LITERAL, PARAM1 )
#define LOG_TRACE2( STRING_LITERAL, PARAM1, PARAM2 )				LOG_LIT2( MLogger::ETrace, 0, STRING_LITERAL, PARAM1, PARAM2 )

#define LOG_DEBUG( STRING_LITERAL )									LOG_LIT( MLogger::EDebug, 0, STRING_LITERAL )
#define LOG_DEBUG1( STRING_LITERAL, PARAM1 )						LOG_LIT1( MLogger::EDebug, 0, STRING_LITERAL, PARAM1 )
#define LOG_DEBUG2( STRING_LITERAL, PARAM1, PARAM2 )				LOG_LIT2( MLogger::EDebug, 0, STRING_LITERAL, PARAM1, PARAM2 )

#define LOG_INFO( STRING_LITERAL )									LOG_LIT( MLogger::EInfo, 0, STRING_LITERAL )
#define LOG_INFO1( STRING_LITERAL, PARAM1 )							LOG_LIT1( MLogger::EInfo, 0, STRING_LITERAL, PARAM1 )
#define LOG_INFO2( STRING_LITERAL, PARAM1, PARAM2 )					LOG_LIT2( MLogger::EInfo, 0, STRING_LITERAL, PARAM1, PARAM2 )

#define LOG_WARN( ERROR_NUMBER, STRING_LITERAL )					LOG_LIT( MLogger::EWarn, ERROR_NUMBER, STRING_LITERAL )
#define LOG_WARN1( ERROR_NUMBER, STRING_LITERAL, PARAM1  )			LOG_LIT1( MLogger::EWarn, ERROR_NUMBER, STRING_LITERAL, PARAM1 )
#define LOG_WARN2( ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )	LOG_LIT2( MLogger::EWarn, ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )

#define LOG_ERROR( ERROR_NUMBER, STRING_LITERAL )					LOG_LIT( MLogger::EError, ERROR_NUMBER, STRING_LITERAL )
#define LOG_ERROR1( ERROR_NUMBER, STRING_LITERAL, PARAM1  )			LOG_LIT1( MLogger::EError, ERROR_NUMBER, STRING_LITERAL, PARAM1 )
#define LOG_ERROR2( ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )	LOG_LIT2( MLogger::EError, ERROR_NUMBER, STRING_LITERAL, PARAM1, PARAM2 )



#endif /* NO_LOGGING */



#endif /* __DEBUG_UTILITIES_H__ */

