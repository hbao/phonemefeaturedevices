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


#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <os_symbian.hpp>
#include "EcomPlusRefCountedBase.h"
#include "EcomPlusRefCountedActive.h"
#include "EventQueue.h"
#include "Connection.h"
#include "WritableConnection.h"
#include "ThreadUtils.h"
#include "SocketFactory.h"

class MSocketManager;
class MIAPSession;

class CCirBufferPlus : public CCirBuffer
{
public:
	CCirBufferPlus* ExpandL(TInt aNewLength);
};

class CSocket : public CEComPlusRefCountedBase, MConnectionCallback, public MSocket
{
	friend class CSocketFactory;
	public:
		CSocket(TAny * aConstructionParameters,MSocketManager* aFactory,MThread& aThread,CThreadRunner& aThreadRunner);
		void ConstructL();
		// MUnknown
		MUnknown * QueryInterfaceL( TInt aInterfaceId );
		void AddRef()  {CEComPlusRefCountedBase::AddRef();}
		void Release() {CEComPlusRefCountedBase::Release();}
		
		// MStateMachineCallback
		void CallbackCommandL( MStateMachine::TCommand aCommand );
		void ReportStateChanged( TInt aComponentId, MStateMachine::TState aState );
	
		// MConnectionCallback
		void ReportError(TErrorType aErrorType, TInt aErrorCode);
		
		// MSocket
		TInt Connect(TInetAddr aAddr);
		void Close();
		TInt Available();
		TInt Read(TPtr8 aData,TInt* aBytesRead);
		TInt Send(TPtr8 aData, TInt* aTxLen);
		TInt GetIPv4Address();
		TInt SetOption(TUint aOption,TUint aVal);
		
		// called from management thread
		void CloseSocket();
		void DebugSocket();
	protected:
		virtual ~CSocket();
	private:
		static void SignalMutex(TAny*);
		void ConstructConnectionL();

		static void ConnectCallback(TAny* aThis);
		void DoConnectCallbackL();
		static void SendCallback(TAny* aThis);
		void DoSendCallbackL();
		static void SetOptionCallback(TAny* aThis);
		void DoSetOptionCallbackL();
		static void CloseCallback(TAny* aThis);
		void DoCloseCallbackL();
		
	private:
		MProperties* iProperties;
		MSocketManager* iFactory;
		MThread& iThread;
		MWritableConnection* iConnection;
		enum TCommand
		{
			EReady,
			EConnect,
			ESend,
			EClose,
			ESetOpt
		};
		enum TSendState
		{
			EReadyToSend,
			ESending,
			ESent
		};
		enum TConnectState
		{
			EReadyToConnect,
			EConnecting,
			EConnected,
			EDisconnected
		};
		TInetAddr iAddr;
		HBufC8* iTx;
		TInt iTxIndex;
		TInt iCurrentLen;
		TSendState iSending;
		CCirBufferPlus* iBuffer;
		RMutex iMutex;
		TInt iErrorStatus;
		TUint iTxBytes;
		TUint iRxBytes;
		TConnectState iConnected;
		TUint iOption;
		TUint iOptVal;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		MIAPSession* iIAPSession;
#else
		TInt iIAP;
#endif
		TBool iWaitingOnRead;
		CThreadRunner& iThreadRunner;
};
#endif // __SOCKETS_H__
