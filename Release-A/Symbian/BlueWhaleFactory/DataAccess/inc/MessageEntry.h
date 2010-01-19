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



#ifndef __MESSAGE_FILE_NAME_H__
#define __MESSAGE_FILE_NAME_H__


#include <f32file.h>
#include "Message.h"


const TInt KMessageEntryFileNameLength = 24; // 16 characters for 64-bit creation time, 8 characters for 32-bit messageId.


/**
 * TMessageEntry is an abbreviated TEntry class for items with fixed, 
 * 24-character filenames that correspond to our message naming scheme.
 *
 * This abbreviated form of TEntry is needed so that we can keep
 * an in-memory a cache of current directory items available on disk, without
 * using as much RAM as would be needed to keep the full TEntry 
 * (which has size greater than 256bytes each by virtue of KMaxFileName iName).
 *
 * We store messages in files which have names that represent some 
 * information for that message.  Specifically, message storage files 
 * are named according to their TTime creation time on device and
 * their full UID.
 *
 *		16 characters for 64-bit creation time
 *      8 characters for 32-bit full uid
 *
 * Along with those 16 bytes, we also store 4 bytes of attributes as well as the
 * last modified time, so we can check for updates.
 */
class TMessageEntry
{
public:

	TMessageEntry();
	TMessageEntry( const TTime & aCreationTime, const MMessage::TMessageId & aMessageId, TUint32 aAttributes );
	TMessageEntry( const TDesC & aRelativeFileName, const TUint32 aAttributes);
//	TMessageEntry( const TEntry & aEntry );
	TMessageEntry( MMessage * message );

	void SetCreationTime(const TTime & aCreationTime);
	const TTime & GetCreationTime() const;

	void SetMessageId(const MMessage::TMessageId & aMessageId);
	const MMessage::TMessageId & GetMessageId() const;

	void SetAttributes(TUint32 aAttributes);
	TUint32 GetAttributes() const;

	const TTime & GetModified() const;
	
	void CreateRelativeFileNameForMessage( TDes & aName /* OUT */ , TInt aStartOffset = 0 /* IN */) const;

	TInt CompareFileName(const TMessageEntry & aOther) const;
	TBool Identical(const TMessageEntry & aOther) const;


	void DebugPrint() const
	{
		TBuf<KMessageEntryFileNameLength> name;
		CreateRelativeFileNameForMessage(name);
		RDebug::Print(name);
	}
private:

	static void ConvertToString(TUint32 aValue, TDes & aDestination, TInt aStartIndex = 0);
	static TUint32 ConvertToValue(const TDesC & aSource, TInt aStartIndex = 0);

	TTime iCreationTime;
	MMessage::TMessageId iMessageId;
	TUint32 iAttributes;
	TTime iModified;
};




#endif /* __MESSAGE_FILE_NAME_H__ */
