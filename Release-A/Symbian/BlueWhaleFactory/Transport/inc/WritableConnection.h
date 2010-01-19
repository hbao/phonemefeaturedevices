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


#ifndef __WRITABLE_CONNECTION_H__
#define __WRITABLE_CONNECTION_H__

#include "Connection.h"



const TInt KIID_MWritableConnection = 0x1000016;
const TInt KIID_MServerConnection = 0x1000017;


/**
 * Key this instance can usually be found in MProperties.
 */
const TObjectUniqueKey KPropertyObjectImapWritableConnection = {KIID_MWritableConnection, 0x0345234};
const TObjectUniqueKey KPropertyObjectVMWritableConnection = {KIID_MWritableConnection, 0x0345235};
const TObjectUniqueKey KPropertyObjectSmtpWritableConnection = {KIID_MWritableConnection, 0x0545231};


class MWritableConnection : public MConnection
{
public: 
	/**
	 * Do not attempt to write more than this value at once.
	 */
	virtual TInt RemainingWriteBufferSpace() = 0;

	/**
	 * Attempting to write more than RemainingWriteBufferSpace() will Leave.
	 */
	virtual void WriteL(const TDesC8& aData) = 0;

	/**
	 * socket options
	 */
	virtual TInt SetOption(TUint aOption,TUint aVal) = 0;
	
protected:
	virtual ~MWritableConnection() {}
};

class MServerConnection : public MConnection
{
protected:
	virtual ~MServerConnection(){}
};

/**
 * Some state machine code may need to write a lot of data to an MWritableConnection.
 *
 * It should write no more than RemainingWriteBufferSpace() each time, then
 * wait for the command below to be invoked to write again.
 */
const MStateMachine::TCommand KCommandStateMachineWriteCompleted = {KIID_MWritableConnection, 0x0745237};






#endif /* __WRITABLE_CONNECTION_H__ */
