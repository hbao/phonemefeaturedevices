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

 
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "Initialize.h"
#include <unknown.h>

const TInt KCID_MLogger = 0x1020806A;

const TInt KIID_MLogger = 0x1000022;

const TObjectUniqueKey KPropertyObjectLogger = { KCID_MLogger, 0x17e100};


class MLogger : public MUnknown
{
public:

	enum TLoggingLevel
	{
		/**
		 * Log everything. 
		 */
		EAll = 0, 

		/**
		 * Finer-grained informational events than the DEBUG. 
		 */
		ETrace = 10,
      
		/**
		 * Fine-grained informational events that are most useful to debug an application. 
		 */
		EDebug = 20,

		/**
		 * Informational messages that highlight the progress of the application at coarse-grained level. 
		 */
		EInfo = 30,

		/**
		 * Potentially harmful situations. 
		 * 
		 * We have made the decision that only ERROR level or worse
		 * problems will be reported directly to the user.
		 * 
		 * Think carefully about whether a warning or error message actually needs to 
		 * be shown to the user or not -- if it's a transient connection issue,
		 * don't show them an error, just try again later. 
		 */
		EWarn = 40,
    
		/**
		 * Events that might still allow the application to continue running. 
		 * 
		 * Think carefully about whether a warning or error message actually needs to 
		 * be shown to the user or not -- if it's a transient connection issue,
		 * don't show them an error, just try again later. 
		 */
		EError = 50,

		/**
		 * Very severe error events that will presumably lead the application to abort. 
		 */
		EFatal = 60, 

		/**
		 * The OFF has the highest possible rank and is intended to turn off logging. 
		 */
		 EOff = 70
	};

	virtual void SetLoggingLevel(TLoggingLevel aLoggingLevel) = 0;

	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage ) = 0;
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt ) = 0;
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt1, TInt aInt2 ) = 0;
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC & aString ) = 0;
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC8 & aString ) = 0;

protected:
	virtual ~MLogger() {}
};


#endif /* __LOGGER_H__ */
