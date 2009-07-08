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
#include <commonKNIMacros.h>
#include <OS_Symbian.hpp>
#include <pcsl_string.h>

extern HBufC* GetDescriptor(jstring aHandle);

KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_media_MyPlayer_playerStart()
{
	unsigned char* data = NULL;
	TInt err = KErrNotSupported;
	KNI_StartHandles(2);

    KNI_DeclareHandle(typeH);
	KNI_DeclareHandle(dataH);

	KNI_GetParameterAsObject(1, typeH);
	KNI_GetParameterAsObject(2, dataH);
		
	HBufC* type = GetDescriptor(typeH);
	if (type)
	{
		HBufC8* type8 = HBufC8::New(type->Length());
		if (type8)
		{
			type8->Des().Copy(*type);
			data = (unsigned char*)JavaByteArray(dataH);
			int length = KNI_GetArrayLength(dataH);
			TPtr8 dataPtr(data, length, length);
			err = static_cast<MApplication*>(Dll::Tls())->PlayAudio(*type8, dataPtr);
			delete type8;
		}
		delete type;
	}

	if (err)
	{
		KNI_ThrowNew("javax.microedition.media.MediaException", NULL);
	}

	KNI_EndHandles();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_media_MyPlayer_playerStop()
{
}
