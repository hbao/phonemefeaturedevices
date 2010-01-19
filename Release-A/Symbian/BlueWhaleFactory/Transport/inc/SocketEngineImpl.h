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


#ifndef __SOCKET_ENGINE_IMPL_H__
#define __SOCKET_ENGINE_IMPL_H__

#include "EComPlusRefCountedActive.h"
#include "BaseStateMachine.h"
#include <in_sock.h>
#include "TimeOutTimer.h"
#include "SocketEngine.h"
#include "Properties.h"
#include <commdb.h>
#include <es_sock.h>



#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

#include <CommDbConnPref.h>
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

#include <intconninit.h> 

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__



class CSocketReader;
class CSocketWriter;
class CTimeOutTimer;

const TInt KNumberRaisingConnectionRetries = 3;


class CSocketEngine : public CBaseStateMachine<CEComPlusRefCountedActive>, public MTimeOutTimerCallback, public MWritableConnection, public MResolvedConnection, public MServerConnection
    {
public:
	static MWritableConnection * NewL( TAny * aConstructionParameters );

public: // MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() {CBaseStateMachine<CEComPlusRefCountedActive>::AddRef();}
	void Release() {CBaseStateMachine<CEComPlusRefCountedActive>::Release();}

public: // MConnection implementation.
    TBool Disconnected() const;

public: // MWritableConnection implementation.
	virtual void WriteL(const TDesC8& aData);
	virtual TInt RemainingWriteBufferSpace();
	virtual TInt SetOption(TUint aOption,TUint aVal);
	
public: // MInitialize implementation.
	virtual void InitializeL( MProperties * aInitializationProperties /* IN */);

public: // MStateMachine implementation.
	virtual TBool AcceptCommandL(TCommand aCommand, MProperties * aCommandProperties);
	virtual void Reset();

public:	// To CSocketRead and CSocketWriter.
	void ReportError(MConnectionCallback::TErrorType aErrorType, TInt aErrorCode);
	TBool ReadyForData();

public: // from MTimeOutTimerCallback
	virtual void TimeOutTimerNotify( TAny * aCookie );

public: // MResolvedConnection
	TUint32 ResolvedAddress() const;

protected: // from CActive
	virtual ~CSocketEngine();
	virtual void DoCancel();
    virtual void RunL();
	virtual TInt RunError(TInt aError);

protected: // New methods
	CSocketEngine( TAny * aConstructionParameters );
    void ConstructL();
    void DoReset();

    void RaiseConnectionL();
	void CheckDNSAndOpenSocketL();
	void OpenSocketL(TUint32 aAddr);
	
    void RaiseConnection2L();
	void CheckDNSL();

	void ListenOnLocalSockectL();
	
	void ReadL();
	virtual TBool InternalAcceptCommandL(TCommand aCommand, MProperties * aCommandProperties);


	void HandleResetWhileRaisingConnection(const TState & aState);
	void HandleResetWhileResolvingServerName(const TState & aState);
	void HandleResetWhileConnectingSocket(const TState & aState);

	void HandleKErrCancelInRunL(const TState & aState);
	
	void HandleRunLWhileRaisingConnectionL(TInt aStatus);
	void HandleRunLWhileRaisingConnectionNoOpenL(TInt aStatus);
	void HandleRunLWhileResolvingServerNameL(TInt aStatus);
	void HandleRunLWhileResolvingServerNameNoConnectL(TInt aStatus);
	void HandleRunLWhileConnectingSocketL(TInt aStatus);
	void HandleRunLWhileListening(TInt aStatus);
	
	void SetupConnectionL();
	void StartConnectionL();
	
private: // Member variables

    /*! @const The maximum time allowed for a lookup or connect requests to complete */
    static const TInt KTimeIntervalSecondsTimeOut;

	// Not owned -- never AddRef on a callback as it could be your owner
	// and this would create a refcount cycle that will prevent objects
	// from ever being cleaned up.
	friend class CSocketReader; // TODO: Figure out best way to give access to this to CSocketReader
	friend class CSocketWriter; // TODO: Figure out best way to give access to this to CSocketWriter
	MConnectionCallback	*	iCallback; 

    /**
     * This is the unsecured socket.
     */
	RSocket                     iAcceptSocket;
	RSocket                     iSocket;

    /*! @var active object to control reads from the socket */
	CSocketReader*             iSocketReader;

    /*! @var active object to control writes to the socket */
	CSocketWriter*             iSocketWriter;

    /*! @var the socket server */
	RSocketServ                 iSocketServ;




#if (__S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

	RConnection					iConnection;

	// Just for kicks, I will make sure this variable used to issue an
	// asynchronous call in RaiseConnectionL exists even when that
	// method goes out of scope.
	TCommDbConnPref				iCommDbConnPref; 

#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

	CIntConnectionInitiator *	iInitiator;
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref iConnPref;

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__


    /*! @var DNS name resolver */
	RHostResolver               iResolver;

    /*! @var The result from the name resolver */
	TNameEntry                  iNameEntry;

    /*! @var The name record found by the resolver */
	TNameRecord                 iNameRecord;

	MTimeOutTimer *              iTimer;

    /*! @var The address to be used in the connection */
	TInetAddr                   iAddress;

	// A flag we set at the start of our destructor, so Reset operations
	// can tell whether they should bother waiting for Cancel's to complete.
	TBool iDeleting;

	TInt iRaisingConnectionRetry;

	// Just for kicks, I will make sure this variable used to issue an
	// asynchronous call in OpenSocketL exists even when that
	// method goes out of scope.
	TBuf<KMaxServerNameLength> iServerName;
	
	
	/**
	 * Sometimes we may be issued a request to initiate a new connection while
	 * we may be in the middle of tearing down or resetting an old one.
	 * This allows us to track this, so that as soon as we have finished
	 * cancelling we can start again.
	 */
	TBool iNewRequestToConnectIssuedWhileCancelling;
    };

#endif /* __SOCKET_ENGINE_IMPL_H__ */

