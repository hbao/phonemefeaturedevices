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


#include "MessageEntry.h"



TMessageEntry::TMessageEntry()
	: iCreationTime( 0 )
	, iMessageId( 0 )
	, iAttributes( 0 )
	, iModified(0)
{
}

TMessageEntry::TMessageEntry(const TTime & aCreationTime, const MMessage::TMessageId & aMessageId, TUint32 aAttributes )
	: iCreationTime( aCreationTime)
	, iMessageId( aMessageId )
	, iAttributes( aAttributes )
	, iModified(0)
{
}


TMessageEntry::TMessageEntry(const TDesC & aFileName, TUint32 aAttributes )
	: iCreationTime( 0)
	, iMessageId( 0 )
	, iAttributes(aAttributes)
	, iModified(0)
{
	if( aFileName.Length() == KMessageEntryFileNameLength )
	{
		TUint timeHigh = 0;
		TLex lex1(aFileName.Left(8));
		lex1.Val(timeHigh, EHex);

		TUint timeLow = 0;
		TLex lex2(aFileName.Mid(8,8));
		lex2.Val(timeLow, EHex);

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		TInt64 tempTime = MAKE_TINT64(timeHigh,timeLow);
#else // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt64 tempTime(timeHigh, timeLow);
#endif // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TTime NineteenSeventy(_L("19700101:000000.000000"));
		NineteenSeventy += TTimeIntervalMicroSeconds(tempTime * 1000);
		iCreationTime = NineteenSeventy;

		iMessageId = 0;
		TLex lex3(aFileName.Mid(16,8));
		lex3.Val(iMessageId, EHex);

		iAttributes = aAttributes;
	}
}

#if 0
TMessageEntry::TMessageEntry( const TEntry & aEntry )
	: iCreationTime( 0 )
	, iMessageId( 0 )
	, iAttributes(0)
	, iModified(0)
{
	const TDesC & fileName = aEntry.iName;

	if( fileName.Length() == KMessageEntryFileNameLength )
	{
		TUint timeHigh = 0;
		TLex lex1(fileName.Left(8));
		lex1.Val(timeHigh, EHex);

		TUint timeLow = 0;
		TLex lex2(fileName.Mid(8,8));
		lex2.Val(timeLow, EHex);

#if (__S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
		TInt64 tempTime = MAKE_TINT64(timeHigh,timeLow);
#else // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
		TInt64 tempTime(timeHigh, timeLow);
#endif // __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__


		iCreationTime = tempTime;

		iMessageId = 0;
		TLex lex3(fileName.Mid(16,8));
		lex3.Val(iMessageId, EHex);

		iAttributes = aEntry[2].iUid;

		iModified = aEntry.iModified;
	}
}
#endif
TMessageEntry::TMessageEntry( MMessage * message )
{
	iCreationTime = message->Time();
	iMessageId = message->MessageId();
	iAttributes = message->Attributes();

	MExternalizable * externalizable = 0;
	TRAPD( queryError, externalizable = QiL(message, MExternalizable) );
	if( KErrNone == queryError )
	{
		CleanupReleasePushL(*externalizable);
		iModified = externalizable->GetModified();
		CleanupStack::PopAndDestroy(externalizable);
	}
}


void TMessageEntry::SetCreationTime(const TTime & aCreationTime)
{
	iCreationTime = aCreationTime;
}

const TTime & TMessageEntry::GetCreationTime() const
{
	return iCreationTime;
}

void TMessageEntry::SetMessageId(const MMessage::TMessageId & aMessageId)
{
	iMessageId = aMessageId;
}

const MMessage::TMessageId & TMessageEntry::GetMessageId() const
{
	return iMessageId;
}

void TMessageEntry::SetAttributes(TUint32 aAttributes )
{
	iAttributes = aAttributes;
}

TUint32 TMessageEntry::GetAttributes() const
{
	return iAttributes;
}

const TTime & TMessageEntry::GetModified() const
{
	return iModified;
}


_LIT( KMessageFileNameFormat, "%08x%08x%08x" );

void TMessageEntry::CreateRelativeFileNameForMessage( TDes & aName, TInt aStartOffset ) const
{
	aName.SetLength( aStartOffset + KMessageEntryFileNameLength);

	TTime NineteenSeventy(_L("19700101:000000.000000"));
	TTimeIntervalMicroSeconds interval = iCreationTime.MicroSecondsFrom(NineteenSeventy);
	
	TInt64 tempTime = interval.Int64() / 1000;
	
	aName.Format(KMessageFileNameFormat, MY64HIGH(tempTime), MY64LOW(tempTime), iMessageId );
}

TInt TMessageEntry::CompareFileName(const TMessageEntry & aOther) const
{
	if( iCreationTime > aOther.iCreationTime )
	{
		return 1;
	}
	else if( iCreationTime < aOther.iCreationTime )
	{
		return -1;
	}
	else
	{
		if( iMessageId > aOther.iMessageId )
		{
			return 1;
		}
		else if( iMessageId < aOther.iMessageId )
		{
			return -1;
		}
	}

	return 0;
}

TBool TMessageEntry::Identical(const TMessageEntry & aOther) const
{
	if( 0 == CompareFileName(aOther) )
	{
		if( iAttributes == aOther.iAttributes )
		{
			if( iModified == aOther.iModified )
			{
				return ETrue;
			}
		}
	}
	return EFalse;
}
