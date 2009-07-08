/*
 *
 *
 * Copyright  1990-2008 Sun Microsystems, Inc. All Rights Reserved.
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

extern "C" {
#include "imgj_imagedatafactory_kni.c"
#include "sni.h"
#include "midpUtilKni.h"
}

#include <e32std.h>
#include <OS_Symbian.hpp>

/**
 * Decodes the given byte array into the <tt>ImageData</tt>.
 * <p>
 * Java declaration:
 * <pre>
 *     loadNative(Ljavax/microedition/lcdui/ImageDataFactory;[BII)Z
 * </pre>
 *
 * @param imageData the ImageData to load to
 * @param imageBytes A byte array containing the encoded PNG image data
 * @param offset The start of the image data within the byte array
 * @param length The length of the image data in the byte array
 *
 * @return true if able to decode
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
KNIDECL(javax_microedition_lcdui_ImageDataFactory_loadNative) {
    int            length = KNI_GetParameterAsInt(4);
    int            offset = KNI_GetParameterAsInt(3);
    int            status = KNI_TRUE;
    unsigned char* srcBuffer = NULL;
    PIXEL* imgPixelData = NULL;
    ALPHA* imgAlphaData = NULL;
    java_imagedata* midpImageData = NULL;
	TBool generateMask = EFalse;

	KNI_StartHandles(4);
    KNI_DeclareHandle(pixelData);
    KNI_DeclareHandle(alphaData);
    KNI_DeclareHandle(inData);
    KNI_DeclareHandle(imageData);

    KNI_GetParameterAsObject(2, inData);
	KNI_GetParameterAsObject(1, imageData);
	midpImageData = IMGAPI_GET_IMAGEDATA_PTR(imageData);

    srcBuffer = (unsigned char *)JavaByteArray(inData);
	int byteArrayLength = KNI_GetArrayLength(inData);
	if (offset >= 0 && length >= 0 && offset + length <= byteArrayLength)
	{
		TPtrC8 sourceData(srcBuffer + offset, length);
		TInt width;
		TInt height;
		if (static_cast<MApplication*>(Dll::Tls())->InitializeDecoder(sourceData, width, height, generateMask) == KErrNone)
		{
			midpImageData->width = width;
			midpImageData->height = height;
		
			SNI_NewArray(SNI_BYTE_ARRAY, midpImageData->width * midpImageData->height * 2, pixelData);
			if (generateMask)
			{
				SNI_NewArray(SNI_BYTE_ARRAY, midpImageData->width * midpImageData->height, alphaData);
			}

			if (!KNI_IsNullHandle(pixelData) && (!KNI_IsNullHandle(alphaData) || !generateMask))
			{		
				if (generateMask)
				{
					imgAlphaData = (ALPHA*)JavaByteArray(alphaData);
				}

				midp_set_jobject_field(KNIPASSARGS imageData, "pixelData", "[B", pixelData);
				imgPixelData = (PIXEL*)JavaByteArray(pixelData);

				if (static_cast<MApplication*>(Dll::Tls())->DecodeImage((char*)imgPixelData, (char*)imgAlphaData) != KErrNone)
				{
					status = KNI_FALSE;
				}
				if (imgAlphaData)
				{
					midp_set_jobject_field(KNIPASSARGS imageData, "alphaData", "[B", alphaData);
				}
			}
            else
            {
                status = KNI_FALSE;
			}
        }
	}

	KNI_EndHandles();
    KNI_ReturnBoolean(status);
}
