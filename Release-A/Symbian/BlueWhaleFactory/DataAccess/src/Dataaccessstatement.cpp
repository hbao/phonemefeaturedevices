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


#include "DataAccessStatementImpl.h"
#include "DataAccessConnectionImpl.h"
#include "Message.h"
#include "EcomPlus.h"
#include "CommandMessage.h"
#include "EmailMessage.h"
#include "DebugUtilities.h"

MDataAccessStatement * CDataAccessStatement::NewL(CDataAccessConnection *aDataAccessConnection)
{
	CDataAccessStatement * self = new (ELeave) CDataAccessStatement(0);
	CleanupStack::PushL(self);
	self->ConstructL(aDataAccessConnection);
	
	MDataAccessStatement * dae = QiL( self, MDataAccessStatement );

	CleanupStack::Pop(self);
	return dae;
}

CDataAccessStatement::CDataAccessStatement(TAny * aConstructionParameters)
: CProperties(aConstructionParameters)
{
	DEBUG_CONSTRUCTOR
}

void CDataAccessStatement::ConstructL(CDataAccessConnection * aDataAccessConnection)
{
	CProperties::ConstructL();

	iDataAccessConnection = aDataAccessConnection;
	iDataAccessConnection->AddRef(); // We "own" it -- it doesn't own us.

}

CDataAccessStatement::~CDataAccessStatement()
{
	DEBUG_DESTRUCTOR

	if( iDataAccessConnection )
	{
		iDataAccessConnection->Release();
		iDataAccessConnection = 0;
	}
}




MUnknown * CDataAccessStatement::QueryInterfaceL(TInt aInterfaceId)
{
	if( KIID_MDataAccessStatement == aInterfaceId )
	{
		AddRef(); // Remember to do this in your subclasses for every QI.
		return static_cast<MDataAccessStatement*>(this);
	}
	else if( KIID_MExternalizable == aInterfaceId )
	{
		User::Leave(KErrNotSupported); // For now.
		return 0;
	}
	else
	{
		return CProperties::QueryInterfaceL(aInterfaceId);
	}
}

void CDataAccessStatement::SetType( const TStatementType & aStatementType )
{
	iStatementType = aStatementType;
}


MUnknown * CDataAccessStatement::ExecuteL( TInt aInterfaceId )
{
	const TDesC * folder = 0;
	TRAPD( failureIgnored, folder = &(GetStringL(KPropertyStringFolder, 0)) );

	if(KStatementTypeRootFolderInfo == iStatementType )
	{
		HBufC * directoryFullPath = 0;
		if(folder)
		{
			directoryFullPath = iDataAccessConnection->CreateDirectoryFullPathLC( CDataAccessConnection::EStorageRootInfo, KNullDesC, *folder);
		}
		else
		{
			User::Leave(KErrArgument);
		}
		HBufC* fileName;
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CDir* files;
		CDir *dirs;
		if(fs.GetDir(*directoryFullPath,KEntryAttNormal,ESortByDate,files,dirs) == KErrNone)
		{
			if(files->Count() > 0)
			{
				TEntry info = (*files)[files->Count()-1];
				fileName = info.iName.AllocLC();
			}
			else
			{
				fileName = KInfoFileName().AllocLC();
			}
		}
		else
		{
			fileName = KInfoFileName().AllocLC();
		}
		
		delete dirs;
		delete files;
		
		HBufC * fileFullPath = iDataAccessConnection->CreateFileFullPathLC(*directoryFullPath,*fileName );

		TUint32 aDummyAttributes = 0;
		MUnknown * unknown = 0;
		TRAPD(loadError, unknown = iDataAccessConnection->LoadExternalizableL(*fileFullPath, aDummyAttributes));
		if( KErrNone != loadError)
		{
			unknown = REComPlusSession::CreateImplementationL(TUid::Uid(KCID_MProperties), TUid::Uid(aInterfaceId) );
		}
		CleanupReleasePushL(*unknown);

		MUnknown * desiredInterface = unknown->QueryInterfaceL( aInterfaceId );

		CleanupStack::PopAndDestroy(unknown);
		CleanupStack::PopAndDestroy(fileFullPath);
		CleanupStack::PopAndDestroy(fileName);
		CleanupStack::PopAndDestroy(&fs);
		CleanupStack::PopAndDestroy(directoryFullPath);
		return desiredInterface;
	}
	else if( KStatementTypeCommit == iStatementType )
	{
		MUnknown * unknown = GetObjectL(KPropertyObjectItem, KIID_MUnknown, 0);
		CleanupReleasePushL(*unknown);


		MProperties * properties = 0;
		TRAPD(queryError, properties = QiL( unknown, MProperties ) );
		if( KErrNone == queryError )
		{
			CleanupReleasePushL(*properties);

			// This implementation assumes it is the info
			// for the system itself, an account or a folder.
			// We set the account and folder values when handing this object out,
			// and we use those values now to determine
			// whether this is folder, account or entire messaging properties.
			// We totally ignore the account and folder settings on this 
			// statement itself.

			const TDesC * itemAccount = 0;
			TRAPD( failureIgnored, itemAccount = &( properties->GetStringL(KPropertyStringAccount, 0) ) );

			const TDesC * itemFolder = 0;
			TRAP( failureIgnored, itemFolder = &( properties->GetStringL(KPropertyStringFolder, 0) ) );

			if( itemAccount )
			{
				HBufC* directoryFullPath = iDataAccessConnection->CreateDirectoryFullPathLC( CDataAccessConnection::EStorageInfo, *itemAccount, KNullDesC);
				HBufC* fileName;
				RFs fs;
				User::LeaveIfError(fs.Connect());
				CleanupClosePushL(fs);
				CDir* files;
				CDir *dirs;
				if(fs.GetDir(*directoryFullPath,KEntryAttNormal,ESortByDate,files,dirs) == KErrNone)
				{
					if(files && files->Count() > 0)
					{
						TEntry info = (*files)[files->Count()-1];
						fileName = info.iName.AllocLC();
					}
					else
					{
						fileName = KInfoFileName().AllocLC();
					}
				}
				else
				{
					fileName = KInfoFileName().AllocLC();
				}
				
				delete dirs;
				delete files;
				CleanupStack::PopAndDestroy(fileName);
				CleanupStack::PopAndDestroy(&fs);
				CleanupStack::PopAndDestroy(directoryFullPath);
			}
			else
			{
				if(folder)
				{
					iDataAccessConnection->PersistRootInfoL(properties,*folder);
				}
			}
			
			CleanupStack::PopAndDestroy(properties);
		}


		// TODO: Check to see if they specified by range instead.

		CleanupStack::PopAndDestroy(unknown);

		return 0;
	}
	else
	{
		User::Leave(KErrNotSupported);
		return 0;
	}
}


