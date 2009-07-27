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

#ifndef __APNMANAGER_H__
#define __APNMANAGER_H__

#include <e32base.h>
#include <Commdb.h>
#include "Properties.h"
#include "NetworkInfo.h"
#include "APNDatabase.h"
#include "IAPInfo.h"
#include "CommDBUtil.h"

#define __DONT_USE_OWNAPN__

_LIT(KBlueWhaleAPN,"BlueWhale!");
const TStringUniqueKey PROPERTY_STRING_COUNTRY_CODE = {0x00000100, 0x00000001};
const TStringUniqueKey PROPERTY_STRING_NETWORK_ID 	= {0x00000100, 0x00000002};
const TIntUniqueKey PROPERTY_INT_DB_COUNT			= {0x00000100, 0x00000003};
const TIntUniqueKey PROPERTY_INT_IAP				= {0x00000100, 0x00000004};
const TIntUniqueKey PROPERTY_INT_PORT				= {0x00000100, 0x00000005};
const TIntUniqueKey PROPERTY_INT_PRIORITY			= {0x00000100, 0x00000006};

class MIAPSession
{
public:
	virtual TInt GetNextIAP(TInt aPort) = 0;
	virtual void ReportStatus(TBool aSuccess, TInt aIAP, TInt aPort) = 0;
	virtual void Release() = 0;
protected:
	virtual ~MIAPSession(){};
};

class CAPNManager;

class CMIAPSessionImpl : public CBase, public MIAPSession
	{
	public:
		static CMIAPSessionImpl* NewL(CAPNManager* aParent);
		virtual ~CMIAPSessionImpl();
		virtual TInt GetNextIAP(TInt aPort);
		virtual void ReportStatus(TBool aSuccess,TInt aIAP, TInt aPort);
		virtual void Release();
	private:
		CMIAPSessionImpl();
		CMIAPSessionImpl(CAPNManager* aParent);
		void ConstructL();
	private:
		CAPNManager* iParent;
		TInt iIndex;
	};

class MAPNManager
{
public:
	virtual MIAPSession* StartIAPSession() = 0;
#ifdef __WINSCW__
	virtual void SetBeingTested() = 0;
#endif
protected:
	virtual ~MAPNManager(){}
};

enum TIAPPriority
{
	KWlanPriority = 0x100,
	KBlueWhalePriority = 0x200,
	KTherestPriority = 0x300,
	KUserPickPriority = 0x400
};

class TIAPWithPort
{
public:
	TIAPWithPort() : iIAP(0),iPort(0),iPriority(KMaxTInt32){}
	TIAPWithPort(TInt aIAP,TInt aPort,TInt aPriority) : iIAP(aIAP),iPort(aPort),iPriority(aPriority){}
	
	void LoadL(MProperties* aProperties,TInt aIndex)
	{
		iIAP = aProperties->GetIntL(PROPERTY_INT_IAP,aIndex);
		iPort = aProperties->GetIntL(PROPERTY_INT_PORT,aIndex);
		TRAPD(err,iPriority = aProperties->GetIntL(PROPERTY_INT_PRIORITY,aIndex));
		if(err != KErrNone)
		{
			iPriority = KMaxTInt32;
		}
	}
	
	void SaveL(MProperties* aProperties,TInt aIndex)
	{
		aProperties->SetIntL(PROPERTY_INT_IAP,iIAP,aIndex);
		aProperties->SetIntL(PROPERTY_INT_PORT,iPort,aIndex);
		aProperties->SetIntL(PROPERTY_INT_PRIORITY,iPriority,aIndex);
	}
	
	TBool operator ==(const TIAPWithPort& aComp) const 
	{ 
		return (iIAP == aComp.iIAP && iPort == aComp.iPort && iPriority == aComp.iPriority);
	}
	TBool operator <(const TIAPWithPort& aComp) const 
	{ 
		return (/*(iIAP < aComp.iIAP || iPort < aComp.iPort) && */ iPriority < aComp.iPriority);
	}

	static TBool ByGroupedPort(const TIAPWithPort& aLeft, const TIAPWithPort& aRight)
	{
		return ((aLeft.iPort == aRight.iPort)
				|| (aLeft.iPort == 80 && aRight.iPort == 443)
				|| (aLeft.iPort == 443 && aRight.iPort == 80) 
				|| (aLeft.iPort != 80 && aRight.iPort != 80 && aLeft.iPort != 443 && aRight.iPort != 443)
				|| (aLeft.iPort == 0 || aRight.iPort == 0)
				);
	}
	TInt iIAP;
	TInt iPort;
	TInt iPriority;
};


template <class T> 
class CIAPRuntimeDatabase : public CBase
{
public:
	CIAPRuntimeDatabase(){}
	virtual ~CIAPRuntimeDatabase()
	{
		iDatabase.Reset();
	}

	static TInt Order(const T & aLeft, const T & aRight)
	{
		if(aLeft == aRight)
		{
			return 0;
		}
		else if(aLeft < aRight)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	
	void AddWorkingIAP(const T aIAP)
	{
		TLinearOrder< T > order(Order);
		TInt ret = iDatabase.InsertInOrder(aIAP,order);
		if(ret != KErrNone || ret != KErrAlreadyExists)
		{
//			RDebug::Print(_L("Workingset add %d"),aIAP);
		}
	}
	
	void RemoveIAP(const T aIAP)
	{
		TIdentityRelation< T > identity(T::ByGroupedPort);
		TInt found = iDatabase.Find(aIAP,identity);
		if(found != KErrNotFound)
		{
			iDatabase.Remove(found);
		}
	}
	
	TInt FindByPort(const T& aEntry)
	{
		TIdentityRelation< T > identity(T::ByGroupedPort);
		return iDatabase.Find(aEntry,identity);
	}
	
	void LoadL(MProperties* aProperties)
	{
		TInt count = aProperties->GetIntL(PROPERTY_INT_DB_COUNT);
		iDatabase.Reset();
		for(TInt i=0;i<count;i++)
		{
			T newIAP;
			newIAP.LoadL(aProperties,i);
			AddWorkingIAP(newIAP);
		}
	}
	
	void SaveL(MProperties* aProperties)
	{
		TInt count = iDatabase.Count(); 
		aProperties->SetIntL(PROPERTY_INT_DB_COUNT,count,0);
		for(TInt i=0;i<count;i++)
		{
			iDatabase[i].SaveL(aProperties,i);
		}
	}
	void ResetL(MProperties* aProperties)
	{
		iDatabase.Reset();
		aProperties->SetIntL(PROPERTY_INT_DB_COUNT,0,0);
	}
	TInt Count() { return iDatabase.Count();}
	const RArray<T>& WorkingSet() { return iDatabase;}
private:
	RArray<T> iDatabase;
};

class CAPNManager : public CBase, public MAPNManager, public MNetWorkInfoObserver
{
public:
	static CAPNManager* NewL(MProperties* aProperties);
	virtual ~CAPNManager();
	
	virtual MIAPSession* StartIAPSession();
#ifdef __WINSCW__
	virtual void SetBeingTested() { iBeingTested = ETrue;}
#endif
	void IAPReport(TBool aSuccess,TInt aIAP,TInt aPort);
	TInt GetIAP(TInt aIndex,TInt aPort);
	void SetAutoAPN();
	void Reset();
private:
	CAPNManager();
	CAPNManager(MProperties* aProperties);
	void ConstructL();
	TUint32 CreateBlueWhaleIAPL();
	void UdateBlueWhaleIAPL(const TDesC& aCountryCode,const TDesC& aNetworkId);
	void BuildCurrentValidIAPsL();
	void SaveSettingsL(const TDesC& aCountryCode,const TDesC& aNetworkId);
	void LoadPropertiesL(TDes& aCountry,TDes& aNetwork);
	//
	virtual void NetworkChanged(const TDesC& aCountryCode,const TDesC& aNetworkId);
	MIAPSession* DoStartIAPSessionL();
	void DoNetworkChangedL(const TDesC& aCountryCode,const TDesC& aNetworkId);
	void IAPReportL(TBool aSuccess,TInt aIAP,TInt aPort);
	
private:
	CNetworkInfoManager* iNetWorkInfo;
	CAPNDatabase iDatabase;
	CIAPRuntimeDatabase<TIAPWithPort> iRuntimeDatabase;
	RArray<TIAPWithPort> iCurrentIAPList;
	TBool iOk;
	CIAPInfo* iIAPInfo;
	MProperties* iProperties;
	CCommDBUtil* iCommDBUtil;
	TBool iAPNCreationEnabled;
	TInt iWlanCount;
#ifdef __WINSCW__
	TBool iBeingTested;
#endif
};

#endif /*__APNMANAGER_H__*/
