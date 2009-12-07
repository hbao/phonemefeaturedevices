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

#include "ConnectionManagerImpl.h"
#include "SocketEngine.h"
#include "DebugUtilities.h"

MConnectionManager* CConnectionManager::NewL( TAny * aConstructionParameters )
{
	CConnectionManager* self = new (ELeave) CConnectionManager(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();

	MConnectionManager * connectionManager = QiL( self, MConnectionManager );
	CleanupStack::Pop(self);
	return connectionManager;
}


CConnectionManager::CConnectionManager(TAny * aConstructionParameters )
	: CEComPlusRefCountedActive(aConstructionParameters),iClientStatus(NULL)
{
	DEBUG_CONSTRUCTOR
	CActiveScheduler::Add(this);
}
CConnectionManager::~CConnectionManager()
{
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
}

MUnknown * CConnectionManager::QueryInterfaceL( TInt aInterfaceId )
{
	if( KIID_MConnectionManager == aInterfaceId )
	{
		AddRef();
		return static_cast<MConnectionManager*>(this);
	}
	else
	{
		return CEComPlusRefCountedActive::QueryInterfaceL( aInterfaceId );
	}
}

void CConnectionManager::AddRef()
{
	CEComPlusRefCountedActive::AddRef();
}

void CConnectionManager::Release()
{
	CEComPlusRefCountedActive::Release();
}

void CConnectionManager::InitializeL(MProperties * aInitializationProperties)
{
	iProperties = aInitializationProperties;
	iProperties->AddRef();
	MConnectionManager * connectionManager = reinterpret_cast<MConnectionManager*>(QueryInterfaceL(KIID_MConnectionManager));
	CleanupReleasePushL(*connectionManager);
	iProperties->SetObjectL(KPropertyObjectConnectionManager, connectionManager);
	CleanupStack::PopAndDestroy(connectionManager);
	
	// We've just added ourselves to a MProperties object which we own.
	// To avoid refcount dependencies that could prevent our own ref 
	// count from ever going back down to 0, lower the ref count.
	// to compensate for it getting raised in SetObjectL above.
	connectionManager->Release(); 
}

void CConnectionManager::RaiseConnectionL(RSocketServ& aSocketServer)
{
	if(!iStarted)
	{
		User::LeaveIfError( iConnection.Open(aSocketServer) );
	}
}

void CConnectionManager::Start(TRequestStatus& aStatus)
{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	if(!iStarted)
	{
		TInt iapUid = 0;
		TRAPD(iapError, iapUid = iProperties->GetIntL(KPropertyIntSocketConnectionIap));
		if(iapUid != 0)
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
		
		SetActive();
	}
	else
	{
		User::RequestComplete(iClientStatus,KErrNone);
	}
}

RConnection& CConnectionManager::Connection()
{
	return iConnection;
}

void CConnectionManager::Close()
{
	iConnection.Close();
	iStarted = EFalse;
}


void CConnectionManager::RunL()
{
	User::RequestComplete(iClientStatus,iStatus.Int());
	iStarted = ETrue;
}

void CConnectionManager::DoCancel()
{}

TInt CConnectionManager::RunError(TInt /*aError*/)
{
	return KErrNone;
}
