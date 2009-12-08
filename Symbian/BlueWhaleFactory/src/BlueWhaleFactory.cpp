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



#include <OSVersion.h>
#include "EcomPlus.h"

// Place object include files here
#ifdef __WINSCW__	
#include "BlankObject.h"
#endif
#include "PropertiesImpl.h"
#include "LoggerImpl.h"
#include "DataAccessConnectionImpl.h"
#include "SocketEngineImpl.h"
#include "TimeOutTimerImpl.h"
#include "PlatformVMTimerStateMachine.h"
#include "Thread.h"
#include "BaseMIDPView.h"
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include "NetworkInfo.h"
#include "IAPInfo.h"
#endif

#include "CommDBUtil.h"
// End of object include files

#define CREATEOBJECT(_AA,_BB) _AA::NewL(_BB)
#define CREATEOBJECT_WITH_EXCLUDE_LIST(_AA,_BB) _AA::NewL(_BB)
#define CREATEOBJECT_NO_TEST(_AA,_BB) _AA::NewL(_BB)

//#define __MEMORY_TESTS__

#ifdef __MEMORY_TESTS__
#define EXCLUDE_LIST(_LEN,_ARRAY...) 

static TBool alreadyChecking = EFalse;
template<class T> MUnknown * MemoryLeakCheck(TAny * aConstructionParameters,TPtrC aName, TInt* aDontTest = NULL, TInt aCount=0)
{
	if(!alreadyChecking)
	{
		RDebug::Print(_L("Entering check"));
		alreadyChecking = ETrue;
		MUnknown * unknown = 0;
		__UHEAP_MARK;
		TInt err = KErrNone;
		for(TInt k=1;;k++)
		{
			TBool skip = EFalse;
			for(TInt i = 0;i<aCount;i++)
			{
				if(k == aDontTest[i])
				{
					RDebug::Print(_L("skipping %d"),k);
					skip = ETrue;
				}
			}		
			if(!skip)
			{
				RDebug::Print(_L("Loop %S %d"),&aName,k);
				__UHEAP_SETFAIL(RHeap::EDeterministic,k);
				__UHEAP_MARK;
				TRAP(err,unknown = T::NewL(aConstructionParameters));
				if(err == KErrNone)
				{
					unknown->Release();
				}
				__UHEAP_MARKEND;
				User::Heap().Check();
				if(err == KErrNone)
				{
					break;
				}
			}
		}
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		User::Heap().Check();
		alreadyChecking = EFalse;
		RDebug::Print(_L("Exiting check"));
	}
	return T::NewL(aConstructionParameters);
}
#undef CREATEOBJECT
#undef CREATEOBJECT_WITH_EXCLUDE_LIST
#undef EXCLUDE_LIST

#define CREATEOBJECT(_AA,_BB) MemoryLeakCheck< _AA > ( _BB , _L( #_AA ))
#define CREATEOBJECT_WITH_EXCLUDE_LIST(_AA,_BB) MemoryLeakCheck< _AA > ( _BB , _L( #_AA ) , _dontTest, _len)
#define EXCLUDE_LIST(_LEN,_ARRAY...) TInt _dontTest[] = { _ARRAY } ; TInt _len = _LEN;
#endif // __MEMORY_TESTS__



/*  For emulator builds allows us to replace the createimp function for testing purposes.
*   This allows us to create dummy test objects
*/
#ifdef __WINSCW__	
TDelegate REComPlusSession::iDelegate = NULL;
EXPORT_C void REComPlusSession::SetDelegate(TDelegate aDelegate)
	{
	iDelegate = aDelegate;
	}
#endif

/**
 * On versions of Symbian which do not have the ecom implemention.
 * this method allows us object factory functionality for cheap.
 */
EXPORT_C MUnknown * REComPlusSession::CreateImplementationL(TUid aImplementationUid, TUid aInterfaceUid, TAny * aConstructionParameters )
{
#ifdef __WINSCW__
	if(iDelegate)
	{
		return iDelegate(aImplementationUid,aInterfaceUid,aConstructionParameters);
	}
#endif
	MUnknown * unknown = 0;
	switch(aImplementationUid.iUid)
	{
#ifdef __WINSCW__	
	case KIID_MBlankObject:
		unknown = CREATEOBJECT(CBlankObject,aConstructionParameters);
		break;
#endif
	case KCID_MProperties:
		unknown = CREATEOBJECT(CProperties,aConstructionParameters);
		break;
	case KCID_MLogger:
		unknown = CREATEOBJECT(CLogger,aConstructionParameters);
		break;
	case KCID_MMessagingDataAccessConnection:
		unknown = CREATEOBJECT(CDataAccessConnection,aConstructionParameters);
		break;
	case KCID_MSocketEngine:
		unknown = CREATEOBJECT(CSocketEngine,aConstructionParameters);
		break;
	case KCID_MTimeOutTimer:
		unknown = CREATEOBJECT(CTimeOutTimer,aConstructionParameters);
		break;
	case KCID_MTimerStateMachine:
		unknown = CREATEOBJECT(CVMTimerStateMachine,aConstructionParameters);
		break;
	case KCID_MThread:
		unknown = CREATEOBJECT_NO_TEST(CThread,aConstructionParameters);
		break;
	case KCID_MVMObjectFactory:
		unknown = CREATEOBJECT(CVMObjectFactory,aConstructionParameters);
		break;
	case KCID_MBaseMIDPView:
		unknown = CREATEOBJECT_NO_TEST(CBaseMIDPView,aConstructionParameters);
		break;
#if __S60_VERSION__ > __S60_V2_FP2_VERSION_NUMBER__
	case KCID_MTelephonyWrapper:
		unknown = CREATEOBJECT(CTelephonyWrapper,aConstructionParameters);
		break;
	case KCID_MConnectionMonitorWrapper:
		unknown = CREATEOBJECT(CConnectionMonitorWrapper,aConstructionParameters);
		break;
	case KCID_MCommDBWrapper:
		unknown = CREATEOBJECT(CCommDBWrapper,aConstructionParameters);
		break;
#endif
	}
	
    if(!unknown)
	{
		User::Leave(KErrNotFound);
	}
	CleanupReleasePushL(*unknown);

	MUnknown * expectedCastUnknown = unknown->QueryInterfaceL(aInterfaceUid.iUid);
	
	CleanupStack::PopAndDestroy(unknown);
	return expectedCastUnknown;
}

EXPORT_C void REComPlusSession::DestroyedImplementation(TUid /* aDtorIDKey */)
{
	// Do nothing as this is a stub.
	//REComSession::DestroyedImplementation(aDtorIDKey);
}

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#else
#ifdef __WINSCW__
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
	return(KErrNone);
}
#endif // __WINSCW__
#endif // __S60_VERSION__ <= __S60_V2_FP3_VERSION_NUMBER__
