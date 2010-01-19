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



#ifndef __DATA_ACCESS_CONNECTION_IMPL_H__
#define __DATA_ACCESS_CONNECTION_IMPL_H__

#include "MessagingDataAccessConnection.h"
#include <EcomPlusRefCountedBase.h>
#include <f32file.h>
#include "DestructorFunctor.h"
#include "Message.h"
#include "Folder.h"
#include "MessageEntry.h"

_LIT( KInfoFileName, "Info");
_LIT( KDirectorySeparator, "\\");
_LIT( KDirectoryTerminator, "\\");

const TUint KEntrySortKey = ESortByName | EDescending | EDirsFirst;



void TDestructorFunctor<MMessage *>::Destroy( MMessage * aMessage)
{
	aMessage->Release();
}



const TInt KIID_MDataAccessResultArrayInternalOptimized = 0x0100002F;
class MDataAccessResultArrayInternalOptimized : public MDataAccessResultArray
{
public:
	virtual TMessageEntry & OptimizedAt(TInt aIndex) = 0;
protected:
	virtual ~MDataAccessResultArrayInternalOptimized() {}
};



class CDataAccessConnection : public CEComPlusRefCountedBase, public MDataAccessConnection
{
public:
	static MDataAccessConnection * NewL( TAny * aConstructionParameters );

public: // MDataAccessConnection
	virtual MDataAccessStatement * CreateStatementL();

public: // MUnknown
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() { CEComPlusRefCountedBase::AddRef(); }
	void Release() { CEComPlusRefCountedBase::Release(); }

public:
	RFs & Fs() { return iFs; }


	typedef enum
	{
		EStorageInfo
		, EStorageRoot
		, EStorageRootInfo
		, EStorageTemp
	} TStorageType;

	HBufC * CreateDirectoryFullPathLC( TStorageType aStorageType, const TDesC & aAccount, const TDesC & aFolder, TInt aPaddingSuffix = 0);
	
	void GetOriginalRelativeFileNameForMessage( TDes & aName, MMessage * aMessage);
	void CreateRelativeFileNameForMessage( TDes & aName, MMessage * aMessage);
	
	HBufC * CreateFileFullPathLC( const TDesC & aDirectoryFullPath, const TDesC & aFileName);

	void PersistRootInfoL(MProperties * aInfo,const TDesC& aFolder);

	MUnknown * LoadExternalizableL( const TDesC & aFullPathFileName /* IN */, TUint32 & aAttributes);
	MUnknown * LoadExternalizableL( RFs & aFs, const TDesC & aRelativeFileName /* IN */, TUint32 & aAttributes);
	MMessage * LoadMessageL( RFs & aFs, const TDesC & aAccount, const TDesC & aFolder, const TDesC & aRelativeFileName);
	
	MFolder * LoadFolderL( RFs & aFs, const TDesC & aAccount, const TDesC & aParentFolderPath, const TDesC & aRelativeFolderFileName);
	void CreateFolderL(MFolder * aFolder);

	void RemoveMessageL(const TDesC & aAccount, const TDesC & aFolder, MMessage * aMessage);
	void RemoveFolderL(const TDesC & aAccount, const TDesC & aFolder);
	void RemoveAccountL(const TDesC & aAccount);
	void ResetEntireMessagingStoreL();

	
	static inline TBool AttributesDirtyL(MMessage * message);
	static inline TBool NameDirtyL(MMessage * message);

protected:
	CDataAccessConnection( TAny * aConstructionParameters );
	void ConstructL();
	virtual ~CDataAccessConnection();

	MUnknown * LoadExternalizableL( RReadStream & aReadStream /* IN */, TUint32 & aAttributes /* OUT */, const TTime & aModified /* IN */);
	void PersistExternalizableL( RWriteStream & aWriteStream /* IN */, MExternalizable * aExternalizable /* IN */, TUint32 aAttributes);

	/**
	 * Requires that the aUnknown object implement MExternalizable.
	 *
	 * This method clobbers the existing file at the specified location.
	 *
	 * This method only persists an object if it is marked as Dirty(), in which
	 * case it returns ETrue.
	 *
	 * If an object is not Dirty(), then it returns EFalse.
	 *
	 * Leaves on failure.
	 */
	TBool PersistExternalizableL( const TDesC & aTempDirectoryPath, const TDesC & aTempRelativeFileName,  const TDesC & aDirectoryPath, const TDesC & aRelativeFileName, MUnknown * aObjectToExternalized, TUint32 aAttributes, TBool forceWriteEvenIfNotDirty = EFalse);

	HBufC * CreateTempFileNameLC(const TDesC & aFileName) const;

	RFs iFs;

	HBufC * iInfoStorageDirectoryFullPathEndingWithSeparator; // Owned.
	HBufC * iRootStorageDirectoryFullPathEndingWithSeparator; // Owned.
	HBufC * iRootInfoStorageDirectoryFullPathEndingWithSeparator; //Owned
	HBufC * iTempStorageDirectoryFullPathEndingWithSeparator; // Owned.

};

#endif /* __DATA_ACCESS_CONNECTION_IMPL_H__ */
