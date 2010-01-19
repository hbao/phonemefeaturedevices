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



#ifndef __EMAIL_MESSAGE_H__
#define __EMAIL_MESSAGE_H__

#include "Message.h"


// EComPlus component id for a default implementation of this interface.
const TInt KCID_MEmailMessage = 0x10206D88;


/**
 * In general, we try to store message body data exactly as it was
 * retrieved over the air.  It is our philosophy that keeping
 * the data in as closely as possible exactly the same format
 * as how it was received will allows us the flexibility later
 * to easily add new processors for data.
 * 
 * Since IMAP is a 7-bit encoding protocol, this means
 * we will be using 8-bit chars as the default storage
 * mechanism for inbound messages' bodies.
 *
 * This key indicates where in a message (or mime subpart)
 * the inbound text can be found.
 */
const TString8UniqueKey KPropertyString8MessageBody		= { KIID_MMessage, 10002 };

/**
 * However, for outbound messages, since we control the choice
 * of format, we have made the executive decision that since
 * Symbian is Unicode, and since we find it easiest to use
 * base64 encoding anyway, and since most mobile-originated
 * messages will be short, it is expedient (and future-proof)
 * to use 16-bit Unicode to store outbound text.
 * This eliminates the need to fiddle with conversions.
 */
#ifdef __USE_BWM_VM__
const TStringUniqueKey KPropertyStringMessageBody		= { KIID_MMessage, 10002 };
#else
const TStringUniqueKey KPropertyStringMessageBody		= { KIID_MMessage, 10003 };
#endif


const TStringUniqueKey KPropertyStringDecodedMessageBody		= { KIID_MMessage, 0x50000001 };


/**
 * Inbound messages have MIME header field names which are 8-bit US-ASCII encoded.
 *
 * When downloading and processing IMAP for a message, we use the MStringKeyProperties interface
 * to populate a mime subpart with the header data.  To retrieve header values, use the
 * following strings.
 *
 */
_LIT8( KMessage8TO,			"TO");
_LIT8( KMessage8CC,			"CC");
_LIT8( KMessage8BCC,			"BCC");
_LIT8( KMessage8SUBJECT,		"SUBJECT");
_LIT8( KMessage8REPLYTO,		"REPLY-TO");
_LIT8( KMessage8FROM,		"FROM");
_LIT8( KMessage8DATE,		"DATE");


/**
 * Outbound messages are stored natively as UNICODE UCS-2, and only converted from UNICODE UCS-2
 * at the moment of transport.
 */
_LIT( KMessageTO,			"to");
_LIT( KMessageCC,			"cc");
_LIT( KMessageBCC,			"bcc");
_LIT( KMessageSUBJECT,		"subject");
_LIT( KMessageREPLYTO,		"reply-to");
_LIT( KMessageFROM,		"from");
_LIT( KMessageDATE,		"date");




_LIT( KMessageRe,			"Re: ");
_LIT( KMessageFw,			"Fw: ");

_LIT( KOriginalMessageDivider, "\r\n\r\n-----Original Message-----\r\n");


const TObjectUniqueKey KPropertyObjectMessageSubparts	= { KIID_MMessage, 10100 };
const TString8UniqueKey KPropertyString8MessageType		= { KIID_MMessage, 10101 };
_LIT(MIME_CONTENT_TYPE,"content-type");
const TString8UniqueKey KPropertyString8MessageSubtype	= { KIID_MMessage, 10102 };

const TString8UniqueKey KPropertyString8MessageEncoding	= { KIID_MMessage, 10103 };
const TIntUniqueKey		KPropertyIntMessageSubpartSize 	= { KIID_MMessage, 10104 };	


/**
 * Some attachment subparts contain no content, rather only a reference to an external file.
 */
const TStringUniqueKey	KPropertyStringAttachmentFileName = { KIID_MMessage, 10105 };


/**
 * The .Low() 32-bits corresponds to the RFC 3501 IMAP uid, the .High() 
 * 32-bits is the IMAP UIDVALIDITY.
 */
const TInt64UniqueKey	KPropertyInt64MessageUid = { KIID_MMessage, 10106 }; 

#endif /* __EMAIL_MESSAGE_H__ */
