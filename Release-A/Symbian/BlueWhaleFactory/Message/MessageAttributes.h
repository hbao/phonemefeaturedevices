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



#ifndef __MESSAGE_ATTRIBUTES_H__
#define __MESSAGE_ATTRIBUTES_H__

#include "Properties.h"


/**
 * This class can be used to manipulate the value found in the 
 * 32-bit KPropertyIntMessageAttributes quantity set on a message.
 *
 * The reason we chose to require MMessage implementing objects to 
 * allow a 32 bit attributes quantity to be Get and Set, and the
 * reason for externally manipulating that quantity here instead of
 * in the MMessage implementing class itself, is that this 32 bit
 * quantity "belongs" to the message storage and transmission 
 * mechanism and not to the message.
 */
class TMessageAttributes
{
public:
	inline static TBool GetViewed( TInt32 aAttributes )
	{
		return (aAttributes & EMessageAttributeMaskViewed) >> FirstBitPosition(EMessageAttributeMaskViewed);
	}
	inline static void SetViewed( TInt32 & aAttributes, TBool aViewed = ETrue )
	{
		aAttributes &= ~ EMessageAttributeMaskViewed;
		aAttributes |= (aViewed << FirstBitPosition(EMessageAttributeMaskViewed)) & EMessageAttributeMaskViewed;
	}

	inline static TBool GetInbound( TInt32 aAttributes )
	{
		return (aAttributes & EMessageAttributeMaskInbound) >> FirstBitPosition(EMessageAttributeMaskInbound);
	}
	inline static void SetInbound( TInt32 & aAttributes, TBool aInbound = ETrue )
	{
		aAttributes &= ~ EMessageAttributeMaskInbound;
		aAttributes |= (aInbound << FirstBitPosition(EMessageAttributeMaskInbound)) & EMessageAttributeMaskInbound;
	}

	typedef enum 
	{
			EMessageDraft
		,	EMessageQueued
		,	EMessageSending		// In process of sending (e.g. via SMTP).
		,	EMessageErrorSending
		,	EMessageSent		// Sent (e.g. via SMTP).
		,	EMessageRecording	// Sent message is now in process of recording message (e.g. uploading it to our Sent Items folder).
		,	EMessageErrorRecording	// Sent message is now in process of recording message (e.g. uploading it to our Sent Items folder).
		,	EMessageRecorded	// Sent message has now completed recording message (e.g. now uploaded to our Sent Items folder).
		,   EMessagePreparingToSend
	} TMessageSendingState;



	inline static TMessageSendingState GetSendingState( TInt32 aAttributes )
	{
        TUint firstPart = ( aAttributes & EMessageAttributeMaskSendingState ) >> FirstBitPosition( EMessageAttributeMaskSendingState, 0 );
        TUint secondPart = ( aAttributes & EMessageAttributeMaskSendingState2 ) >> FirstBitPosition( EMessageAttributeMaskSendingState2, 0 );

        return static_cast<TMessageAttributes::TMessageSendingState>( ( secondPart << BitCount( EMessageAttributeMaskSendingState ) ) + firstPart );
	}
	inline static void SetSendingState( TInt32 & aAttributes, TMessageSendingState aSendingState)
	{
		TUint attributebits1 = aAttributes & ~EMessageAttributeMaskSendingState;
		TUint sendingStateBits1 = aSendingState << FirstBitPosition( EMessageAttributeMaskSendingState, 0 );
        attributebits1 |= sendingStateBits1 & EMessageAttributeMaskSendingState;

        TUint attributebits2 = attributebits1 & ~EMessageAttributeMaskSendingState2;
        TUint sendingStateBits2 = ( aSendingState >> BitCount( EMessageAttributeMaskSendingState ) ) << FirstBitPosition( EMessageAttributeMaskSendingState2, 0 );
        attributebits2 |= sendingStateBits2 & EMessageAttributeMaskSendingState2;
        aAttributes = (aAttributes & ~(EMessageAttributeMaskSendingState | EMessageAttributeMaskSendingState2)) |  attributebits2;
	}


private:
	enum TMessageAttributesMasks
	{
		  EMessageAttributeMaskViewed = 0x00000001
		, EMessageAttributeMaskSendingState = 0x0000000E
		, EMessageAttributeMaskInbound = 0x00000010
		, EMessageAttributeMaskSendingState2 = 0x000000C0 // Another 2 bits of sending state, after discovering that 3 bits (8 states) was not enough.

	} ;

	/**
	 * Returns first non-zero bit position for a given mask,
	 * so you know how far you have to shift a value.
	 */
	static TInt FirstBitPosition( TMessageAttributesMasks aMask, TInt aFirstBitPosition = 0 )
	{
		TInt bitPosition = 32;

		TInt32 bit = 0x1 << aFirstBitPosition;
		
		for( TInt i = aFirstBitPosition; i < 32; ++i )
		{
			if( aMask & bit )
			{
				return i;
				break;
			}
			bit <<= 1;
		}

		return bitPosition;
	}

	static TInt BitCount( TUint aMask )
	{
		TUint bitCount = 0;
	
		TUint bit = 0x1;
	
	    for( TUint i = bit; i < 32; ++i )
	    {
	        if( ( aMask & bit ) > 0 )
	        {
	            ++bitCount;
	        }
	        bit <<= 1;
	    }
	
	    return bitCount;
	}

};


#endif /* __MESSAGE_ATTRIBUTES_H__ */
