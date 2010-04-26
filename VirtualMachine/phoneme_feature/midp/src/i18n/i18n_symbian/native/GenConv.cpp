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

#include <stdio.h>
#include <e32std.h>
#include <string.h>
#include <conv.h>
#include <OS_Symbian.hpp>

#define KMaxBufferLen 1024;

//#define __DEBUG_CONV__

#ifdef __DEBUG_CONV__
#include "pcsl_print.h"
#define DEBUGMSG1(_XX,_YY){pcsl_print(_XX);pcsl_print(_YY);pcsl_print("\n");}
#else
#define DEBUGMSG1
#endif

LOCAL_C HBufC8* NativeToDescriptor8(char* aSource)
{
    int len = strlen(aSource);
    HBufC8* buffer8 = HBufC8::New(len);
	if(buffer8)
	{
        buffer8->Des().Copy((unsigned char*)aSource,len);
    }
    return buffer8;
}
LOCAL_C HBufC8* NativeToDescriptor8(char* aSource,int aOffset,int aLen)
{
    HBufC8* buffer8 = HBufC8::New(aLen);
	if(buffer8)
	{
        buffer8->Des().Copy((unsigned char*)aSource + aOffset,aLen);
    }
    return buffer8;
}

LOCAL_C HBufC16* UnicodeToDescriptor16(const TUint16* aSource,int aOffset,int aLen)
{
    HBufC16* ret = NULL;
    return ret;
}

LOCAL_C int byteMaxLen()
{
    return KMaxBufferLen;
}

LOCAL_C int byteLen(const unsigned char* aBuffer, int aLen)
{
    int ret = 0;
    HBufC8* buffer8 = NativeToDescriptor8((char*)aBuffer,0,aLen);
    if(buffer8)
    {
        ret = static_cast<MApplication*>(Dll::Tls())->GetSizeOfConvertedNative(*buffer8);
        delete buffer8;
    }
    return ret;

}

LOCAL_C int unicodeToNative(const jchar* aInBuffer, int aInLen, unsigned char* aOutBuffer, int aOutLen)
{
    int ret = 0;
    HBufC16* buffer16 = UnicodeToDescriptor16(aInBuffer,0,aInLen);
    if(buffer16)
    {
        TPtr8 outBuffer(aOutBuffer,aOutLen);
        ret = static_cast<MApplication*>(Dll::Tls())->ConvertUnicodeToNative(buffer16->Des(),outBuffer);
        delete buffer16;
    }
    return ret;
}

LOCAL_C int nativeToUnicode(const unsigned char* aInBuffer, int aInLen, jchar* aOutBuffer, int aOutLen)
{
    int ret = 0;
    HBufC8* buffer8 = NativeToDescriptor8((char*)aInBuffer,0,aInLen);
    if(buffer8)
    {
        TPtr16 outBuffer(aOutBuffer,aOutLen);
        ret = static_cast<MApplication*>(Dll::Tls())->ConvertNativeToUnicode(static_cast<TDesC8>(*buffer8),outBuffer);
        delete buffer8;
    }
    return ret;
}

LOCAL_C int sizeOfByteInUnicode(const unsigned char* aBuffer, int aOffset, int aLen)
{
    int ret = 0;
    HBufC8* buffer8 = NativeToDescriptor8((char*)aBuffer,aOffset,aLen);
    if(buffer8)
    {
        ret = static_cast<MApplication*>(Dll::Tls())->GetSizeOfConvertedNative(*buffer8);
        delete buffer8;
    }
    return ret;
}

LOCAL_C int sizeOfUnicodeInByte(const jchar* aBuffer, int aOffset, int aLen)
{
    int ret = 0;
    HBufC16* buffer16 = UnicodeToDescriptor16(aBuffer,0,aLen);
    if(buffer16)
    {
        ret = static_cast<MApplication*>(Dll::Tls())->GetSizeOfConvertedUnicode(*buffer16);
        delete buffer16;
    }
    return ret;
}

LcConvMethodsRec conv_methods = 
    {
        "gb2312 gbk GB2312 GBK ISO_8859_1 KO18_r US_ASCII WINDOWS_1252",
        &byteMaxLen,
        &byteLen,
        &unicodeToNative,
        &nativeToUnicode,
        &sizeOfByteInUnicode,
        &sizeOfUnicodeInByte
    };
    
LcConvMethods getLcGenConvMethods(char *aEncoding) 
{
    LcConvMethods ret = NULL;
    DEBUGMSG1("getLcGenConvMethods ",aEncoding);
    HBufC8* buffer8 = NativeToDescriptor8(aEncoding);
    if(buffer8)
    {
        if(static_cast<MApplication*>(Dll::Tls())->SetConverter(*buffer8))
        {
            ret = &conv_methods;
        }
    delete buffer8;
    }
    
    return ret;
}
