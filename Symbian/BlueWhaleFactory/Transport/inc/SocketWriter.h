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


#ifndef __SOCKET_WRITER_H__
#define __SOCKET_WRITER_H__

#include <in_sock.h>
#include <OSVersion.h>

#include "TimeOutTimer.h"
#include "Properties.h"

class CTimeOutTimer;
class CSocketEngine;

// TODO: Candidate for our CBaseStateMachine class.
class CSocketWriter : public CActive, public MTimeOutTimerCallback
    {
public:
    static CSocketWriter* NewL(CSocketEngine * aSocketEngine);
	~CSocketWriter();

	void InitializeL(MProperties *aInitializationProperties);


	/**
	 * On V12, this is all there is to set.
	 */
	void SetSocket( RSocket * aSocket )
	{
		Cancel();
		iSocket = aSocket;
	}

    void IssueWriteL(const TDesC8& aData);
	TInt RemainingWriteBufferSpace();

	void Reset();

public: // From MTimeOutTimerCallback
	virtual void TimeOutTimerNotify( TAny * aCookie );

protected: // from CActive
	void DoCancel();
	void RunL();	

private:
	CSocketWriter(CSocketEngine * aSocketEngine);
	void ConstructL();

    void SendNextPacket();

private:

    enum {KWriteBufferSize = 3000};

    static const TInt KTimeIntervalSecondsTimeOut;

	enum TWriteState 
	    {
		ESending, 
        EWaiting
	    };

    RSocket *                iSocket;

	// A non-owned pointer to our parent object.
	CSocketEngine * iSocketEngine;

    // Accumulate data to send in here.
	TBuf8<KWriteBufferSize> iTransferBuffer;

    // Holds data currently being sent to socket.
	TBuf8<KWriteBufferSize> iWriteBuffer;

    // A timer used to cancel an outstanding write after a predefined timeout.
	MTimeOutTimer*          iTimer;

    // The timeout in seconds to use.
	TTimeIntervalSeconds    iTimeOut;

    // The state of this Active Object.
	TWriteState             iWriteStatus;

	MProperties * iProperties;	// Not owned.

    };

#endif /* __SOCKET_WRITER_H__ */

