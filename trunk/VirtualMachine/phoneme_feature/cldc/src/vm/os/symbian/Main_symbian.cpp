/*
 *   
 *
 * Copyright  1990-2006 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation. 
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt). 
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA 
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions. 
 */

#include "incls/_precompiled.incl"
#include "incls/_Main_symbian.cpp.incl"
#include <stdio.h>
#include <stdlib.h>
#include <ecom/implementationproxy.h>
#include <EcomPlusRefCountedBase.h> // Copyright Michael Maguire but in public domain -- see http://newlc.com/ECOMPLUS.html
#include <OSVersion.h>
#include <midpCommandState.h>
#include <midpams.h>

extern "C" void DrawSplashScreen();

#if ENABLE_PCSL
extern "C" {
#include <pcsl_memory.h>
#include <pcsl_print.h>
#include <pcsl_file.h>
#include <midpEvents.h>
#include <gxj_putpixel.h>
#include <lcdlf_export.h>
#include <midpMalloc.h>
}
#endif

#include <midp_properties_port.h>

//#define __DEBUGMAINSYMBIAN__

#ifdef __DEBUGMAINSYMBIAN__
#define DEBUGPRINT(_XX) {pcsl_print(_XX);}
#define DEBUGPRINT1(_XX,_YY) {RDebug::Print(_XX,_YY);}
#else
#define DEBUGPRINT(_XX)
#define DEBUGPRINT1(_XX,_YY)
#endif

gxj_screen_buffer gxj_system_screen_buffer;

void JVMSPI_PrintRaw(const char* s) {
#if ENABLE_PCSL
  pcsl_print(s);
#else
  printf("%s", s);
#endif
}

void JVMSPI_Exit(int code) 
{
  exit(code);
}

int resizeScreenBuffer() 
{
	int result = KErrNone;
    int newWidth = lcdlf_get_screen_width();
    int newHeight = lcdlf_get_screen_height();
    int newScreenSize = sizeof(gxj_pixel_type) * newWidth * newHeight;

	if ((newWidth != gxj_system_screen_buffer.width) || (newHeight != gxj_system_screen_buffer.height))
	{

		gxj_system_screen_buffer.width = newWidth;
		gxj_system_screen_buffer.height = newHeight;
		gxj_system_screen_buffer.alphaData = NULL;

		gxj_system_screen_buffer.pixelData = (gxj_pixel_type *)static_cast<MApplication*>(Dll::Tls())->CreateScreen(newWidth,newHeight);
	}

    if (gxj_system_screen_buffer.pixelData == NULL) 
	{
		result = KErrNoMemory;
    } 
	else 
	{
        memset(gxj_system_screen_buffer.pixelData, 255, newScreenSize);
    }
	return result;
}


#if ENABLE_DYNAMIC_RESTRICTED_PACKAGE
jboolean JVMSPI_IsRestrictedPackage(const char* pkg_name, int name_length) {
  GUARANTEE(UseROM, "sanity");

  // This is just a sample implementation. Please modify it for
  // an actual deployment.
  if (name_length == 8 && strncmp(pkg_name, "com/abcd", 8) == 0) {
    // Don't allow any class in the package of "com.abcd".
    return true;
  }
  if (name_length > 8  && strncmp(pkg_name, "com/abcd/", 9) == 0) {
    // Don't allow any class in a subpackage of "com.abcd".
    return true;
  }
  // Note that classes in an related package, such as com.abcdefg.MyClass, 
  // are allowed.
  return false;
}
#endif

/**
 * The implementation id of a default component which implements the MtestInterface interface.
 */
const TInt KCID_PhonemeFeatureVirtualMachine = 0x10240D69;
const TUid KImplUid_PhonemeFeatureVirtualMachine = {KCID_PhonemeFeatureVirtualMachine};





const TInt KIID_MVirtualMachine = 0x59332912;

class MVirtualMachine : public MUnknown
{
public:
	virtual void RunVMCode(const JvmPathChar * aClassPath,char* aMainClass,MApplication* aApp,const char* aMIDlet) = 0;

protected:
	virtual ~MVirtualMachine() {}
};



class CVirtualMachine : public CEComPlusRefCountedBase, public MVirtualMachine
{
public:
	static MVirtualMachine * NewL(TAny * aConstructionParameters );

	virtual void RunVMCode(const JvmPathChar * aClassPath,char* aMainClass,MApplication* aApp,const char* aMIDlet);

	MUnknown * QueryInterfaceL( TInt aInterfaceId);
	void AddRef() { CEComPlusRefCountedBase::AddRef(); }
	void Release() { CEComPlusRefCountedBase::Release(); }

protected:
	CVirtualMachine(TAny * aConstructionParameters);
	void ConstructL();
};

LOCAL_C char* GetFullPath(const TDesC& aPath)
{
    char* utf8 = NULL;
	TFileName fullPath = static_cast<MApplication*>(Dll::Tls())->GetFullPath(aPath);

	int fullLength = fullPath.Length();
	HBufC8* fullPath8 = HBufC8::New(fullLength);
	if (fullPath8)
	{
        fullPath8->Des().Copy(fullPath);
		utf8 = (char*)midpMalloc(fullLength + 1);
		if (utf8)
		{
			Mem::Copy(utf8, fullPath8->Ptr(), fullLength);
			utf8[fullLength] = 0;
		}
		delete fullPath8;
	}
    return utf8;
}


// ECOM: Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KCID_PhonemeFeatureVirtualMachine,	CVirtualMachine::NewL)
	};

// ECOM: Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

EXPORT_C CVMArguments* CVMArguments::NewLC()
{
    CVMArguments* self = new (ELeave) CVMArguments();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CVMArguments::CVMArguments()
{}

CVMArguments::~CVMArguments()
{
    iProperty->Reset();
    delete iProperty;
}

void CVMArguments::AddL(const TDesC8& aProperty)
{
    iProperty->AppendL(aProperty);
}

const TInt CVMArguments::Count()
{
    return iProperty->Count();
}

void CVMArguments::ConstructL()
{
    iProperty = new (ELeave) CDesC8ArrayFlat(4);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C CVMProperties* CVMProperties::NewLC()
{
    CVMProperties* self = new (ELeave) CVMProperties();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CVMProperties::CVMProperties()
{}

CVMProperties::~CVMProperties()
{
    iKey->Reset();
    delete iKey;
}

void CVMProperties::ConstructL()
{
    CVMArguments::ConstructL();
    iKey = new (ELeave) CDesC8ArrayFlat(4);
    
}

EXPORT_C void CVMProperties::AddL(const TDesC8& aKey,const TDesC8& aProperty)
{
    iKey->AppendL(aKey);
    iProperty->AppendL(aProperty);
}

EXPORT_C const TInt CVMProperties::Count()
{
    return iKey->Count();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
EXPORT_C TInt RunVMCode(const JvmPathChar * aClassPath,char* aMainClass,CVMProperties& aProperties,CVMArguments& aArguments,MApplication* aApp,const char* aMIDlet)
{
	TInt status = -1;
	if(aApp)
	{
		Dll::SetTls((TAny*) aApp);
		status = resizeScreenBuffer();
		DrawSplashScreen();
		aApp->Refresh(0, 0, lcdlf_get_screen_width(), lcdlf_get_screen_height());
		if (status != KErrNone)
			{
			return status;
			}

		InitializeEvents();
		pcsl_mem_initialize(NULL, 0);
		pcsl_file_init();
 		JVM_Initialize();
		JVM_SetConfig(JVM_CONFIG_HEAP_CAPACITY, KMaxHeap);
		JVM_SetConfig(JVM_CONFIG_HEAP_MINIMUM, KMinHeap);
		JVM_SetConfig(JVM_CONFIG_SLAVE_MODE,false);
        TInt count = aProperties.Count();
        RBuf8 keyBuffer(NULL);
        RBuf8 propertyBuffer(NULL);
        for(TInt i=0;i<count;i++)
        {
            TPtrC8 key(aProperties.Key(i));
            TPtrC8 property(aProperties.Property(i));
            
            keyBuffer.Create(key.Length() + 1);
            keyBuffer.Copy(key);
            propertyBuffer.Create(property.Length() + 1);
            propertyBuffer.Copy(property);
            JVMSPI_SetSystemProperty((char*)keyBuffer.PtrZ(),(char*)propertyBuffer.PtrZ());
            keyBuffer.Close();
            propertyBuffer.Close();
		}
        
		initializeConfig();
        count = aArguments.Count();
        char* argument[1];
        for(TInt i=0;i<count;i++)
        {   
            TPtrC8 property(aArguments.Property(i));
            propertyBuffer.Create(property.Length() + 1);
            propertyBuffer.Copy(property);
            argument[0] = (char*)propertyBuffer.PtrZ();
            JVM_ParseOneArg(1,argument);
            propertyBuffer.Close();
        }

		char* appPath = GetFullPath(_L("app"));
		midpSetAppDir(appPath);
        char* resPath = GetFullPath(_L("vm\\resources"));
		midpSetConfigDir(resPath);
		midpInitialize();

		MIDPCommandState* commandState = midpGetCommandState();

		int len = strlen(aMIDlet);
		pcsl_string_convert_from_utf8((const signed char*)aMIDlet,len,&commandState->midletClassName);
		
		const jbyte arg1[] = "http://www.bluewhalesystems.com/download/xhtml/bluewhalemail_midlet.jad";
		len = strlen((const char *)arg1);
		pcsl_string_convert_from_utf8(arg1,len,&commandState->arg1);
		
		const jbyte arg2[] = "\\private\\2000E27A\\vm";
		len = strlen((const char *)arg2);
		pcsl_string_convert_from_utf8(arg2,len,&commandState->arg2);
		
		commandState->suiteId = -1;
		status = JVM_Start(aClassPath,aMainClass, 0,NULL);
        DEBUGPRINT1(_L("JVM_Start %d"),status);
        // MIDP did a graceful exit, so return OK back to call application
        if(status == MAIN_EXIT)
        {
            status =0;
        }
		midpFree(appPath);
        midpFree(resPath);
        JVMSPI_FreeAllSystemProperties();
		lcdlf_ui_finalize();
		FinalizeEvents();
		pcsl_mem_finalize();
		pcsl_file_finalize();
		midp_suite_storage_cleanup();
		finalizeConfig();
		storageFinalize();
		FinalizeEvents();
        midpFinalize();
		static_cast<MApplication*>(Dll::Tls())->CreateScreen(0,0);
        gxj_system_screen_buffer.pixelData = NULL;
	}
	return status;
}



CVirtualMachine::CVirtualMachine(TAny * aConstructionParameters)
: CEComPlusRefCountedBase(aConstructionParameters)
{
}

void CVirtualMachine::ConstructL()
{
}

MVirtualMachine * CVirtualMachine::NewL(TAny * aConstructionParameters )
{
	CVirtualMachine * self = new (ELeave) CVirtualMachine(aConstructionParameters );
	CleanupStack::PushL(self);
	self->ConstructL();
	
	MVirtualMachine * virtualMachineInterface = QiL( self, MVirtualMachine);

	CleanupStack::Pop(self);
	return virtualMachineInterface;
}

void CVirtualMachine::RunVMCode(const JvmPathChar * aClassPath,char* aMainClass,MApplication* aApp,const char* aMIDlet)
{
	RunVMCode(aClassPath,aMainClass,aApp,aMIDlet);
}


MUnknown * CVirtualMachine::QueryInterfaceL(TInt aInterfaceId )
{
	if( KIID_MVirtualMachine == aInterfaceId )
	{
		AddRef(); // Remember to do this in your subclasses for every QI.
		return static_cast<MVirtualMachine*>(this);
	}
	else
	{
		//Handy to test for not leaking if component forgets to implement this interface.
		//User::Leave(KErrNotSupported);
		//return 0;
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

#if (__S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__) && (__UIQ_VERSION_NUMBER__ < __UIQ_V3_FP0_VERSION_NUMBER__)
#ifdef __WINS__
GLDEF_C TInt E32Dll(TDllReason)
{
    return KErrNone;
}
#endif
#endif
