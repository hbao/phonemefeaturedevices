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



#ifndef __LOGGER_MESSAGE_H__
#define __LOGGER_MESSAGE_H__

#include "Message.h"
#include "MessageAttributes.h"


const TInt KIID_MLoggerMessage = 0x01000023;


const TInt KCID_MLoggerMessage = 0x1020DFE3;


/**
 * Our interface to represent a message.
 */
class MLoggerMessage : public MUnknown
{
public:
	/**
	 * A bitmap used to allow for up to 32 different logging domains.
	 */
	virtual void SetDomain(TUint32 aDomain) = 0;
	virtual TUint32 Domain() = 0;
	
	/**
	 * How critical is the log entry?  
	 *
	 * e.g.		0 => Error
	 *			1 => Info
	 *			2 => Debug
	 */
	virtual void SetLevel(TInt aLoggingLevel) = 0;
	virtual TInt Level() = 0;

	virtual void SetError( TInt aError ) = 0;
	virtual TInt Error() const = 0; 

	/**
	 * Makes a copy.
	 */
	virtual void SetTextL( const TDesC & aText ) = 0;

	/**
	 * Takes ownership.
	 */
	virtual void SetTextL( HBufC * aTextBuffer ) = 0;

	virtual const TDesC & TextL() const = 0;

protected:
	virtual ~MLoggerMessage() {}
};


#endif /* __LOGGER_MESSAGE_H__ */
