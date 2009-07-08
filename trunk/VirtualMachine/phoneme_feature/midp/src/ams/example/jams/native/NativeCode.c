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
#include <pcsl_memory.h>

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_main_BWMDisplayController_requestBackground0()
{
    // Stub code
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_main_BWMDisplayController_requestForeground0()
{
    // stub code
}

KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_bluewhalesystems_midp_SMSTextReader_getInstallSMSBody()
{
    // stub code
    KNI_StartHandles(1);
    KNI_DeclareHandle(string);
    KNI_NewStringUTF("", string);
    KNI_EndHandlesAndReturnObject(string);
}

// See midp/src/configuration/properties/reference/native/properties_kni.c
static char* UnicodeToCString(jchar* uString, int length) {
    int        i;
    char*      cString;
    
    if (NULL == uString) {
        return NULL;
    }
    
    cString = (char *)pcsl_mem_malloc(length + 1);
    if (NULL == cString) {
        return NULL;
    }

    // WideCharToMultiByte(CP_ACP, 0, uString, -1, cString, length, NULL, NULL);
    for (i = 0; i < length; i++) {
        cString[i] = (char)uString[i];
    }

    cString[length] = '\0';

    return cString;
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_bluewhalesystems_midp_PlatformRequestListener_setSystemProperty0()
{
    int keyLength;
    int valLength;
    jchar* keyChars;
    jchar* valChars;
    char* key8;
    char* val8;

    KNI_StartHandles(2);
    KNI_DeclareHandle(key);
    KNI_DeclareHandle(val);
    KNI_GetParameterAsObject(1, key);
    KNI_GetParameterAsObject(2, val);
    keyLength = KNI_GetStringLength(key);
    valLength = KNI_GetStringLength(val);

    keyChars = (jchar*)pcsl_mem_malloc(keyLength * sizeof (jchar));
    if (keyChars)
    {
        KNI_GetStringRegion(key, 0, keyLength, keyChars);
        valChars = (jchar*)pcsl_mem_malloc(valLength * sizeof (jchar));
        if (valChars)
        {
            KNI_GetStringRegion(val, 0, valLength, valChars);
            key8 = UnicodeToCString(keyChars,keyLength);
            if (key8)
            {
                val8 = UnicodeToCString(valChars,valLength);
                if (val8)
                {
                    JVMSPI_SetSystemProperty(key8,val8);
                    pcsl_mem_free(val8);
                }
                pcsl_mem_free(key8);
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
    int keyLength;
    jchar* keyChars;
    char* key8;

    KNI_StartHandles(1);
    KNI_DeclareHandle(key);
    KNI_GetParameterAsObject(1, key);
    keyLength = KNI_GetStringLength(key);

    keyChars = (jchar*)pcsl_mem_malloc(keyLength * sizeof (jchar));
    if (keyChars)
    {
        KNI_GetStringRegion(key, 0, keyLength, keyChars);
        key8 = UnicodeToCString(keyChars,keyLength);
        if (key8)
        {
            JVMSPI_FreeSystemProperty(key8);
            pcsl_mem_free(key8);
        }
        pcsl_mem_free(keyChars);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}
