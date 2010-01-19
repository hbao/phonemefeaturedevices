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


#ifndef __SOCKET_READER_H__
#define __SOCKET_READER_H__

#include <in_sock.h>
#include "Properties.h"
#include <OSVersion.h>

class CSocketEngine;


class CSocketReader : public CActive
    {
public:
	static CSocketReader* NewL(CSocketEngine * aSocketEngine);
	~CSocketReader();

	void InitializeL(MProperties *aInitializationProperties);

	/**
	 * On Series 60 v12, this is all there is to set.
	 */
	void SetSocket( RSocket * aSocket )
	{
		Cancel();
		iSocket = aSocket;
	}

	/**
	 * Initiate a read from the socket.
	 */
	void Start();
	void Reset();


protected: // from CActive

	void DoCancel();
	void RunL();	

private:

	CSocketReader(CSocketEngine * aSocketEngine);
	void ConstructL();

	/**
	 * Initiates a read from the socket.
	 */
	void IssueRead();

private: // Member variables

	RSocket * iSocket;

	// A non-owned pointer to our parent object.
	CSocketEngine * iSocketEngine;

    // Pointer to buffer for receiving data.
	TPtr8 iBuffer;

    // Length of data read is written here.
    TSockXfrLength          iDummyLength;

	MProperties * iProperties;	// Not owned.
};

#endif /* __SOCKET_READER_H__ */

