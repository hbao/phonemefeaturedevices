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

 
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "StateMachine.h"


const TInt KMaxServerNameLength = 256;	// TODO: What should this really be?
const TInt KMaxMailboxNameLength = KMaxServerNameLength;




const TInt KIID_MConnectionCallback = 0x1000017;



class MConnectionCallback : public MStateMachineCallback
    {
public:

	/**
	 *
	 * @discussion Error types encountered when reading/writing to a sockets
  	 * @value EDisconnected A disconnected error has been encountered
  	 * @value ETimeOutOnWrite A write operation has failed to complete within a predetermined period
  	 * @value EGeneralReadError A general error has been encountered during a read
  	 * @value EGeneralWriteError A general error has been encountered during a write
  	 */
    enum TErrorType
        {
		EErrorDNSLookup,
		EErrorConnecting,
        EErrorDisconnected,
        EErrorTimeOutOnWrite,
        EErrorGeneralRead,
        EErrorGeneralWrite
        };

	/**
	 * @discussion Report a communication error
  	 * @param aErrorType error type
  	 * @param aErrorCode associated error code
  	 */
	virtual void ReportError(TErrorType aErrorType, TInt aErrorCode) = 0;

protected:
	virtual ~MConnectionCallback() {}
};





const TInt KIID_MConnection = 0x1000015;


const MStateMachine::TCommand KCommandConnectionConnectSecurely = {KIID_MConnection, 0x10};
const MStateMachine::TCommand KCommandConnectionConnectWithoutGoingSecure = {KIID_MConnection, 0x20};
const MStateMachine::TCommand KCommandConnectionResolveHost = {KIID_MConnection, 0x25};
const MStateMachine::TCommand KCommandConnectionServerListen = {KIID_MConnection, 0x27};
const MStateMachine::TCommand KCommandConnectionDisconnect = {KIID_MConnection, 0x40};



/**
 * An interface which defines the basic properties of a connection.
 */
class MConnection : public MStateMachine
{
public: 
	/** 
	 * Result is ETrue if socket is disconnected.
	 *
	 * Using 'Connected' is not good as a flag, because it could
	 * be in a state of connecting.
	 */
    virtual TBool Disconnected() const = 0;
protected:
	virtual ~MConnection() {}
};





#endif /* __CONNECTION_H__ */
