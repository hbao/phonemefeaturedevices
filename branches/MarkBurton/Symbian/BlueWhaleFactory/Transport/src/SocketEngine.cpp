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


#include <eikgted.h>
#include <es_enum.h>
#include "SocketEngineImpl.h"
#include "TimeOutTimer.h"
#include "SocketReader.h"
#include "SocketWriter.h"
#include "DebugUtilities.h"
#include "ConnectionManager.h"
#include <OSVersion.h>

#ifdef _DEBUG_SOCKETENGINE_
#define DEBUGPRINT(_XX) RDebug::Print(_XX,);
#define DEBUGPRINT1(_XX,_YY) RDebug::Print(_XX,_YY);
#else
#define DEBUGPRINT(_XX)
#define DEBUGPRINT1(_XX,_YY)
#endif

#if __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
#define __USE_CONNECTION_SHARING__
#endif

// TODO: Not sure if such a long time out will be needed when there's no "bad certificate use anyway dialog?".
const TInt CSocketEngine::KTimeIntervalSecondsTimeOut = 60; // 60 seconds time-out -- longer needed because of bad certificate dialog.


#define TRY_SYNCHRONOUS
	

MWritableConnection* CSocketEngine::NewL( TAny * aConstructionParameters )
{
	CSocketEngine* self = new (ELeave) CSocketEngine(aConstructionParameters);
	CleanupStack::PushL(self);
	DEBUGPRINT1(_L("CSocketEngine::NewL 0x%08x"),self);
	self->ConstructL();

	MWritableConnection * writableConnection = QiL( self, MWritableConnection );
	CleanupStack::Pop(self);
	return writableConnection;
}


CSocketEngine::CSocketEngine(TAny * aConstructionParameters )
	: CBaseStateMachine<CEComPlusRefCountedActive>(aConstructionParameters)
{
	DEBUG_CONSTRUCTOR
}

void CSocketEngine::DoReset()
{
	DEBUGPRINT( _L("CSocketEngine::DoReset") );

	// Cache this for below as CBaseStateMachine::Reset will clobber it.
	const TState state = State();

	CBaseStateMachine<CEComPlusRefCountedActive>::Reset();

	if( iTimer )
	{
	    iTimer->Reset();
	}

	if( iSocketReader )
	{
	    iSocketReader->Reset();
	}
	
	if( iSocketWriter )
	{
	    iSocketWriter->Reset();
	}

	if( (KStateSocketEngineRaisingConnection == state) || (KStateSocketEngineErrorRaisingConnection == state) || (KStateSocketEngineCancellingRaisingConnection == state) )
	{
		HandleResetWhileRaisingConnection(state);
	}
	else if( (KStateSocketEngineResolvingServerName == state) || (KStateSocketEngineErrorResolvingServerName == state) || (KStateSocketEngineCancellingResolvingServerName == state) )
	{
		HandleResetWhileResolvingServerName(state);
	}
	else if( (KStateSocketEngineConnectingSocket == state) || (KStateSocketEngineErrorConnectingSocket == state) || (KStateSocketEngineCancellingConnectingSocket == state) )
    {
    	HandleResetWhileConnectingSocket(state);
	}
	else if ( state == KStateSocketEngineConnectedAcceptConnection || state == KStateSocketEngineRaisingAcceptConnection)
	{
		iAcceptSocket.CancelAccept();
		iAcceptSocket.Close();
	}
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	// I wasn't able to figure out what all needed to be cancelled or
	// closed to properly reset this CSecureSocket, so in the interests
	// of expediency, I simply delete it and re-create it as needed.
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__


	iSocket.Close();

	iRaisingConnectionRetry = 0;
}


void CSocketEngine::RunL()
{

	const TState state = State();

	const TInt status = iStatus.Int();

	// We completed in time.
	iTimer->Reset();

	if( KErrCancel == status )
	{
		HandleKErrCancelInRunL(state);
	}
	else
	{
		if( KStateSocketEngineRaisingConnection == state )
		{
			HandleRunLWhileRaisingConnectionL(status);
		}
		else if(KStateSocketEngineRaisingConnectionNoOpen == state)
		{
			HandleRunLWhileRaisingConnectionNoOpenL(status);
		}
		else if( KStateSocketEngineResolvingServerName == state )
		{
			HandleRunLWhileResolvingServerNameL(status);
		}
		else if( KStateSocketEngineResolvingServerNameNoConnect == state)
		{
			HandleRunLWhileResolvingServerNameNoConnectL(status);
		}
		else if( KStateSocketEngineConnectingSocket == state )
		{
			HandleRunLWhileConnectingSocketL(status);
		}
		else if (KStateSocketEngineRaisingAcceptConnection == state)
		{
			HandleRunLWhileListening(status);
		}
		else
		{
			User::Leave(KErrBadState);
		}
	}
}




TInt CSocketEngine::RunError(TInt aError)
{
	if( KErrNone != aError )
	{
		// Ensure we are in a consistent state.
		Reset();

		LOG_ERROR2( aError, "CSocketEngine::RunError, state now", (TInt) State().High(), (TInt) State().Low() );

		iCallback->ReportError(MConnectionCallback::EErrorConnecting, aError);

	}

	// We have handled the Leave, so return KErrNone otherwise
	// the CActiveScheduler's Error() method will get called.
	return KErrNone;
}





MUnknown * CSocketEngine::QueryInterfaceL( TInt aInterfaceId)
{
	if( KIID_MWritableConnection == aInterfaceId )
	{
		AddRef();
		return static_cast<MWritableConnection*>(this);
	}
	else if( KIID_MConnection == aInterfaceId )
	{
		AddRef();
		return static_cast<MWritableConnection*>(this);
	}
	else if( KIID_MResolvedConnection == aInterfaceId )
	{
		AddRef();
		return static_cast<MResolvedConnection*>(this);
	}
	else if( KIID_MServerConnection == aInterfaceId )
		{
			AddRef();
			return static_cast<MServerConnection*>(this);
		}
	else
	{
		return CBaseStateMachine<CEComPlusRefCountedActive>::QueryInterfaceL(aInterfaceId);
	}
}


const TInt KNumServerSlotsNeeded = 24;

void CSocketEngine::ConstructL()
{
	CBaseStateMachine<CEComPlusRefCountedActive>::ConstructL();

	// Start a timer
	iTimer = DiL( MTimeOutTimer );
	iTimer->SetCallback( this );

	// Add ourselves to the ActiveScheduler.
	CActiveScheduler::Add(this); 

	// Open channel to Socket Server
	User::LeaveIfError(iSocketServ.Connect(/* KNumServerSlotsNeeded */));


    // Create socket read and write active objects.
	iSocketReader = CSocketReader::NewL(this);
	iSocketWriter = CSocketWriter::NewL(this);

}



CSocketEngine::~CSocketEngine()
{
	DEBUG_DESTRUCTOR

	iDeleting = ETrue;

	Reset(); // If IsActive calls Cancel which calls DoCancel which calls DoReset else calls DoReset.

    delete iSocketReader;
    iSocketReader = 0;

    delete iSocketWriter;
    iSocketWriter = 0;

    if( iTimer )
	{
		iTimer->Release();
		iTimer = 0;
	}

    if(iConnection)
    {
    	iConnection->Release();
    	iConnection = NULL;
    }

#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)


	// Moved to Reset.
	//if( iTlsSocket )
	//{
	//	delete iTlsSocket;
	//	iTlsSocket = 0;
	//}

#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

	//User::Heap().Check();

	if( iInitiator )
	{
		iInitiator->Cancel();
		delete iInitiator;
		iInitiator = 0;
	}

	//User::Heap().Check();

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__


    iSocketServ.Close();
}


void CSocketEngine::Reset()
{
	DEBUGPRINT( _L( "CSocketEngine::Reset") );

	if( IsActive() )
	{
		Cancel();	// Calls DoCancel which calls DoReset.
	}
	else
	{
		DoReset();
	}
}


void CSocketEngine::DoCancel()
{
	DEBUGPRINT( _L("CSocketEngine::DoCancel") );
	DoReset();
}


void CSocketEngine::ListenOnLocalSockectL()
{
	
	LOG_INFO( "CSocketEngine::ListenOnLocalSockectL" ); 
	
	const TState state = State();
	if( KStateInitial == state )
	{
		LOG_DEBUG( "CSocketEngine::ListenOnLocalSockectL proceeding" ); 
		SetState(KStateSocketEngineRaisingAcceptConnection);
		User::LeaveIfError(iSocket.Open(iSocketServ));
		User::LeaveIfError(iAcceptSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));

		// Set up address information
		iAddress.SetPort(iProperties->GetIntL(KPropertyIntSocketPort));

		// Initiate socket connection.
		User::LeaveIfError(iAcceptSocket.Bind(iAddress));
		User::LeaveIfError(iAcceptSocket.Listen(1));
		iAcceptSocket.Accept(iSocket,iStatus);
		SetActive();
	}
}
void CSocketEngine::StartConnectionL()
{
	TInt iapUid = 0;
	TRAPD(iapError, iapUid = iProperties->GetIntL(KPropertyIntSocketConnectionIap));

#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	iConnection->Start(iStatus);
	// Request time out
	iTimer->AfterL(KTimeIntervalSecondsTimeOut);
	SetActive();

/*	if(iapUid != 0)
	{
		iCommDbConnPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt); 
	}
	else
	{
		iCommDbConnPref.SetDialogPreference(ECommDbDialogPrefPrompt); 
	}
	iCommDbConnPref.SetIapId( iapUid ); 

	iCommDbConnPref.SetDirection(ECommDbConnectionDirectionOutgoing); 

	iConnection.Start(iCommDbConnPref, iStatus);
	
	// Request time out
	iTimer->AfterL(KTimeIntervalSecondsTimeOut);

	SetActive();
*/
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

	iConnPref.iRanking = 1; 
	iConnPref.iDirection = ECommDbConnectionDirectionOutgoing; 
	iConnPref.iDialogPref = ECommDbDialogPrefDoNotPrompt; 
	CCommsDbConnectionPrefTableView::TCommDbIapBearer bearer; 
	bearer.iBearerSet = KMaxTUint32;
	bearer.iIapId = iapUid; 
	iConnPref.iBearer = bearer;
	
	if( ! iInitiator )
	{
		// This may have been deleted if there was a Reset, as according to 
		// the Forum Nokia IAPConnect example, it must be re-created to reset properly.
		iInitiator = CIntConnectionInitiator::NewL();
	}
	
#ifdef TRY_SYNCHRONOUS
	iStatus = KErrNone;	// Note: This doesn't work if you pass KRequestPending (as you should).
	iInitiator->ConnectL(iConnPref, iStatus);
	User::WaitForRequest(iStatus);
	TInt status = iStatus.Int();
	if(    KErrNone == status 
		|| KConnectionPref1Exists == status
		|| KConnectionPref2Exists == status
		|| KConnectionExists == status
		|| KConnectionPref1Created == status
		|| KConnectionPref2Created == status
		|| KConnectionCreated == status
		)
	{
		SetState(KStateSocketEngineConnectionRaised);
		CheckDNSAndOpenSocketL();
	}
	else if( KErrAccessDenied == status )
	{
		// According to the Forum Nokia IAPConnect example, changing the
		// IAP may fail with KErrAccessDenied on the first try. 
		// Just try again.
		iStatus = KErrNone;	// Note: This doesn't work if you pass KRequestPending (as you should).
		iInitiator->ConnectL(iConnPref, iStatus);
		User::WaitForRequest(iStatus);
		
		status = iStatus.Int();
		if(    KErrNone == status 
			|| KConnectionPref1Exists == status
			|| KConnectionPref2Exists == status
			|| KConnectionExists == status
			|| KConnectionPref1Created == status
			|| KConnectionPref2Created == status
			|| KConnectionCreated == status
			)
		{
			SetState(KStateSocketEngineConnectionRaised);
			CheckDNSAndOpenSocketL();
		}
		else
		{
			LOG_ERROR( status, "CSocketEngine::RaiseConnectionL -- Could not raise connection" );
			Reset();
		}
	}
	
#else /* not TRY_SYNCHRONOUS */
	iInitiator->ConnectL(iConnPref, iStatus);
	
	// Request time out
	iTimer->AfterL(KTimeIntervalSecondsTimeOut);
	
	SetActive();
#endif /* TRY_SYNCHRONOUS */
	
	
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
}

void CSocketEngine::SetupConnectionL()
{
	LOG_DEBUG( "CSocketEngine::SetupConnectionL");
	TBool startConnection = ETrue;
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	iConnection->RaiseConnectionL(iSocketServ);
/*	
	User::LeaveIfError( iConnection.Open(iSocketServ) );

	// this code allows connection sharing.
#ifdef __USE_CONNECTION_SHARING__
	TInt iapUid = 0;
	TConnectionInfo inf;
	TPckg<TConnectionInfo> info(inf);
	TUint count;
	if(iConnection.EnumerateConnections(count) == KErrNone)
	{
		for(TInt i=1;i<=count;i++)
		{
			if(iConnection.GetConnectionInfo(i, info)== KErrNone)
			{
				if(iConnection.Attach(info,RConnection::EAttachTypeNormal) == KErrNone)
				{
					startConnection = EFalse;
					TRequestStatus* status = &iStatus;
					User::RequestComplete(status,KErrNone);
					SetActive();
					break;
				}
			}
		}
	}
#endif 
*/	
#endif
	if(startConnection)
	{
		StartConnectionL();
	}
}

void CSocketEngine::RaiseConnectionL()
{
	//User::Heap().Check();
	LOG_INFO( "CSocketEngine::RaiseConnectionL" ); 
	//User::Heap().Check();

	const TState state = State();

	if( 	(KStateSocketEngineCancellingRaisingConnection == state) 	|| (KStateSocketEngineCancellingResolvingServerName == state) 
		 || (KStateSocketEngineCancellingConnectingSocket == state) 	|| (KStateSocketEngineCancellingSecuring == state) 
	 	 )
	{
		// If someone asks us to connect while we're in the middle 
		// of cancelling some asynchronous service provider, 
		// then save that state state so we'll know
		// to start raising a connection once our KErrCancel is received.
		iNewRequestToConnectIssuedWhileCancelling = ETrue;
		return;
	}
	else if( KStateInitial == state )
	{
		LOG_DEBUG( "CSocketEngine::RaiseConnectionL proceeding" ); 
		SetState(KStateSocketEngineRaisingConnection);
		SetupConnectionL();
	}
	else
	{
		LOG_ERROR2( KErrBadState, "CSocketEngine::RaiseConnectionL -- bad state", State().High(), State().Low() );
		User::Leave(KErrBadState);
	}

}

void CSocketEngine::RaiseConnection2L()
{
	//User::Heap().Check();
	LOG_INFO( "CSocketEngine::RaiseConnection2L" ); 
	//User::Heap().Check();

	const TState state = State();

	if( 	(KStateSocketEngineCancellingRaisingConnection == state) 	|| (KStateSocketEngineCancellingResolvingServerName == state) 
		 || (KStateSocketEngineCancellingConnectingSocket == state) 	|| (KStateSocketEngineCancellingSecuring == state) 
	 	 )
	{
		// If someone asks us to connect while we're in the middle 
		// of cancelling some asynchronous service provider, 
		// then save that state state so we'll know
		// to start raising a connection once our KErrCancel is received.
		iNewRequestToConnectIssuedWhileCancelling = ETrue;
		return;
	}
	else if( KStateInitial == state )
	{
		LOG_DEBUG( "CSocketEngine::RaiseConnectionL proceeding" ); 
		SetState(KStateSocketEngineRaisingConnectionNoOpen);
		SetupConnectionL();
	}
	else
	{
		LOG_ERROR2( KErrBadState, "CSocketEngine::RaiseConnection2L -- bad state", State().High(), State().Low() );
		User::Leave(KErrBadState);
	}

}

void CSocketEngine::CheckDNSL()
{
	const TState state = State();

	User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp, iConnection->Connection()));
	iServerName.Copy(iProperties->GetStringL(KPropertyStringSocketServer));
	iResolver.GetByName(iServerName, iNameEntry, iStatus);
	SetState(KStateSocketEngineResolvingServerNameNoConnect);
	// Request time out
	iTimer->AfterL(KTimeIntervalSecondsTimeOut);
	SetActive();
}

void CSocketEngine::CheckDNSAndOpenSocketL()
{
	const TState state = State();

	if( KStateSocketEngineConnectionRaised == state )
	{

		TInetAddr addr;
		iServerName.Copy(iProperties->GetStringL(KPropertyStringSocketServer));

#ifdef __WINS__
		DebugPrint(0, iServerName );
#endif /* __WINS__ */
		LOG_INFO1( "CSocketEngine::CheckDNSAndOpenSocketL -- Server name", iServerName );
		if (addr.Input(iServerName) == KErrNone)
		{
			// Server name is already a valid ip address -- use it.
			SetState(KStateSocketEngineServerNameResolved);
			OpenSocketL(addr.Address());
		}
		else
		{
			// Must initiate DNS look up of name.

#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= _UIQ_V3_FP0_VERSION_NUMBER__)

			User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp, iConnection->Connection()));

#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

			User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp));

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

			// DNS request for name resolution
			iResolver.GetByName(iServerName, iNameEntry, iStatus);
			
			SetState(KStateSocketEngineResolvingServerName);
			
			// Request time out
			iTimer->AfterL(KTimeIntervalSecondsTimeOut);

			SetActive();
		}
	}
	else
	{
		User::Leave(KErrBadState);
	}
}


void CSocketEngine::OpenSocketL(TUint32 aAddr)
{
	LOG_DEBUG1( "CSocketEngine::OpenSocketL", aAddr);

	const TState state = State();

	if( KStateSocketEngineServerNameResolved == state )
	{

#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp, iConnection->Connection()));

#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));

		// This is the old Series 60 v12 way of going secure (e.g. N-Gage I).
		// On v2.0, 2.1 etc. (e.g. 6600) a dialog pops up reporting "Feature not supported"
		// an on emulator it simply Leaves.
		if ( iGoSecure )
		{
			User::LeaveIfError( iSocket.SetOpt( KSoSecureSocket, KSolInetSSL ) );
		}

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

		// Set up address information
		iAddress.SetPort(iProperties->GetIntL(KPropertyIntSocketPort));
		iAddress.SetAddress(aAddr);

		// Initiate socket connection.
		iSocket.Connect(iAddress, iStatus);

		SetState(KStateSocketEngineConnectingSocket);
    
		// Start a timeout.
		iTimer->AfterL(KTimeIntervalSecondsTimeOut);

		SetActive();
	}
	else
	{
		User::Leave(KErrBadState);
	}
}

void CSocketEngine::WriteL(const TDesC8& aData)
{
	if( ReadyForData() )
    {
	    // Write data to socket
        iSocketWriter->IssueWriteL(aData);
    }
	else
	{
		LOG_ERROR(0, "CSocketEngine::WriteL -- not connected!" );
		User::Leave(KErrBadState);
	}
}
	
TInt CSocketEngine::RemainingWriteBufferSpace()
{
	return iSocketWriter->RemainingWriteBufferSpace();
}

TInt CSocketEngine::SetOption(TUint /*aOption*/, TUint /*aVal*/)
{
	TPckgBuf<int> val(1);
	return iSocket.SetOpt(KSoTcpNoDelay, KSolInetTcp,1);
}

TBool CSocketEngine::ReadyForData()
{
	const TState state = State();

	if( KStateDone == state || KStateSocketEngineSocketConnectedButNotYetSecure == state || KStateSocketEngineConnectedAcceptConnection == state)
    {
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

void CSocketEngine::ReadL()
{
    // Initiate read of data from socket

	TBool readyForData = ReadyForData();

	if( readyForData && (!iSocketReader->IsActive()) ) 
    {
        iSocketReader->Start();
    }
	else if( readyForData )
	{
		LOG_ERROR(0, "CSocketEngine::ReadL -- already waiting on a read!" );
		User::Leave(KErrBadState);
	}
	else
	{
		LOG_ERROR( 0, "CSocketEngine::ReadL -- not connected!" );
		User::Leave(KErrBadState);
	}
}

void CSocketEngine::TimeOutTimerNotify( TAny * /* aCookie */)
{
	Reset();

	LOG_ERROR( 0, "CSocketEngine::TimerExpired" );

	iCallback->ReportError(MConnectionCallback::EErrorConnecting, KErrTimedOut);
}

TUint32 CSocketEngine::ResolvedAddress() const
{
	TNameRecord entry = iNameEntry();
	TBuf<15> ipAddr;
	return TInetAddr::Cast(entry.iAddr).Address();
}

void CSocketEngine::ReportError(MConnectionCallback::TErrorType aErrorType, TInt aErrorCode)
{
    // No recovery or retries are attempted in this example so we just
    // disconnect and inform the user
    Reset();

	// Propagate.
	iCallback->ReportError(aErrorType, aErrorCode);
}


TBool CSocketEngine::Disconnected() const
{
    return( KStateInitial == State() );
}


void CSocketEngine::InitializeL( MProperties * aInitializationProperties /* IN */)
{	
	CBaseStateMachine<CEComPlusRefCountedActive>::InitializeL(aInitializationProperties);

	LOG_DEBUG( "CSocketEngine::InitializeL" );

	// Not owned -- we shouldn't AddRef as we may be added to this 
	// MProperties by our owner. 
	// The same situation as for callback interfaces in general.
	//iProperties->AddRef();

	iConnection = GoL( iProperties, KPropertyObjectConnectionManager, MConnectionManager );
	
	iCallback = GoL( iProperties, KPropertyObjectStateMachineCallback, MConnectionCallback );

	// QueryInterfaceL just raised refcount on an object which owns us.
	// Lower refcount to prevent refcount cycles.
	iCallback->Release(); 

	iSocketReader->InitializeL(aInitializationProperties);
	iSocketWriter->InitializeL(aInitializationProperties);

}




TBool CSocketEngine::AcceptCommandL(TCommand aCommand, MProperties * aCommandProperties )
{
	TBool returnValue = EFalse;
	TRAPD( error, returnValue = InternalAcceptCommandL( aCommand, aCommandProperties ) );
	if( KErrNone != error )
	{
		// Ensure we are in a consistent state.
		Reset();

		LOG_ERROR2( error, "CSocketEngine::AcceptCommandL ", (TInt) State().Low(), (TInt) aCommand.Low() );

		// Propagate the error.
		User::Leave( error );
	}

	return returnValue;
}


TBool CSocketEngine::InternalAcceptCommandL(TCommand aCommand, MProperties * /* aCommandProperties */)
{
	const TState state = State();

	if( KCommandConnectionConnectSecurely == aCommand )
	{
		LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionConnectSecurely" );
		if( KStateInitial == state )
		{
			LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionConnectSecurely proceeding" );
			RaiseConnectionL();
			return ETrue;
		}
	}
	else if( KCommandConnectionConnectWithoutGoingSecure == aCommand )
	{
		LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionConnectWithoutGoingSecure" );
		if( KStateInitial == state )
		{
			LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionConnectWithoutGoingSecure proceeding" );
			RaiseConnectionL();
			return ETrue;
		}
	}
	else if( KCommandConnectionDisconnect == aCommand )
	{
		LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionDisconnect" );
		Reset();
		return ETrue;
	}
	else if( KCommandConnectionResolveHost == aCommand )
	{
		LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionResolveHost" );
		RaiseConnection2L();
		
	}
	else if(KCommandConnectionServerListen == aCommand )
	{
		LOG_DEBUG( "CSocketEngine::InternalAcceptCommandL KCommandConnectionServerListen" );
		ListenOnLocalSockectL();
	}
	else
	{
		LOG_DEBUG2( "CSocketEngine::InternalAcceptCommandL", aCommand.High(), aCommand.Low() );
	}

	return EFalse;
}

void CSocketEngine::HandleResetWhileRaisingConnection(const TState & aState)
{
	if( KStateSocketEngineRaisingConnection == aState )
	{
		SetState( KStateSocketEngineCancellingRaisingConnection );
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

		// Nothing because iConnection.Close later will be cancelling a request. Correct?
#else //__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
		if( iInitiator )
		{
			iInitiator->Cancel();
			// According to Forum Nokia IAPConnect example, CIntConnectionInitiator must be recreated to reset
			delete iInitiator;
			iInitiator = 0;
		}
#endif //__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	}
}

void CSocketEngine::HandleResetWhileResolvingServerName(const TState & aState)
{
	if( KStateSocketEngineResolvingServerName == aState )
	{
	    // Cancel look up attempt
		SetState( KStateSocketEngineCancellingResolvingServerName );
		
	    iResolver.Cancel();
	}
}

void CSocketEngine::HandleResetWhileConnectingSocket(const TState & aState)
{
	if( KStateSocketEngineConnectingSocket == aState)
	{
		SetState( KStateSocketEngineCancellingConnectingSocket );
	    iSocket.CancelConnect();
	}
}

void CSocketEngine::HandleKErrCancelInRunL(const TState & aState)
{
	if( KStateSocketEngineCancellingRaisingConnection == aState )
	{
		LOG_INFO( "CSocketEngine::RunL -- KErrCancel while KStateSocketEngineCancellingRaisingConnection" );
		SetState(KStateInitial);
	}
	else if( KStateSocketEngineCancellingResolvingServerName == aState )
	{
		LOG_INFO( "CSocketEngine::RunL -- KErrCancel while KStateSocketEngineCancellingResolvingServerName" );
		iResolver.Close();
		SetState(KStateInitial);
	}
	else if( KStateSocketEngineCancellingConnectingSocket == aState )
	{
		LOG_INFO( "CSocketEngine::RunL -- KErrCancel while KStateSocketEngineCancellingConnectingSocket" );
		SetState(KStateInitial);
	}
	else if( KStateSocketEngineCancellingSecuring == aState )
	{
		LOG_INFO( "CSocketEngine::RunL -- KErrCancel while KStateSocketEngineCancellingSecuring" );
		SetState(KStateInitial);
	}
	else if ( KStateSocketEngineRaisingConnectionNoOpen == aState)
	{
		LOG_INFO( "CSocketEngine::RunL -- KErrCancel while KStateSocketEngineRaisingConnectionNoOpen" );
		iResolver.Close();
		SetState(KStateInitial);
		User::Leave(KErrCancel);
	}
	else
	{
		LOG_ERROR( 0, "CSocketEngine::RunL -- KErrCancel received in unexpected state" );
		SetState(KStateUndefined);
		User::Leave(KErrBadState);
	}

	if( iNewRequestToConnectIssuedWhileCancelling )
	{
		iNewRequestToConnectIssuedWhileCancelling = EFalse;
		RaiseConnectionL();
	}
}

void CSocketEngine::HandleRunLWhileRaisingConnectionNoOpenL(TInt aStatus)
{
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	if( KErrNone == aStatus )
	{
		LOG_DEBUG( "CSocketEngine::RunL KStateSocketRaisingConnection OK" );

		SetState(KStateSocketEngineConnectionRaised);
		CheckDNSL();
#if __S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__
		TInt iapUid = 0;
		TConnectionInfo inf;
		TPckg<TConnectionInfo> info(inf);
		TUint count;
		if(iConnection.EnumerateConnections(count) == KErrNone)
		{
			if(iConnection.GetConnectionInfo(count, info)== KErrNone)
			{
				iapUid = inf.iIapId;
				TRAPD(iapError, iProperties->SetIntL(KPropertyIntSocketConnectionIap,iapUid));
			}
		}
#endif
	}
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	if(    KErrNone == aStatus 
		|| KConnectionPref1Exists == aStatus
		|| KConnectionPref2Exists == aStatus
		|| KConnectionExists == aStatus
		|| KConnectionPref1Created == aStatus
		|| KConnectionPref2Created == aStatus
		|| KConnectionCreated == aStatus
		)
	{
		LOG_DEBUG1( "CSocketEngine::RunL KStateSocketRaisingConnection OK", aStatus );
			

			SetState(KStateSocketEngineConnectionRaised);
			CheckDNSL();
		}
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	else
    {
		// Something went wrong -- try again.
		// e.g. According to the Forum Nokia IAPConnect example, changing the
		// IAP may fail with KErrAccessDenied on the first try. 
		// On GPRS networks with e.g. a 6600, we see a lot of -4159 errors the first try.
		// Just try again.
		++iRaisingConnectionRetry;

		iConnection->Close();
		
		LOG_INFO1( "CSocketEngine::RunL KStateSocketRaisingConnection", aStatus );
		if( iRaisingConnectionRetry <= KNumberRaisingConnectionRetries )
		{
			LOG_INFO1( "CSocketEngine::RunL KStateSocketRaisingConnection retry", iRaisingConnectionRetry );
			SetState(KStateInitial);
			RaiseConnection2L();
		}
		else
		{
			LOG_ERROR1( aStatus, "CSocketEngine::RunL KStateSocketRaisingConnection max retries exceeded", KNumberRaisingConnectionRetries );
			SetState(KStateSocketEngineErrorRaisingConnection);
			iCallback->ReportError(MConnectionCallback::EErrorConnecting, aStatus);
			//User::Leave(aStatus);
		}
    }
}

void CSocketEngine::HandleRunLWhileRaisingConnectionL(TInt aStatus)
{
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	if( KErrNone == aStatus )
	{
		//TUint32 iap = 0; 
	// Note: docs are inconsistent -- it should be a backslash.
	//iConnection.GetIntSetting( _L("IAP\\Id"), iap );
	//LOG_DEBUG1( "iap", (TInt) iap );

	LOG_DEBUG( "CSocketEngine::RunL KStateSocketRaisingConnection OK" );

		SetState(KStateSocketEngineConnectionRaised);
		CheckDNSAndOpenSocketL();
#if __S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__

		TInt iapUid = 0;
		TConnectionInfo inf;
		TPckg<TConnectionInfo> info(inf);
		TUint count;
		if(iConnection.EnumerateConnections(count) == KErrNone)
		{
			if(iConnection.GetConnectionInfo(count, info)== KErrNone)
			{
				iapUid = inf.iIapId;
				TRAPD(iapError, iProperties->SetIntL(KPropertyIntSocketConnectionIap,iapUid));
			}
		}
#endif
	}
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	if(    KErrNone == aStatus 
		|| KConnectionPref1Exists == aStatus
		|| KConnectionPref2Exists == aStatus
		|| KConnectionExists == aStatus
		|| KConnectionPref1Created == aStatus
		|| KConnectionPref2Created == aStatus
		|| KConnectionCreated == aStatus
		)
	{
		LOG_DEBUG1( "CSocketEngine::RunL KStateSocketRaisingConnection OK", aStatus );
			

			SetState(KStateSocketEngineConnectionRaised);
			CheckDNSAndOpenSocketL();
		}
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	else
    {
		// Something went wrong -- try again.
		// e.g. According to the Forum Nokia IAPConnect example, changing the
		// IAP may fail with KErrAccessDenied on the first try. 
		// On GPRS networks with e.g. a 6600, we see a lot of -4159 errors the first try.
		// Just try again.
		++iRaisingConnectionRetry;
		
		LOG_INFO1( "CSocketEngine::RunL KStateSocketRaisingConnection", aStatus );
		if( iRaisingConnectionRetry <= KNumberRaisingConnectionRetries )
		{
			LOG_INFO1( "CSocketEngine::RunL KStateSocketRaisingConnection retry", iRaisingConnectionRetry );
			SetState(KStateInitial);
			RaiseConnectionL();
		}
		else
		{
			LOG_ERROR1( aStatus, "CSocketEngine::RunL KStateSocketRaisingConnection max retries exceeded", KNumberRaisingConnectionRetries );
			SetState(KStateSocketEngineErrorRaisingConnection);
			iCallback->ReportError(MConnectionCallback::EErrorConnecting, aStatus);
			//User::Leave(aStatus);
		}
    }
}

void CSocketEngine::HandleRunLWhileResolvingServerNameNoConnectL(TInt aStatus)
{
	iResolver.Close();
	if(	KErrNone == aStatus )
    {
		// DNS look up successful
		iNameRecord = iNameEntry();
		TBuf<15> ipAddr;
		TUint32 add = TInetAddr::Cast(iNameRecord.iAddr).Address();

		// And connect to the IP address
		SetState(KStateSocketEngineServerNameResolved);
		iCallback->CallbackCommandL(KCommandStateMachineFinishedNegotiatingConnection);
    }
	else
    {	
        // DNS lookup failed.
		LOG_ERROR( aStatus, "CSocketEngine::RunL -- DNS lookup failed" ); 
		SetState(KStateSocketEngineErrorResolvingServerName);
		iCallback->ReportError(MConnectionCallback::EErrorDNSLookup, aStatus );
		//User::Leave(aStatus);
    }
}

void CSocketEngine::HandleRunLWhileResolvingServerNameL(TInt aStatus)
{
	iResolver.Close();
	iConnection->Close();
	if(	KErrNone == aStatus )
    {
		// DNS look up successful
		iNameRecord = iNameEntry();
		// Extract domain name and IP address from name record
		//Print(_L("Domain name = "));
		//Print(iNameRecord.iName);
        TBuf<15> ipAddr;
		TInetAddr::Cast(iNameRecord.iAddr).Output(ipAddr);
		//Print(_L("\r\nIP address = "));
		//Print(ipAddr);
        //Print(_L("\r\n"));
		// And connect to the IP address
		SetState(KStateSocketEngineServerNameResolved);
		OpenSocketL(TInetAddr::Cast(iNameRecord.iAddr).Address());
    }
	else
    {	
        // DNS lookup failed.
		LOG_ERROR( aStatus, "CSocketEngine::RunL -- DNS lookup failed" ); 
		SetState(KStateSocketEngineErrorResolvingServerName);
		iCallback->ReportError(MConnectionCallback::EErrorDNSLookup, aStatus );
		//User::Leave(aStatus);
    }
}

void CSocketEngine::HandleRunLWhileConnectingSocketL(TInt aStatus)
{
#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
	if( KErrNone == aStatus )
	{
		// Connection completed successfully
		SetState(KStateSocketEngineSocketConnectedButNotYetSecure);

		// We weren't told to go secure yet.
		LOG_INFO( "CSocketEngine::RunL -- Now connected (insecurely)" );
		iSocketReader->SetSocket( &iSocket);
		iSocketWriter->SetSocket( &iSocket);
		// Start CSocketReader Active Object.
		ReadL(); 
		iCallback->CallbackCommandL(KCommandStateMachineFinishedNegotiatingConnection);
	}
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	if( KErrNone == aStatus )
	{
	    // Connection completed successfully.
		if( iGoSecure )
		{
			// In v12, we set the secure parameters when we created the socket.
			SetState(KStateDone);
		}
		else
		{
			SetState(KStateSocketEngineSocketConnectedButNotYetSecure);
		}

		iSocketReader->SetSocket( &iSocket);
		iSocketWriter->SetSocket( &iSocket);

		//Start CSocketReader Active Object.
		ReadL(); 
    }
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
	else
    {
		LOG_ERROR( aStatus, "CSocketEngine::RunL -- KStateSocketEngineConnectingSocket" );
		SetState(KStateSocketEngineErrorConnectingSocket);
		iCallback->ReportError(MConnectionCallback::EErrorConnecting, aStatus);
		//User::Leave(aStatus);
    }
}

void CSocketEngine::HandleRunLWhileListening(TInt aStatus)
{
	if( KErrNone == aStatus )
	{
		// Connection completed successfully
		SetState(KStateSocketEngineConnectedAcceptConnection);

		LOG_INFO( "CSocketEngine::RunL -- Now connected (insecurely)" );
		iSocketReader->SetSocket( &iSocket);
		iSocketWriter->SetSocket( &iSocket);
		// Start CSocketReader Active Object.
		ReadL(); 
		iCallback->CallbackCommandL(KCommandStateMachineFinishedNegotiatingConnection);
	}
}
