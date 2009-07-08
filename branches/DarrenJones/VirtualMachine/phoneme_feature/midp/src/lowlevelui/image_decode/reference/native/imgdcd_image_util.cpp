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
#include "imgdcd_image_util.c"
}

#include <e32std.h>
#include <f32file.h>
#include <OS_Symbian.hpp>

/**
 * Decodes the given input data into a storage format used by immutable
 * images.  The input data should be a PNG image.
 *
 *  @param srcBuffer input data to be decoded.
 *  @param length length of the input data.
 *  @param creationErrorPtr pointer to the status of the decoding
 *         process. This function sets creationErrorPtr's value.
 */
int
imgdcd_decode_png
(unsigned char* srcBuffer, int length, 
 int width, int height,
 imgdcd_pixel_type *pixelData, 
 imgdcd_alpha_type *alphaData,
 img_native_error_codes* creationErrorPtr)
{
	char* imgAlphaData = NULL;
	TPtrC8 sourceData(srcBuffer, length);
	TInt decoderWidth;
	TInt decoderHeight;
	TBool generateMask;
	if (static_cast<MApplication*>(Dll::Tls())->InitializeDecoder(sourceData, decoderWidth, decoderHeight, generateMask) == KErrNone)
	{
		ASSERT(decoderWidth == width);
		ASSERT(decoderHeight == height);
		if (generateMask)
		{
			imgAlphaData = (char*)alphaData;
		}
		if (static_cast<MApplication*>(Dll::Tls())->DecodeImage((char*)pixelData, (char*)imgAlphaData) == KErrNone)
		{
			*creationErrorPtr = IMG_NATIVE_IMAGE_NO_ERROR;
		}
		else
		{
			*creationErrorPtr = IMG_NATIVE_IMAGE_DECODING_ERROR;
		}
	}
	else
	{
		*creationErrorPtr = IMG_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
	}
	
	return (imgAlphaData != NULL);
}

