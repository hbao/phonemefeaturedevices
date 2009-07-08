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


#ifndef __LOGGER_IMPL_H__
#define __LOGGER_IMPL_H__

#include "Logger.h"
#include <EcomPlusRefCountedBase.h>
#include <e32svr.h>
#include "MessagingDataAccessConnection.h"
#include <flogger.h>

class CLogger : public CEComPlusRefCountedBase, public MLogger
{
public:
	static MLogger * NewL(TAny * aConstructionParameters);

public: // MUnknown
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	virtual void AddRef() { /* RDebug::Print(_L("CLogger::AddRef")); */ CEComPlusRefCountedBase::AddRef(); }
	virtual void Release() { /* RDebug::Print(_L("CLogger::Release")); */ CEComPlusRefCountedBase::Release(); }

public: // MInitialize
	//virtual void InitializeL(MProperties * aInitializationProperties);

public: // MLogger
	virtual void SetLoggingLevel(TLoggingLevel aLoggingLevel);

public:
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage );
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt );
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt1, TInt aInt2 );
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC & aString );
	virtual void Log( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC8 & aString );
protected:
	virtual void LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, TInt aInt1, TInt aInt2, TInt aIntsToUse );
	virtual void LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC & aString );
	virtual void LogL( TLoggingLevel aLoggingLevel, TInt aErrorNumber, const TDesC & aMessage, const TDesC8 & aString );


protected:
	CLogger(TAny * aConstructionParameters);
	void ConstructL();
	virtual ~CLogger();

	void CreateLogEntryL(TLoggingLevel aLoggingLevel, TInt aError, HBufC * aTextBuffer );

	TLoggingLevel iLoggingLevel;

	RFileLogger iFileLogger;
};


#endif /* __LOGGER_IMPL_H__ */
