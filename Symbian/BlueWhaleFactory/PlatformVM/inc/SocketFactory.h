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

#ifndef __SOCKETFACTORY_H__
#define __SOCKETFACTORY_H__

#include "EventQueue.h"
#include "Properties.h"
#include "Application.h"
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
#include "APNManager.h"
#endif

class CSocket;
class CHostResolver;
class CApSelect;
class CCommsDatabase;
class MPropertyPersist;
class MConnectionManager;

class MSocketManager
{
public:
	virtual void SocketDestroyed(CSocket* aSocket) = 0;
	virtual void ResolverDestroyed(CHostResolver* aREsolver)=0;
	virtual void DataReceived(MSocket* aClient) = 0;
	virtual void DataSent(MSocket* aClient) = 0;
	virtual void HostResolved(MHostResolver* aClient) = 0;
	virtual void ResetAPN() = 0;
	virtual void SetAutoAPN() = 0;
	virtual TBool CanResolve() = 0;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	virtual MIAPSession* StartIAPSession() = 0;
#else
	virtual TInt IAPForPort(TInt aPort) = 0;
	virtual void SetIAPForPortL(TInt aPort,TInt aIap) = 0;
#endif
protected:
	virtual ~MSocketManager(){}
};

class CSocketFactory : public CBase, public MSocketManager, public MRunnable
{
	public:
		CSocketFactory(MEventQueue* aQueue,MProperties* aProperties,MPropertyPersist* aPersist);
		virtual ~CSocketFactory();
		void ConstructL();
		//
		MSocket* CreateSocket();
		MHostResolver* CreateResolver();
		// MSocketManager
		virtual void SocketDestroyed(CSocket* aSocket);
		virtual void ResolverDestroyed(CHostResolver* aREsolver);
		virtual void DataReceived(MSocket* aClient);
		virtual void DataSent(MSocket* aClient);
		virtual void HostResolved(MHostResolver* aClient);
		virtual void ResetAPN();
		virtual void SetAutoAPN();
		virtual TBool CanResolve();
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		virtual MIAPSession* StartIAPSession();
#else
		TInt IAPForPort(TInt aPort);
		virtual void SetIAPForPortL(TInt aPort,TInt aIap);
#endif
		//
		void StartL(RThread& aThread);
		void StopL();
		void SetThread(MThread* aThread);

		//
		void DebugFactory();

	private:
		TBool CheckIAPValidityL(TInt aIap);
		static void ResolverDestroyedCallback(TAny* aThis);
		void DoResolverDestroyedCallbackL();
		static void ResetAPNCallback(TAny* aThis);
		void DoResetAPNCallbackL();
		static void SetAutoAPNCallback(TAny* aThis);
		void DoSetAutoAPNCallback();

	private:
		CThreadRunner* iThreadRunner;
		MProperties* iProperties;
		MPropertyPersist* iPersist;
		MThread* iThread;	
		RPointerArray<CSocket> iSockets;
		RPointerArray<CHostResolver> iResolvers;
		MEventQueue* iQueue;
		CHostResolver* iResolver;
		CCommsDatabase* iCommDb;
		CApSelect* iApSelect;
		TInt iCurrentPort;
		TInt iErrorStatus;
		MConnectionManager* iConnectionManager;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		CAPNManager* iAPNManager;
#endif
		TBool iCanResolve;
};




#endif /*__SOCKETFACTORY_H__*/
