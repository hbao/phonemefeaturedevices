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


#include "DebugUtilities.h"

const TInt KDebugPrintChunk = 80;

EXPORT_C void DebugPrint( TInt aNestingLevel, const TDesC8 & aString )
{
#ifdef __WINS__
	TInt size = aString.Length();
	TInt position = 0; 
	TInt segment = KDebugPrintChunk;
	if( aNestingLevel > KRDebugPrintGetsPissedOffSize/3)
	{
		aNestingLevel = KRDebugPrintGetsPissedOffSize - KRDebugPrintGetsPissedOffSize/3;
	}
	if( segment + aNestingLevel > KRDebugPrintGetsPissedOffSize )
	{
		segment = KRDebugPrintGetsPissedOffSize - aNestingLevel;
	}
	while( position < size)
	{
		if( position + KDebugPrintChunk > size )
		{
			segment = size - position;
		}
		TBuf<KDebugPrintChunk> wideBuffer;
		TBuf<KRDebugPrintGetsPissedOffSize> nestedWideBuffer;
		TPtrC8 ptr = aString.Mid(position, segment);
		wideBuffer.Copy(ptr);
		if( aNestingLevel )
		{
			for( TInt i = 0; i < aNestingLevel; ++i )
			{
				nestedWideBuffer.Append(_L(" "));
			}
			nestedWideBuffer.Append(_L("|"));
		}
		nestedWideBuffer.Append(wideBuffer);
		RDebug::Print(_L("%S"), &nestedWideBuffer);
		position += segment;
	}
#endif /* __WINS__ */
}

EXPORT_C void DebugPrint( TInt aNestingLevel, const TDesC & aString )
{
#ifdef __WINS__
	TInt size = aString.Length();
	TInt position = 0; 
	TInt segment = KDebugPrintChunk;
	if( aNestingLevel > KRDebugPrintGetsPissedOffSize/3)
	{
		aNestingLevel = KRDebugPrintGetsPissedOffSize - KRDebugPrintGetsPissedOffSize/3;
	}
	if( segment + aNestingLevel > KRDebugPrintGetsPissedOffSize )
	{
		segment = KRDebugPrintGetsPissedOffSize - aNestingLevel;
	}
	while( position < size)
	{
		if( position + KDebugPrintChunk > size )
		{
			segment = size - position;
		}
		TBuf<KDebugPrintChunk> wideBuffer;
		TBuf<KRDebugPrintGetsPissedOffSize> nestedWideBuffer;
		TPtrC ptr = aString.Mid(position, segment);
		wideBuffer.Copy(ptr);
		if( aNestingLevel )
		{
			for( TInt i = 0; i < aNestingLevel; ++i )
			{
				nestedWideBuffer.Append(_L(" "));
			}
			nestedWideBuffer.Append(_L("|"));
		}
		nestedWideBuffer.Append(wideBuffer);
		RDebug::Print(_L("%S"), &nestedWideBuffer);
		position += segment;
	}
#endif /* __WINS__ */
}

