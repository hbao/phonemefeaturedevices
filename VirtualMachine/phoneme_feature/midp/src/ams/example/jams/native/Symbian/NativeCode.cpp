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
 
#include "kni.h"
#include "sni.h"
#include <OS_Symbian.hpp>
#include <pcsl_string.h>
#include <midpMidletSuiteUtils.h>
#include <midpResourceLimit.h>
#include <midpServices.h>

extern "C" {
#include <midp_foreground_id.h>
#include <commonKNIMacros.h>
}

#include <msvapi.h>
#include <smut.h>
#include <SMSCLNT.h> 
#include <mtclreg.h> 
#include <msvstd.h> 
#include <txtrich.h>
#include <SMUTHDR.h> 
#include <badesca.h>
#include <midpMalloc.h>
#include <OSVersion.h>
#include <pcsl_memory.h>
#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__)
#include <swinstapi.h>
#elif (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#else
#include <apacmdln.h>
#include <eikdll.h>
#endif

_LIT(KShortcutsPath, "vm\\shortcuts\\");

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_main_BWMDisplayController_requestBackground0()
{
	int displayId;
	displayId = KNI_GetParameterAsInt(1);
	static_cast<MApplication*>(Dll::Tls())->RequestOrdinalChange(displayId,EFalse);
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_main_BWMDisplayController_requestForeground0()
{
	int displayId = KNI_GetParameterAsInt(1);
	gForegroundIsolateId = KNI_GetParameterAsInt(2);
	gForegroundDisplayId = displayId;
		
	static_cast<MApplication*>(Dll::Tls())->RequestOrdinalChange(displayId,ETrue);
}

/** 
 * We're not really interested in listening for changes at all - if the user
 * managed to install the app, they've definitely already received the SMS
 * install SMS with the appropriate link.
 */
class CDummyMsvSessionObserver : public MMsvSessionObserver
{
public:
	void HandleSessionEventL(TMsvSessionEvent /* aEvent */, TAny* /* aArg1 */, TAny* /* aArg2 */, TAny* /* aArg3 */)
	{

	}
};

char* getInstallSMSBodyL(const TDesC& aFrom)
{
	TBuf<200> from;
	HBufC* body = NULL;

	CDummyMsvSessionObserver* dummy = new (ELeave) CDummyMsvSessionObserver();
	CleanupStack::PushL(dummy);
	CMsvSession* session = CMsvSession::OpenSyncL(*dummy);
	CleanupStack::PushL(session);

	CClientMtmRegistry* mtmRegistry = CClientMtmRegistry::NewL(*session);
	CleanupStack::PushL(mtmRegistry);

	CMsvEntry* entry = session->GetEntryL(KMsvGlobalInBoxIndexEntryId);
	CleanupStack::PushL(entry);
	if (0 < entry->Count())
	{
		TMsvSelectionOrdering sortType;
		sortType.SetSorting(EMsvSortByDateReverse); // Date (latest-earliest)
		entry->SetSortTypeL(sortType);

		CMsvEntrySelection* entries = entry->ChildrenL();
		CleanupStack::PushL(entries);

		CSmsClientMtm* smsClientMtm = STATIC_CAST(CSmsClientMtm*,mtmRegistry->NewMtmL(KUidMsgTypeSMS));
		CleanupStack::PushL(smsClientMtm);

		bool done = false;
		for (TInt i = 0; i < entries->Count() && !done; i++)
		{
			const TMsvEntry & childEntryReference = entry->ChildDataL(entries->At(i));
			if (KUidMsgTypeSMS == childEntryReference.iMtm)
			{
				smsClientMtm->SwitchCurrentEntryL(childEntryReference.Id());
				smsClientMtm->LoadMessageL();

				// Try first obtaining from address this way.
				from.Copy(smsClientMtm->SmsHeader().Message().ToFromAddress());
				if (!from.Length())
				{
					// Try this way.
					if (0 < smsClientMtm->AddresseeList().Count())
					{
			
#if (__S60_VERSION__ > __S60_V2_FP3_VERSION_NUMBER__) || ( __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
						from.Copy((smsClientMtm->AddresseeList()).RecipientList().MdcaPoint(0));
#else					
						from.Copy((smsClientMtm->AddresseeList()).MdcaPoint(0));
#endif
					}
				}

				if (KErrNotFound != from.FindF(aFrom))
				{
					body = smsClientMtm->Body().Read(0).AllocL();
					done = true;
				}
			}
		}

		CleanupStack::PopAndDestroy(smsClientMtm);
		CleanupStack::PopAndDestroy(entries);
	}

	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(mtmRegistry);
	CleanupStack::PopAndDestroy(session);
	CleanupStack::PopAndDestroy(dummy);
	
	char* utf8 = NULL;
	if (body)
	{
		CleanupStack::PushL(body);
		int length = body->Length();
		HBufC8* body8 = HBufC8::NewL(length);
		body8->Des().Copy(*body);
		utf8 = (char*)midpMalloc(length + 1);
		if (utf8)
		{
			Mem::Copy(utf8, body8->Ptr(), length);
			utf8[length] = 0;
		}
		delete body8;
		CleanupStack::PopAndDestroy(body);
	}

	return utf8;
}

KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_bluewhalesystems_midp_SMSTextReader_getInstallSMSBody()
{
	KNI_StartHandles(2);
	KNI_DeclareHandle(fromObject);
	KNI_GetParameterAsObject(1, fromObject);
	const int fromLength = KNI_GetStringLength(fromObject);

	char* utf8 = NULL;
	jchar* fromChars = (jchar*)pcsl_mem_malloc(fromLength * sizeof (jchar));
	if (fromChars)
	{
		KNI_GetStringRegion(fromObject, 0, fromLength, fromChars);
		HBufC16* fromParam = HBufC16::New(fromLength);
		if (fromParam)
		{
			fromParam->Des().Copy(fromChars, fromLength);
			TRAPD(ignore, utf8 = getInstallSMSBodyL(*fromParam));
			delete fromParam;
		}
		pcsl_mem_free(fromChars);
	}

	KNI_DeclareHandle(string);
	if (utf8)
	{
		KNI_NewStringUTF(utf8, string);
		midpFree(utf8);
	}
	KNI_EndHandlesAndReturnObject(string);
}

static char* UnicodeToCString(jchar* uString, int length) {
    int        i;
    char*      cString;

    if (NULL == uString) {
        return NULL;
    }

    /* Add 1 for null terminator */
    cString = (char*)pcsl_mem_malloc(length + 1);
    if (NULL == cString) {
        return NULL;
    }

    for (i = 0; i < length; i++) {
        cString[i] = (char)uString[i];
    }

    cString[length] = '\0';                /* Null-terminate C string */
    return cString;
}


KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_bluewhalesystems_midp_PlatformRequestListener_setSystemProperty0()
{
	KNI_StartHandles(2);
	KNI_DeclareHandle(key);
	KNI_DeclareHandle(val);
	KNI_GetParameterAsObject(1, key);
	KNI_GetParameterAsObject(2, val);
	const int keyLength = KNI_GetStringLength(key);
	const int valLength = KNI_GetStringLength(val);
	
	jchar* keyChars = (jchar*)pcsl_mem_malloc(keyLength * sizeof (jchar));
	if(keyChars)
	{
		KNI_GetStringRegion(key, 0, keyLength, keyChars);
		jchar* valChars = (jchar*)pcsl_mem_malloc(valLength * sizeof (jchar));
		if(valChars)
		{
			KNI_GetStringRegion(val, 0, valLength, valChars);
			char* key8 = UnicodeToCString(keyChars,keyLength);
			if(key8)
			{
				char* val8 = UnicodeToCString(valChars,valLength);
				if(val8)
				{
					JVMSPI_SetSystemProperty(key8,val8);
					pcsl_mem_free(key8);
				}
				pcsl_mem_free(val8);
			}
			pcsl_mem_free(valChars);
		}
		pcsl_mem_free(keyChars);
	}
	
	KNI_EndHandles();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_bluewhalesystems_midp_PlatformRequestListener_deleteSystemProperty0()
{
	KNI_StartHandles(1);
	KNI_DeclareHandle(key);
	KNI_GetParameterAsObject(1, key);
	const int keyLength = KNI_GetStringLength(key);
	
	jchar* keyChars = (jchar*)pcsl_mem_malloc(keyLength * sizeof (jchar));
	if(keyChars)
	{
		KNI_GetStringRegion(key, 0, keyLength, keyChars);
		char* key8 = UnicodeToCString(keyChars,keyLength);
		if(key8)
		{
			JVMSPI_FreeSystemProperty(key8);
			pcsl_mem_free(key8);
		}
		pcsl_mem_free(keyChars);
	}
	
	KNI_EndHandles();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_com_sun_midp_installer_GraphicalInstaller_00024BackgroundInstaller_install0()
{
	unsigned char* fileData = NULL;

	KNI_StartHandles(2);
	KNI_DeclareHandle(fileNameObject);
	KNI_DeclareHandle(fileDataObject);
	KNI_GetParameterAsObject(1, fileNameObject);
	KNI_GetParameterAsObject(2, fileDataObject);

	const int fileNameLength = KNI_GetStringLength(fileNameObject);

	jchar* fileNameChars = (jchar*)pcsl_mem_malloc(fileNameLength * sizeof(jchar));
	if (fileNameChars)
	{
		KNI_GetStringRegion(fileNameObject, 0, fileNameLength, fileNameChars);
		TFileName fileName;
		fileName.Copy(fileNameChars, fileNameLength);
		for (TInt i = 0; i < fileName.Length(); i++)
		{
			if (fileName[i] == '/')
			{
				fileName[i] = '\\';
			}
		}
		TParsePtrC parse(fileName);
		fileName = parse.NameAndExt();

		fileData = (unsigned char*)JavaByteArray(fileDataObject);
		int fileDataLength = KNI_GetArrayLength(fileDataObject);
	
		RFs fs;
		if (fs.Connect() == KErrNone)
		{
			CleanupClosePushL(fs);
			TFileName shortcutPath(static_cast<MApplication*>(Dll::Tls())->GetFullPath(KShortcutsPath));
			TInt ignore = fs.MkDirAll(shortcutPath);
			shortcutPath += fileName;
			RFile file;
			if (file.Replace(fs, shortcutPath, EFileWrite) == KErrNone)
			{
				TPtr8 data(fileData, fileDataLength, fileDataLength);
				file.Write(data);
				file.Close();

#ifndef __WINSCW__

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__)
				SwiUI::RSWInstSilentLauncher installer;
				if (installer.Connect() == KErrNone)
				{
					CleanupClosePushL(installer);
					SwiUI::TInstallOptions options;
					options.iUpgrade = SwiUI::EPolicyAllowed;
					options.iOCSP = SwiUI::EPolicyNotAllowed;
					TFileName location;
					TBuf<2> drive;
					Dll::FileName(location); // Get the drive letter
					TParsePtrC parse(location);
					location = parse.Drive();

					options.iDrive = location[0];
					options.iUntrusted = SwiUI::EPolicyNotAllowed;
					options.iCapabilities = SwiUI::EPolicyNotAllowed;

					SwiUI::TInstallOptionsPckg optionsPckg = options;

					TRequestStatus status;
					installer.SilentInstall(status, shortcutPath, optionsPckg);
					User::WaitForRequest(status);
					CleanupStack::PopAndDestroy(&installer);
				}
#elif (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
#else
				// start the installer
				CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
				cmdLine->SetLibraryNameL(_L("z:\\system\\apps\\appinst\\appinst.app"));
				cmdLine->SetDocumentNameL(shortcutPath);
				cmdLine->SetCommandL(EApaCommandRun);
				cmdLine->SetTailEndL(_L8("INSTALLX"));
				EikDll::StartAppL(*cmdLine);
				CleanupStack::PopAndDestroy();

				// wait for it to complete
				TFileName matchName(_L("appinst*"));
				TFindProcess finder(matchName);
				TFileName result;
				if (finder.Next(result) == KErrNone)
				{
					RProcess proc;
					if (proc.Open(finder) == KErrNone)
					{
						TRequestStatus requestStatus;
						proc.Logon(requestStatus);
						User::WaitForRequest(requestStatus);
						proc.Close();
					}
				}
#endif

#endif
			}
			CleanupStack::PopAndDestroy(&fs);
		}
	}

	KNI_EndHandles();
	KNI_ReturnVoid();
}
