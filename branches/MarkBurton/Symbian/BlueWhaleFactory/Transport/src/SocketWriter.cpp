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


#include "SocketWriter.h"
#include "TimeOutTimer.h"
#include "SocketEngineImpl.h"

const TInt CSocketWriter::KTimeIntervalSecondsTimeOut = 60;

	
CSocketWriter* CSocketWriter::NewL(CSocketEngine * aSocketEngine)
{
	CSocketWriter* self = new (ELeave) CSocketWriter(aSocketEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}


CSocketWriter::CSocketWriter(CSocketEngine * aSocketEngine)
: CActive(EPriorityStandard)
, iSocketEngine(aSocketEngine)
{
	DEBUG_CONSTRUCTOR
}


void CSocketWriter::InitializeL(MProperties *aInitializationProperties)
{
	iProperties = aInitializationProperties;
	
	// Not owned -- never AddRef on something which may own you.
	//iProperties->AddRef();
}


CSocketWriter::~CSocketWriter()
{
	DEBUG_DESTRUCTOR

    Reset();
    if( iTimer )
	{
		iTimer->Release();
		iTimer = 0;
	}

	// Not owned so don't cleanup.
	//if( iProperties )
	//{
	//	iProperties->Release();
	//	iProperties = 0;
    //}

}

void CSocketWriter::DoCancel()
{	
    // Cancel asynchronous write request
	if( iSocket )
	{
		iSocket->CancelWrite();
	}
	iWriteStatus = EWaiting;
}

void CSocketWriter::ConstructL()
{
	CActiveScheduler::Add(this);

	iTimeOut = KTimeIntervalSecondsTimeOut; 
	iTimer = DiL( MTimeOutTimer );
	iTimer->SetCallback( this );

	iWriteStatus = EWaiting;
}

void CSocketWriter::TimeOutTimerNotify( TAny * /* aCookie */ )
{
	Reset();
	iWriteStatus = EWaiting;

	LOG_ERROR( 0, "CSocketWriter::TimerExpired" );

    iSocketEngine->ReportError(MConnectionCallback::EErrorTimeOutOnWrite, KErrTimedOut);
}

void CSocketWriter::RunL()
{
    iTimer->Reset();

	TInt status = iStatus.Int();

	if( KErrNone == status )
	{
		switch(iWriteStatus)
		{
		case ESending:
		// Character has been written to socket
            SendNextPacket();
	        TRAPD( ignoreError, iSocketEngine->iCallback->CallbackCommandL(KCommandStateMachineWriteCompleted) );
			break;
		default:
			LOG_ERROR( 0, "CSocketWriter::RunL bad iWriteStatus" );
            break;
	    };
	}
	else if( KErrCancel == status )
	{
		LOG_ERROR( status, "CSocketWriter::RunL KErrCancel" );
	}
    else 
    {
		LOG_ERROR( status, "CSocketWriter::RunL" );

        iSocketEngine->ReportError(MConnectionCallback::EErrorGeneralWrite, status);
        iWriteStatus = EWaiting;
	}
}

void CSocketWriter::IssueWriteL(const TDesC8& aData)
{
	if ((iWriteStatus != EWaiting) && (iWriteStatus != ESending))
	{
		User::Leave(KErrNotReady);
	}

	if ((aData.Length() + iTransferBuffer.Length()) > iTransferBuffer.MaxLength())
	{
		// Not enough space in buffer
		User::Leave(KErrOverflow);
	}

#ifdef __WINS__
	//DebugPrint(0, _L("CSocketWriter::IssueWriteL BEGIN REQUEST"));
	//DebugPrint(0, aData);
	//DebugPrint(0, _L("CSocketWriter::IssueWriteL END REQUEST"));
#endif /* __WINS__ */

	//LOG_DEBUG1("TX",aData);
	// Add new data to buffer
	iTransferBuffer.Append(aData);

	if (!IsActive())
	{
		SendNextPacket();
	}
}

TInt CSocketWriter::RemainingWriteBufferSpace()
{
	return ( iTransferBuffer.MaxLength() - iTransferBuffer.Length() );
}


void CSocketWriter::SendNextPacket()
{
	if (iTransferBuffer.Length() > 0)
    {
		// Move data from transfer buffer to actual write buffer
		iWriteBuffer = iTransferBuffer;
		iTransferBuffer.Zero();

		iSocket->Write(iWriteBuffer, iStatus); // Initiate actual write

        // Request timeout
	    TRAPD( afterError, iTimer->AfterL(iTimeOut) );
		if( KErrNone == afterError )
		{
			SetActive();
			iWriteStatus = ESending;
		}
		else
		{
	        iSocketEngine->ReportError(MConnectionCallback::EErrorGeneralWrite, afterError);
		}
    }
    else
    {
	    iWriteStatus = EWaiting;
    }
}



void CSocketWriter::Reset()
{
	Cancel();
	if( iTimer )
	{
	    iTimer->Reset();
	}
}
