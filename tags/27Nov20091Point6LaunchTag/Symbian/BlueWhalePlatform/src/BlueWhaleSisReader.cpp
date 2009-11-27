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


#include "BlueWhaleSisReader.h"
#include <OSVersion.h>

_LIT(KSisPattern, "*bluewhale*.sis*");
_LIT(KPersistSisFileName, "sisfilename.txt");
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
_LIT(KDownloadDirC, "c:\\system\\dmgr\\");
_LIT(KDownloadDirE, "e:\\system\\dmgr\\");
_LIT(KInstallDir, "c:\\data\\installs\\;c:\\system\\temp\\browser\\;e:\\;e:\\data\\installs\\");
#elif __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
_LIT(KInstallDir, "c:\\nokia\\installs\\;c:\\system\\install\\;e:\\;e:\\system\\install\\");
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
_LIT(KInstallDir, "c:\\system\\temp\\;d:\\system\\temp\\;d:\\");
#endif

CBlueWhaleSisReader* CBlueWhaleSisReader::NewL()
{
	CBlueWhaleSisReader* self = new (ELeave) CBlueWhaleSisReader;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CBlueWhaleSisReader::ConstructL()
{
	User::LeaveIfError(iFs.Connect());
}

CBlueWhaleSisReader::CBlueWhaleSisReader()
{
}

CBlueWhaleSisReader::~CBlueWhaleSisReader()
{
	iFs.Close();
}

void CBlueWhaleSisReader::PersistFileNameL(const TFileName& aSisFileName)
{
	iFs.MkDirAll(iPersistSisFileName);
	RFile file;
	if (file.Replace(iFs, iPersistSisFileName, EFileWrite) == KErrNone)
	{
		TBuf8<KMaxFileName> fileName8;
		fileName8.Copy(aSisFileName);
		file.Write(fileName8);
		file.Close();
	}
}

TBool CBlueWhaleSisReader::FindFileInSubDirsL(const TFileName& aDir)
{
	TBool result = EFalse;	
	CDir* dir = NULL;
	TFindFile findFile(iFs);
	CDirScan* dirScan = CDirScan::NewLC(iFs);
	dirScan->SetScanDataL(aDir, KEntryAttDir | KEntryAttMatchExclusive, ESortNone);
	TRAPD(err, dirScan->NextL(dir));
	if (err == KErrNone)
	{
		while (dir)
		{
			CDir* subDir = NULL;
			if (findFile.FindWildByPath(KSisPattern, &dirScan->FullPath(), subDir) == KErrNone && subDir->Count())
			{
				subDir->Sort(ESortByDate | EDescending);
				TInt ignore = RProperty::Set(KUidSisReaderExe, KUidSisFileName.iUid, (*subDir)[0].iName);
				PersistFileNameL((*subDir)[0].iName);
				result = ETrue;
				delete subDir;
				break;
			}
			delete dir;
			dir = NULL;
			dirScan->NextL(dir);
		}
		delete dir;
		dir = NULL;
	}
	CleanupStack::PopAndDestroy(dirScan);
	return result;
}

void CBlueWhaleSisReader::FindAndPublishSisFileNameL()
{
	TBool found = EFalse;
	TInt ignore = RProperty::Define(KUidSisReaderExe, KUidSisFileName.iUid, RProperty::EText);
	ignore = RProperty::Set(KUidSisReaderExe, KUidSisFileName.iUid, KNullDesC);
	
	TBuf<2> drive;
#ifdef __WINSCW__
	drive = _L("c:"); 
#else
	iPersistSisFileName = RProcess().FileName(); // Get the drive letter
	TParsePtrC parse(iPersistSisFileName);
	drive = parse.Drive();
#endif
	_LIT(KLitPrivatePath, "\\private\\%08x\\");
	TBuf<18> pathToUid;
	pathToUid.Format(KLitPrivatePath, KUidSisReaderExe);
	
	iPersistSisFileName = drive;
	iPersistSisFileName.Append(pathToUid);
	iPersistSisFileName.Append(KPersistSisFileName);	

	
	TFindFile findFile(iFs);

#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	found = FindFileInSubDirsL(KDownloadDirC());
	if (!found)
	{
		found = FindFileInSubDirsL(KDownloadDirE());
	}
#endif

	CDir* dir = NULL;
	if (!found && findFile.FindWildByPath(KSisPattern, &KInstallDir, dir) == KErrNone && dir->Count())
	{
		dir->Sort(ESortByDate | EDescending);
		ignore = RProperty::Set(KUidSisReaderExe, KUidSisFileName.iUid, (*dir)[0].iName);
		PersistFileNameL((*dir)[0].iName);
	}
	delete dir;
	
	if (!found)
	{
		// try to read in the name from the persist file
		RFile file;
		if (file.Open(iFs, iPersistSisFileName, EFileRead) == KErrNone)
		{
			TBuf8<KMaxFileName> fileName8;
			file.Read(fileName8);
			iPersistSisFileName.Copy(fileName8);
			ignore = RProperty::Set(KUidSisReaderExe, KUidSisFileName.iUid, iPersistSisFileName);
			file.Close();
		}
	}
}

void CBlueWhaleSisReader::ReadSisL()
{
	CBlueWhaleSisReader* sisReader = CBlueWhaleSisReader::NewL();
	CleanupStack::PushL(sisReader);
	sisReader->FindAndPublishSisFileNameL();
	CleanupStack::PopAndDestroy(sisReader);
}
	
#if !defined(EKA2) && defined(__WINS__)

EXPORT_C TInt InitEmulator()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CBlueWhaleSisReader::ReadSisL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	User::Exit(0);	
	return KErrNone;
}

GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}

#else

GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CBlueWhaleSisReader::ReadSisL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	User::Exit(0);
	return KErrNone;
}

#endif


