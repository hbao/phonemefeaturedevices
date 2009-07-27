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


#ifndef __TESTWITHPORTCONNECTIONMONITORWRAPPER_H__
#define __TESTWITHPORTCONNECTIONMONITORWRAPPER_H__

_LIT(KTestWlanWithIAP,"TestWLANIAP");
_LIT(KTestWlan,"TestWLAN");


class CTestConnectionMonitorWrapperWithTimeout : public CBase, public MConnectionMonitorWrapper
{
public:
	CTestConnectionMonitorWrapperWithTimeout(TInt aDelay) : iDelay(aDelay)
	{}
	virtual ~CTestConnectionMonitorWrapperWithTimeout()
	{
		delete iTimer;
	}
protected:
	// MConnectionMonitorWrapper
	virtual void Close()
	{}

	virtual TInt ConnectL()
	{
		return KErrNone;
	}

	virtual TInt NotifyEventL( MConnectionMonitorObserver& aObserver )
	{
		return KErrNone;
	}

	virtual TInt GetConnectionInfo( const TUint aIndex,TUint& aConnectionId,TUint& aSubConnectionCount ) const

	{
		return KErrNone;
	}

	virtual void GetStringAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes& aValue,TRequestStatus& aStatus ) const
	{}

	virtual void GetPckgAttribute( const TUint aConnectionId,const TUint aSubConnectionId,const TUint aAttribute,TDes8& aValue,TRequestStatus& aStatus ) const

	{
		iClientStatus = &aStatus;
		*iClientStatus = KRequestPending;
		switch(aAttribute)
		{
			case KNetworkNames:
			{
				TConnMonNetworkNames info;
				TPckg<TConnMonNetworkNames> infoPckg(info);
				info.iCount = 2;
				info.iNetwork[0].iName.Copy(KTestWlanWithIAP);
				info.iNetwork[0].iSignalStrength = 10;
				
				info.iNetwork[1].iName.Copy(KTestWlan);
				info.iNetwork[1].iSignalStrength = 10;
				
				aValue.Copy(infoPckg);
				TRequestStatus* status = &aStatus; 
				User::RequestComplete(status,KErrNone);
				break;
			}
			case KIapAvailability:
			{
				TConnMonIapInfo info;
				TPckg<TConnMonIapInfo> infoPckg(info);
				
				info.iCount = 2;
				info.iIap[0].iIapId = 1;
				info.iIap[1].iIapId = 5;
				aValue.Copy(infoPckg);
				User::RequestComplete(iClientStatus,KErrNone);
				iClientStatus = NULL;
				break;
			}
			default:
				CActiveScheduler::Stop();
		}
		if(!iTimer)
		{
			iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
			TCallBack callBack(TimeOut,(TAny*)this);
			iTimer->Start(iDelay,0,callBack);
		}

	}

	virtual void GetConnectionCount( TUint& aConnectionCount, TRequestStatus& aStatus )

	{
		aConnectionCount = 0;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNone);
	}

	virtual void CancelAsyncRequest(TInt aReqToCancel)

	{
		if(iClientStatus)
		{
			User::RequestComplete(iClientStatus,KErrCancel);
			iClientStatus = NULL;
		}
	}

	
	// MUnknown implementation.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId )
	{
		return NULL;
	}
	void AddRef() {iRef++;}
	virtual void Release() 
	{
		if(--iRef <= 0)
		{
			delete this;
		}
	}
	static TInt TimeOut(TAny* aThis)
	{
		CTestConnectionMonitorWrapperWithTimeout* This = static_cast<CTestConnectionMonitorWrapperWithTimeout*>(aThis);
		return This->TimeOut();
	}

	TInt TimeOut()
	{
		CActiveScheduler::Stop();
		return KErrNone;
	}

	mutable TRequestStatus* iClientStatus;
	TInt iRef;
	TInt iDelay;
	mutable CPeriodic* iTimer;
};

#endif
