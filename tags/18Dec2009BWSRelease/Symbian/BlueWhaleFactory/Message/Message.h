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



#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "MessageAttributes.h"


const TInt KIID_MMessage = 0x01000005;


/**
 * Our interface to represent a message.
 */
class MMessage : public MUnknown
{
public:

	/**
	 * A 32 bit quantity which must uniquely identify a message.
	 *
	 * This quantity is used to check to make sure we have not
	 * already downloaded a message, or to locate previously
	 * downloaded messages when, for example, we receive a
	 * requested attachment for a message.
	 *
	 * We will attempt to use a 32-bit hash of the message-id email 
	 * header field.  We choose this way of doing this, because
	 * when using Outlook to forward or reply to a message, Exchange 
	 * re-issues existing messages of the same message-id
	 * with a new IMAP UID.  For this reason, assuming that:
	 *
	 *		(different IMAP UID => new message)
	 *
	 * is false on Exchange. The message-id, however, is not changed.
	 * 
	 *
	 * If message-id is not present, we will fall back
	 * to using 32-bit IMAP UID.
	 * In that case, however, if you forward or reply to a message
	 * using Outlook, you will receive a new copy of the original message
	 * (with a new UID).
	 */
	typedef TUint32 TMessageId;

	/**
	 * Note that setting MessageId for a message should not alter it's
	 * dirty() status, but the message is responsible for providing
	 * its original MessageId on request.
	 */
	virtual void SetMessageId(const TMessageId & aMessageId ) = 0;
	virtual TMessageId MessageId() const = 0;

	/**
	 * Used to determine the currently persisted TMessageId for a message.
	 */
	virtual TMessageId OriginalMessageId() const = 0;



	/**
	 * Every message lives in a folder within an account.
	 *
	 * Note that setting the account for a message should not
	 * alter its Dirty() status.  Instead, the message is
	 * required to produce it's original account on demand.
	 */
	virtual void SetAccountL( const TDesC & aAccount ) = 0;
	virtual const TDesC & AccountL() const = 0;

	/**
	 * Used to determine the account in which a message is currently persisted.
	 */
	virtual const TDesC & OriginalAccountL() const = 0;


	/**
	 * Note that setting the folder for a message should not
	 * alter its Dirty() status.  Instead, the message is
	 * required to produce it's original folder on demand.
	 */
	virtual void SetFolderL( const TDesC & aFolder ) = 0;
	virtual const TDesC & FolderL() const = 0;

	/**
	 * Used to determine the folder in which a message is currently persisted.
	 */
	virtual const TDesC & OriginalFolderL() const = 0;
	
	/**
	 * This 32-bit quantity is used in some interesting ways and should
	 * only be manipulated through the TMessageAttributes class.
	 *
	 * Messages are for the most part unchanging objects -- we rarely
	 * modify a downloaded message (perhaps only if we've downloaded more
	 * of the message) nor do we usually change a sent message once composed.
	 *
	 * But there are a few message attributes (such as read versus
	 * unread state, message sending state, etc.) which do change often
	 * but which fundamentally reflect no change to the message itself.
	 *
	 * We have chosen to set those values aside here, so that
	 * we can optimize changes to message attributes.
	 *
	 *
	 * Note that changing a messages attributes should not
	 * change its Dirty() status.  Instead, a message is 
	 * required to produce its original attributes on demand.
	 */
	virtual void SetAttributes( TUint32 aAttributes ) = 0;
	virtual TUint32 Attributes() const = 0;
	virtual TUint32 OriginalAttributes() const = 0;


	/**
	 * The creation time of the message on the device.
	 *
	 * Automatically set when the message is created.
	 *
	 * Note distinction from time on server or time initially sent.
	 * Those are stored elsewhere.
	 */
	virtual void SetTime(const TTime & aTime) = 0;
	virtual TTime Time() const = 0;
	virtual TTime OriginalTime() const = 0;


protected:
	virtual ~MMessage() {}
};


/**
 * Sometimes it is handy when creating messages to persist in the folder info 
 * the last messageId ever created.  This is the key into folder properties for that information.
 */
const TIntUniqueKey KPropertyIntLastMessageIdCreated = {KIID_MMessage, 0x133};


#endif /* __MESSAGE_H__ */
