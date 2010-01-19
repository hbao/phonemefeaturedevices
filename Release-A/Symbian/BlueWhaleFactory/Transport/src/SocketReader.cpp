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


#include "SocketReader.h"
#include "SocketEngineImpl.h"

CSocketReader* CSocketReader::NewL(CSocketEngine * aSocketEngine)
{
	CSocketReader* self = new (ELeave) CSocketReader(aSocketEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}



CSocketReader::CSocketReader(CSocketEngine * aSocketEngine)
: CActive(EPriorityStandard)
, iSocketEngine(aSocketEngine)
, iBuffer(0,0)
{
	DEBUG_CONSTRUCTOR
}


CSocketReader::~CSocketReader()
{
	DEBUG_DESTRUCTOR

    Reset();

	// Not owned so don't cleanup.
	//if( iProperties )
	//{
	//	iProperties->Release();
	//	iProperties = 0;
    //}
}

void CSocketReader::ConstructL()
{
	CActiveScheduler::Add(this);
}

void CSocketReader::InitializeL(MProperties *aInitializationProperties)
{
	iProperties = aInitializationProperties;
	
	// Not owned -- never AddRef on something which may own you.
	//iProperties->AddRef();

	// Retrieve and save a pointer to the read buffer we are to use.
	HBufC8 * buffer = iProperties->GetStringBuffer8L(KPropertyString8ConnectionReadBuffer);
	iBuffer.Set( buffer->Des() );
	iBuffer.SetLength( buffer->Size() );
}


void CSocketReader::DoCancel()
{
    // Cancel asynchronous read request.
	if( iSocket )
	{
		iSocket->CancelRead();
	}
}


void CSocketReader::RunL()
{
	// Active object request complete handler

	TInt status = iStatus.Int();

	switch(status)
    {
    case KErrNone:

#ifdef __WINS__
//		DebugPrint(0, _L("CSocketReader::RunL BEGIN RESPONSE"));
//		DebugPrint(0, iBuffer);
//		DebugPrint(0, _L("CSocketReader::RunL END RESPONSE"));
#endif /* __WINS__ */


        // Character has been read from socket
        TRAPD( err, iSocketEngine->iCallback->CallbackCommandL(KCommandStateMachineAcceptReadBuffer) );
		if( KErrNone == err )
		{
			// If we connect but there's a security policy in place
			// We may receive (e.g. for IMAP) a "BYE Access Prohibited" message
			// and the connection may immediately shut down.
			// In that case, this IssueRead will fail.
			// Parsing code should shut down the connection whevener the 
			// appropriate equivalent to BYE is received.
			// For this reason, we check to make sure we are still Connected().
			
			// Immediately start another read.
			IssueRead(); 
		}
		else if (err == KErrNoMemory)
		{
			// failed when trying to increase the buffer size to contain incoming data (ie overflow)
			// try again - it will succeed eventually when data is read and removed from the buffer
			LOG_ERROR( err, "CSocketReader::RunL -- overflow" );
			iStatus = KRequestPending;
			TRequestStatus* status = &iStatus;
			SetActive();
			User::RequestComplete(status, KErrNone);
		}
		else
		{
			LOG_ERROR( err, "CSocketReader::RunL -- error calling callback" );
		}

        break;
    case KErrDisconnected:

		LOG_ERROR( status, "CSocketReader::RunL -- KErrDisconnected" );

        iSocketEngine->ReportError(MConnectionCallback::EErrorDisconnected, status);
        break;
	case KErrCancel:
		LOG_INFO( "CSocketReader::RunL -- KErrCancel" );
		break;
    default:

		LOG_ERROR( status, "CSocketReader::RunL" );

        iSocketEngine->ReportError(MConnectionCallback::EErrorGeneralRead, status);
        break;
    }	
}

void CSocketReader::IssueRead()
{
	if( iSocketEngine->ReadyForData() )
	{
		// Initiate a new read from socket into iBuffer

		if( IsActive() )
		{
			LOG_ERROR( KErrBadState, "CSocketReader::IssueRead -- already active" );
		}

		iBuffer.FillZ();
		iBuffer.Zero();
		iSocket->RecvOneOrMore(iBuffer, 0, iStatus, iDummyLength);
		SetActive();
	}
}

void CSocketReader::Start()
{
    // Initiate a new read from socket into iBuffer
    if (!IsActive())
    {
        IssueRead();
    }
}


void CSocketReader::Reset()
{
	Cancel();
}
