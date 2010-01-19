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


#include "DataAccessConnectionImpl.h"
#include "DataAccessStatementImpl.h"
#include "DebugUtilities.h"
#include "EcomPlus.h"
#include "Message.h"
#include "Folder.h"
#include "EmailMessage.h"
#include "CommandMessage.h"
#include "Application.h"
//#include "AdBase.h"

#ifdef _DEBUG_DATAACCESSCONNECTION_
#define DEBUGPRINT(_XX) RDebug::Print(_XX,);
#define DEBUGPRINT1(_XX,_YY) RDebug::Print(_XX,_YY);
#else
#define DEBUGPRINT(_XX)
#define DEBUGPRINT1(_XX,_YY)
#endif

// These are private to this file for a reason -- they should never be used directly, rather
// CreateDirectoryFullPathLC should be used to obtain the correct value.

#if (__S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)

_LIT( KInfoStorageDirectory, "store\\I\\");
_LIT( KRootStorageDirectory, "store\\");
_LIT( KRootInfoStorageDirectory, "store\\I\\");

#else // __S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__

#if __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

_LIT( KMessageStorageDirectory, "c:\\system\\apps\\bluewhalemail\\messaging\\");
_LIT( KInfoStorageDirectory, "c:\\system\\apps\\bluewhalemail\\messaging\\__INFO\\");
_LIT( KRootStorageDirectory, "c:\\system\\apps\\bluewhalemail\\messaging\\");
_LIT( KRootInfoStorageDirectory, "c:\\system\\apps\\bluewhalemail\\messaging\\__INFO\\");

#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

_LIT( KMessageStorageDirectory, "c:\\system\\messaging\\");
_LIT( KInfoStorageDirectory, "c:\\system\\messaging\\__INFO\\");
_LIT( KRootStorageDirectory, "c:\\system\\messaging\\");
_LIT( KRootInfoStorageDirectory, "c:\\system\\messaging\\__INFO\\");

#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

#ifdef __WINS__
_LIT( KAttachmentStorageDirectory, "c:\\Attachments\\");
#else /* not __WINS__ */
_LIT( KAttachmentStorageDirectory, "e:\\Attachments\\");
#endif /* __WINS__ */

#endif // __S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__

_LIT( KTempFileSuffix, ".TEMP");


const TInt32 KTUid1ForMExternalizable = 0x1020DFFC;	// This is an allocated Uid.
const TInt32 KMessageStoreFilePersistVersion = 0x00000001;

#ifdef __WINS__
EXPORT_C
#endif
MDataAccessConnection * CDataAccessConnection::NewL(TAny * aConstructionParameters)
{
	CDataAccessConnection * self = new (ELeave) CDataAccessConnection(aConstructionParameters);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	MDataAccessConnection * dae = QiL( self, MDataAccessConnection );

	CleanupStack::Pop(self);
	return dae;
}

CDataAccessConnection::CDataAccessConnection(TAny * aConstructionParameters)
: CEComPlusRefCountedBase(aConstructionParameters)
{
	DEBUG_CONSTRUCTOR
}


void CDataAccessConnection::ConstructL()
{
	CEComPlusRefCountedBase::ConstructL();

	User::LeaveIfError( iFs.Connect() );

	iInfoStorageDirectoryFullPathEndingWithSeparator = CVMManager::FullPath(KInfoStorageDirectory()).AllocL();
	iRootStorageDirectoryFullPathEndingWithSeparator = CVMManager::FullPath(KRootStorageDirectory()).AllocL();
	iRootInfoStorageDirectoryFullPathEndingWithSeparator = CVMManager::FullPath(KRootInfoStorageDirectory()).AllocL();
	iTempStorageDirectoryFullPathEndingWithSeparator = CVMManager::FullPath(KTempStorageDirectory()).AllocL();
}


CDataAccessConnection::~CDataAccessConnection()
{
	DEBUG_DESTRUCTOR

	delete iInfoStorageDirectoryFullPathEndingWithSeparator;
	delete iRootStorageDirectoryFullPathEndingWithSeparator;
	delete iRootInfoStorageDirectoryFullPathEndingWithSeparator;
	delete iTempStorageDirectoryFullPathEndingWithSeparator;

	iFs.Close();
}

MDataAccessStatement * CDataAccessConnection::CreateStatementL()
{
	MDataAccessStatement * statement = CDataAccessStatement::NewL(this);
	CleanupReleasePushL(*statement);

	// Do some stuff to initialized the statement.

	// We don't keep track of our statements -- they AddRef on us keep track of us.
	CleanupStack::Pop(statement); 
	return statement;
}

MUnknown * CDataAccessConnection::QueryInterfaceL(TInt aInterfaceId )
{
	if( KIID_MDataAccessConnection == aInterfaceId )
	{
		AddRef(); // Remember to do this in your subclasses for every QI.
		return static_cast<MDataAccessConnection*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

void CDataAccessConnection::PersistExternalizableL( RWriteStream & aWriteStream /* IN */, MExternalizable * aExternalizable /* IN */, TUint32 /*aAttributes*/ /* IN */)
{
	TUid id(aExternalizable->GetECOMPlusComponentId());

	aWriteStream.WriteUint32L(id.iUid);
	aWriteStream.WriteUint8L(aExternalizable->GetVersion());
	aExternalizable->ExternalizeL(aWriteStream);
}


MUnknown * CDataAccessConnection::LoadExternalizableL( const TDesC & aFullPathFileName, TUint32 & aAttributes)
{
	return LoadExternalizableL( iFs, aFullPathFileName,aAttributes);
}


MUnknown * CDataAccessConnection::LoadExternalizableL( RFs & aFs, const TDesC & aRelativeFileName /* IN */, TUint32 & /*aAttributes*/)
{
	RFile readFile;
	User::LeaveIfError( readFile.Open(aFs,aRelativeFileName,EFileRead) );
	CleanupClosePushL(readFile);

	TTime modified;
	User::LeaveIfError(readFile.Modified(modified));

	RFileReadStream readStream(readFile);
	CleanupClosePushL(readStream);

	TUid id = TUid::Uid(readStream.ReadUint32L());
	/*TUint8 version = */ readStream.ReadUint8L();

	MUnknown* unknown = REComPlusSession::CreateImplementationL(id ,TUid::Uid(KIID_MUnknown), NULL);
	
	MExternalizable* externalizable = QiL(unknown,MExternalizable);
	CleanupReleasePushL(*externalizable);
	
	externalizable->InternalizeL(readStream);

	CleanupStack::PopAndDestroy(externalizable);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&readFile);

	return unknown;
}

MMessage * CDataAccessConnection::LoadMessageL( RFs & aFs, const TDesC & aAccount, const TDesC & aFolder, const TDesC & aRelativeFileName /* IN */)
{
	TUint32 attributes = 0 ;
	MUnknown * unknown = LoadExternalizableL(aFs, aRelativeFileName, attributes);
	CleanupReleasePushL(*unknown);

	MMessage * message = QiL(unknown, MMessage);
	CleanupStack::PopAndDestroy(unknown);
	CleanupReleasePushL(*message);

	TMessageEntry messageFileName(aRelativeFileName, attributes);
	message->SetTime(messageFileName.GetCreationTime());
	message->SetMessageId(messageFileName.GetMessageId());
	message->SetAttributes(attributes);
	message->SetAccountL(aAccount);
	message->SetFolderL(aFolder);

	CleanupStack::Pop(message);
	return message;
}

void CDataAccessConnection::PersistRootInfoL(MProperties * aInfo,const TDesC& aFolder)
{
	HBufC * directoryFullPath = CreateDirectoryFullPathLC( EStorageRootInfo, KNullDesC, aFolder);
	HBufC * tempFileName = CreateTempFileNameLC(KInfoFileName);
	PersistExternalizableL(*directoryFullPath, *tempFileName, *directoryFullPath, KInfoFileName, aInfo, 0);
	CleanupStack::PopAndDestroy(tempFileName);
	CleanupStack::PopAndDestroy(directoryFullPath);
	
}


void CDataAccessConnection::ResetEntireMessagingStoreL()
{
	CFileMan * fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);

	// TODO

	CleanupStack::PopAndDestroy(fileMan);
}



/**
 * Doesn't really matter whether a full or relative path name.
 */
HBufC * CDataAccessConnection::CreateTempFileNameLC(const TDesC & aFileName) const
{
	HBufC * fullPathTempFileName = HBufC::NewLC(aFileName.Length() + KTempFileSuffix().Length() );
	TPtr ptr = fullPathTempFileName->Des();
	ptr.Copy(aFileName);
	ptr.Append(KTempFileSuffix);
	return fullPathTempFileName;
}



TBool CDataAccessConnection::PersistExternalizableL( const TDesC & aTempDirectoryPath, const TDesC & aTempRelativeFileName,  const TDesC & aDirectoryPath, const TDesC & aRelativeFileName, MUnknown * aObjectToExternalized, TUint32 aAttributes, TBool forceWriteEvenIfNotDirty )
{
	TBool returnValue = EFalse;
	MExternalizable * externalizable = QiL( aObjectToExternalized, MExternalizable );
	CleanupReleasePushL(*externalizable);

	if( externalizable->Dirty() || forceWriteEvenIfNotDirty )
	{
		// For atomicity, always write to a temporary file, then
		// when completed, replace old file with that new one.
		HBufC * tempFullPathFileName = CreateFileFullPathLC(aTempDirectoryPath, aTempRelativeFileName);

		// Create the full path file name.
		HBufC * fullPathFileName = CreateFileFullPathLC(aDirectoryPath, aRelativeFileName);

		// Attempt to open the file.  Overwrite existing.
		RFile persistFile;
		TInt error = persistFile.Replace(iFs,*tempFullPathFileName,EFileWrite);
		if( KErrPathNotFound == error )
		{
			// The directory doesn't exist yet so create it
			User::LeaveIfError( iFs.MkDirAll(aTempDirectoryPath) );
			User::LeaveIfError( persistFile.Create(iFs,*tempFullPathFileName,EFileWrite) );
		}
		else if( KErrNone != error )
		{
			User::Leave(error);
		}

		CleanupClosePushL(persistFile);
		
		// Open a write stream on the file.
		RFileWriteStream writeStream(persistFile);
		CleanupClosePushL(writeStream);

		PersistExternalizableL(writeStream, externalizable, aAttributes);

		CleanupStack::PopAndDestroy(&writeStream);
		CleanupStack::PopAndDestroy(&persistFile);

		// Move the newly created file over on top of the old file.
		error = iFs.Replace(*tempFullPathFileName, *fullPathFileName);
		if( KErrPathNotFound == error )
		{
			// The directory doesn't exist yet so create it
			User::LeaveIfError( iFs.MkDirAll(aDirectoryPath) );
			User::LeaveIfError( iFs.Replace(*tempFullPathFileName, *fullPathFileName) );
		}
		else if( KErrNone != error )
		{
			User::Leave(error);
		}
		else
		{
			// Success!
		}

		CleanupStack::PopAndDestroy(fullPathFileName);
		CleanupStack::PopAndDestroy(tempFullPathFileName);

		// If we got to this step, we know we've successfully written the item out.
		externalizable->SetDirty(EFalse);
		returnValue = ETrue;
	}
	CleanupStack::PopAndDestroy(externalizable);
	return returnValue;
}



HBufC * CDataAccessConnection::CreateFileFullPathLC( const TDesC & aDirectoryFullPath, const TDesC & aFileName)
{
	HBufC * fileFullPath = HBufC::NewLC( aDirectoryFullPath.Length() + aFileName.Length() );
	TPtr ptr = fileFullPath->Des();

	ptr.Copy(aDirectoryFullPath);
	ptr.Append(aFileName);

	return fileFullPath;
}


/**
 * Creates a '\' terminated full directory path name.
 *
 * Note: assumes iStorageDirectoryFullPathEndingWithSeparator is a full path including trailing '\' separator.
 *
 * aStorageType can be used to place files e.g. in a "__TEMP" or "__INFO" subdirectory.
 */
HBufC * CDataAccessConnection::CreateDirectoryFullPathLC( CDataAccessConnection::TStorageType aStorageType, const TDesC & aAccount, const TDesC & aFolder, TInt aPaddingSuffix )
{
	HBufC * prefix = 0;
	switch( aStorageType )
	{
	case EStorageInfo:
		prefix = iInfoStorageDirectoryFullPathEndingWithSeparator;
		break;
	case EStorageRoot:
		prefix = iRootStorageDirectoryFullPathEndingWithSeparator;
		break;
	case EStorageRootInfo:
		prefix = iRootInfoStorageDirectoryFullPathEndingWithSeparator;
		break;
	case EStorageTemp:
		prefix = iTempStorageDirectoryFullPathEndingWithSeparator;
		break;
	default:
		User::Leave(KErrArgument);
		break;
	}

	TInt prefixLength = prefix->Length();

	TInt accountLength = aAccount.Length();
	if( accountLength )
	{
		accountLength += KDirectorySeparator().Length(); // Extra size needed for the '\' separator.
	}
	TInt folderLength = aFolder.Length();
	if( folderLength )
	{
		folderLength += KDirectorySeparator().Length(); // Extra size needed for the '\' separator.
	}

	HBufC * directoryFullPath = HBufC::NewLC( prefixLength + accountLength + folderLength + aPaddingSuffix );
	TPtr directoryFullPathPtr = directoryFullPath->Des();
	directoryFullPathPtr.Copy(*prefix); // prefix assumed to already end in '\'
	if( accountLength )
	{
		directoryFullPathPtr.Append(aAccount);
		if(folderLength)
		{
			directoryFullPathPtr.Append(KDirectorySeparator);
		}
		else
		{
			directoryFullPathPtr.Append(KDirectoryTerminator);
		}
	}
	if( folderLength )
	{
		directoryFullPathPtr.Append(aFolder);
		directoryFullPathPtr.Append(KDirectoryTerminator);
	}

	return directoryFullPath;
}


void CDataAccessConnection::GetOriginalRelativeFileNameForMessage( TDes & aName /* OUT */, MMessage * aMessage /* IN */)
{
	TMessageEntry messageFileName(aMessage->OriginalTime(), aMessage->OriginalMessageId(), aMessage->OriginalAttributes() );
	messageFileName.CreateRelativeFileNameForMessage( aName );
}


void CDataAccessConnection::CreateRelativeFileNameForMessage( TDes & aName, MMessage * aMessage)
{
	TMessageEntry messageFileName( aMessage->Time(), aMessage->MessageId(), aMessage->Attributes() );
	messageFileName.CreateRelativeFileNameForMessage( aName );
}

TBool CDataAccessConnection::AttributesDirtyL(MMessage * message)
{
	if( message->Attributes() != message->OriginalAttributes() )
	{
		return ETrue;
	}
	return EFalse;
}

TBool CDataAccessConnection::NameDirtyL(MMessage * message)
{
	if( message->Time() != message->OriginalTime() )
	{
		return ETrue;
	}
	if( message->MessageId() != message->OriginalMessageId() )
	{
		return ETrue;
	}
	if( message->AccountL().CompareF(message->OriginalAccountL()) )
	{
		return ETrue;
	}
	if( message->FolderL().CompareF(message->OriginalFolderL()) )
	{
		return ETrue;
	}

	return EFalse;
}

