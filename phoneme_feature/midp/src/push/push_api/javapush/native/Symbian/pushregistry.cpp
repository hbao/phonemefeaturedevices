#include "kni.h"
#include "sni.h"
#include <OS_Symbian.hpp>
#include <pcsl_string.h>

HBufC* GetDescriptor(jstring aHandle)
{
	HBufC* buffer = NULL;
	jsize len = KNI_GetStringLength(aHandle);
	if(len > 0 )
	{
		buffer = HBufC::New(len);
		if(buffer)
		{
			KNI_GetStringRegion(aHandle,0,len,(unsigned short*)buffer->Des().Ptr());
			buffer->Des().SetLength(len);
		}
	}
	return buffer;
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_io_PushRegistry_registerConnection0()
{
	KNI_StartHandles(3);
    KNI_DeclareHandle(connectionH);
    KNI_DeclareHandle(midletH);
    KNI_DeclareHandle(filterH);
	
	KNI_GetParameterAsObject(1,connectionH);
	KNI_GetParameterAsObject(2,midletH);
	KNI_GetParameterAsObject(3,filterH);
	
	HBufC* connection = GetDescriptor(connectionH);
	HBufC* midlet = GetDescriptor(midletH);
	HBufC* filter = GetDescriptor(filterH);
	
	if(! static_cast<MApplication*>(Dll::Tls())->RegisterConnection(*connection,*midlet,*filter))
	{
		KNI_ThrowNew("javax.microedition.io.ConnectionNotFoundException", "Connection not supported");
	}
	
	delete connection;
	delete midlet;
	delete filter;
	
	KNI_EndHandles();
}

KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_javax_microedition_io_PushRegistry_unregisterConnection0()
{
	bool ret;
	KNI_StartHandles(1);
    KNI_DeclareHandle(connectionH);
	KNI_GetParameterAsObject(1,connectionH);
	HBufC* connection = GetDescriptor(connectionH);
	ret = static_cast<MApplication*>(Dll::Tls())->UnregisterConnection(*connection);
	delete connection;
	KNI_EndHandles();
	KNI_ReturnBoolean(ret);
 }
