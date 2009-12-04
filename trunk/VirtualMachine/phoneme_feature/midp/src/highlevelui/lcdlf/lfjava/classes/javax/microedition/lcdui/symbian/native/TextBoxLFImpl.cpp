#include <e32std.h>
extern "C" {
#include <midpMalloc.h>
#include <midpEvents.h>
#include <midpEventUtil.h>
#include <midpError.h>
}
#include <midputilkni.h>
#include <pcsl_memory.h>
#include <ROMStructs.h>
#include <stdio.h>
#include <OS_symbian.hpp>

#define __DEBUGTEXTBOXLFIMPL__
#ifdef __DEBUGTEXTBOXLFIMPL__
	#include <pcsl_print.h>
	class Debug
	{
		public:
		static void _print(char* aStr)
		{
			pcsl_print(aStr);
		}
		static void _print(HBufC* aStr)
		{
			if(aStr->Length() > 0)
			{
				HBufC8* buf8 = HBufC8::New(aStr->Length());
				if(buf8)
				{
					buf8->Des().Copy(aStr->Des());
					pcsl_print((const char*)buf8->Des().PtrZ());
					delete buf8;
				}
			}
		}
	};
	#define DEBUGMSG(_AA) {Debug::_print(_AA);}
#else
	#define DEBUGMSG(_AA)	
#endif

static HBufC* JBufferToHBufC(jcharArray java_arr, jint length) 
{
	HBufC* ret = NULL;
    if (KNI_IsNullHandle(java_arr)) 
	{
        return KNullDesC().Alloc();;
    } 
	else if (length < 0) 
	{
        return NULL;
    } 
	else if (length == 0) 
	{
        return KNullDesC().Alloc();;
    } 
	else 
	{
        jchar * buffer = static_cast<jchar *>(pcsl_mem_malloc(length * sizeof(jchar)));
		if (buffer == NULL) 
		{
            return NULL;
        }

        KNI_GetRawArrayRegion(java_arr, 0,length * sizeof(jchar),(jbyte *) buffer);
		ret = HBufC::New(length);
		if(ret)
		{
			TPtr ptr(ret->Des());
			ptr.Copy(static_cast<const TUint16 *>(buffer),length);
		}
		pcsl_mem_free(buffer);
    }
	return ret;
}



#define cppunhand(_type,_ptr)  (*static_cast<_type**>(((void*)_ptr )))

#define cpphand(__ptr)  ((static_cast<jobject>( (void*) & (__ptr) )))


#define getMidpDynamicCharacterArrayPtr(__handle) \
(cppunhand(struct Java_com_sun_midp_lcdui_DynamicCharacterArray, __handle))

#define BUFFER_JARRAY_HANDLE(__handle) \
	cpphand(getMidpDynamicCharacterArrayPtr((__handle))->buffer)


KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_TextFieldLFImpl_setString0()
{
    HBufC* text;
	int err;
    int id = KNI_GetParameterAsInt(1);
    int parentId = KNI_GetParameterAsInt(3);
    int maxSize = KNI_GetParameterAsInt(4);

    KNI_StartHandles(2);

    KNI_DeclareHandle(bufferJDCArray);
    KNI_DeclareHandle(bufferJCharArray);
    
    KNI_GetParameterAsObject(2, bufferJDCArray);

    bufferJCharArray = BUFFER_JARRAY_HANDLE(bufferJDCArray);

    text = JBufferToHBufC(bufferJCharArray,
		      getMidpDynamicCharacterArrayPtr(bufferJDCArray)->length);

    KNI_EndHandles();
	//DEBUGMSG("setString0 "); DEBUGMSG(text); DEBUGMSG("\n");
	if (!text) 
	{
	    err = KNI_ENOMEM;
    } 
	else 
	{
		// application takes ownership
		err = static_cast<MApplication*>(Dll::Tls())->SetTextFieldString(id, text, parentId, maxSize);
    }

    if (err != KErrNone) 
	{
		KNI_ThrowNew(midpOutOfMemoryError, NULL);
    }
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_javax_microedition_lcdui_TextFieldLFImpl_getString0()
{
	HBufC* text;
	KNI_StartHandles(1);
	KNI_DeclareHandle(str);
    int id = KNI_GetParameterAsInt(1);

    text = static_cast<MApplication*>(Dll::Tls())->GetTextFieldString(id);
			
    if (!text) 
	{
        KNI_ThrowNew(midpOutOfMemoryError, NULL);
    } 
	else 
	{
		KNI_NewString(text->Des().Ptr(),text->Des().Length(),str);
    }

    KNI_EndHandlesAndReturnObject(str);
}

#define cppunhand(_type,_ptr)  (*static_cast<_type**>(((void*)_ptr )))

#define cpphand(__ptr)  ((static_cast<jobject>( (void*) & (__ptr) )))

#define getTextFieldLFImplPtr(handle) cppunhand(Java_javax_microedition_lcdui_TextFieldLFImpl,handle)


KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_TextFieldLFImpl_setEditPos0()
{
	int editPos = 0;
	int editLength = 0;
	KNI_StartHandles(1);
    KNI_DeclareHandle(thisObject);

	KNI_GetThisPointer(thisObject);
	
	static_cast<MApplication*>(Dll::Tls())->GetEditData(editPos,editLength);
	getTextFieldLFImplPtr(thisObject)->editPos = editPos;
	getTextFieldLFImplPtr(thisObject)->editLength = editLength;
	getTextFieldLFImplPtr(thisObject)->underline = static_cast<MApplication*>(Dll::Tls())->GetUnderline() ? KNI_TRUE : KNI_FALSE;
	KNI_EndHandles();
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_TextFieldLFImpl_setCursorPos0()
{
	int pos = KNI_GetParameterAsInt(2);
	static_cast<MApplication*>(Dll::Tls())->SetCursorPosition(pos);
	Java_javax_microedition_lcdui_TextFieldLFImpl_setEditPos0();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_DisplayableLFImpl_showDisplayableNotify0()
{
    int displayableId = KNI_GetParameterAsInt(1);

	static_cast<MApplication*>(Dll::Tls())->ShowDisplayableNotify(displayableId);

	KNI_ReturnVoid();
}
