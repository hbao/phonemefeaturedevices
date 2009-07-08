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



#ifndef __MESSAGING_DATA_ACCESS_CONNECTION_H__
#define __MESSAGING_DATA_ACCESS_CONNECTION_H__

#include "DataAccess.h"
#include "Message.h"



/**
 * Pass this to ECOMPLUS to create a connection to the "messaging" data store.
 */
const TInt KCID_MMessagingDataAccessConnection = 0x1020805F;


/**
 * The key usually used to store a pointer to a messaging MDataAccessConnection interface in an MProperties properties bag.
 */
const TObjectUniqueKey KPropertyObjectMessagingDataAccessConnection = {KCID_MMessagingDataAccessConnection, 0x75235};


/**
 * The key usually used to store a pointer to a messaging MDataAccessStatement interface in an MProperties properties bag.
 */
const TObjectUniqueKey KPropertyObjectMessagingDataAccessStatement = {KCID_MMessagingDataAccessConnection, 0x75245};


/**
 * The key usually used to store a pointer to a messaging MDataAccessResultArray interface in an MProperties properties bag.
 */
const TObjectUniqueKey KPropertyObjectMessagingDataAccessOutboxResultArray = {KCID_MMessagingDataAccessConnection, 0x75255};

const TStringUniqueKey PROPERTY_STRING_INBOUND_PROTOCOL     = {KCID_MMessagingDataAccessConnection,  0x7353ef31 };

/**
 * Additional statement types supported by this KCID_MMessagingDataAccessConnection.
 *
 * On ExecuteL's return, aReturn should be a pointer to an MProperties object which
 * contains information about a folder, account, or the entire messaging system.
 */
const MDataAccessStatement::TStatementType KStatementTypeRootFolderInfo = {KCID_MMessagingDataAccessConnection, 0xaa999};

const TInt64UniqueKey PROPERTY_LONG_UPLOADED_BYTES = {KCID_MMessagingDataAccessConnection, 0xe132344 };
/** 
 * This property on the global messaging info retrieved using (STATEMENT_TYPE_MESSAGING_INFO) is
 * used to store the total number of bytes downloaded (for emails and ads)
*/
const TInt64UniqueKey PROPERTY_LONG_DOWNLOADED_BYTES = {KCID_MMessagingDataAccessConnection, 0xe132345 };

/** 
 * This property on the global messaging info retrieved using (STATEMENT_TYPE_MESSAGING_INFO) is
 * used to store the last time at which the PROPERTY_LONG_UPLOADED_BYTES and PROPERTY_LONG_DOWNLOADED_BYTES
 * were reset.
 */
const TInt64UniqueKey PROPERTY_LONG_BYTE_COUNTERS_RESET = {KCID_MMessagingDataAccessConnection, 0xe13f45 };


/**
 * Additional statement type supported by this KCID_MMessagingDataAccessConnection.
 *
 * This statement will return the full file name path where the specified attachment for
 * a given message (set as the KPropertyObjectItem) should be stored (and from which it 
 * can be retrieved if it already exists).
 *
 */
const MDataAccessStatement::TStatementType KStatementTypeRetrieveExternalAttachmentFullFileName = {KCID_MMessagingDataAccessConnection, 0xaa999};
const TStringUniqueKey KPropertyStringAttachmentFullFileName = {KCID_MMessagingDataAccessConnection, 0x990};
const TObjectUniqueKey KPropertyObjectSubItem = {KCID_MMessagingDataAccessConnection, 0x9901040};



/**
 * Below are some keys which can be set on the MDataAccessStatement for the 
 * KMessagingDataAccessConnection and how they are used.
 */

/**
 * The account name -- this store supports multiple accounts.
 *
 * Expected to be set in the following statements:
 *
 *		KStatementTypeSelect		// required
 *		KStatementTypeDelete		// optional -- needed for ranges but not needed when you specify a KPropertyObjectItem as item stores its own account information.
 *		KStatementTypeInsert		// required
 *		KStatementTypeFolderInfo	// required
 */
const TStringUniqueKey KPropertyStringAccount = {KCID_MMessagingDataAccessConnection, 0x1000};


/**
 * The folder name -- there may be multiple folders within each account.
 *
 * Expected to be set in the following statements:
 *
 *		KStatementTypeSelect		// required
 *		KStatementTypeDelete		// required
 *		KStatementTypeInsert		// required
 *		KStatementTypeCommit		// required
 *		KStatementTypeFolderInfo	// required
 */
const TStringUniqueKey KPropertyStringFolder = {KCID_MMessagingDataAccessConnection, 0x1010};


/**
 * The start and end MessageId's for a range of items.
 *
 * MessageId's are 32-bit identifiers for items which should (pretty) much 
 * uniquely identify an item within a given account and folder.
 *
 * Expected to be set in the following statements:
 *
 *		KStatementTypeSelect // optional -- use same start and end to find a single
 *								item -- this will usually (barring duplicates)
 *								produce a result array containing 1 item.
 *
 *		KStatementTypeDelete // required -- use same start and end to delete
 *							 // a single item.
 *
 */
const TIntUniqueKey KPropertyIntMessageIdRangeStart = {KCID_MMessagingDataAccessConnection, 0x1020};
const TIntUniqueKey KPropertyIntMessageIdRangeEnd = {KCID_MMessagingDataAccessConnection, 0x1030};


/**
 * A pointer to item(s) to perform actions on such as insert.
 *
 * Expected to be set in the following statements:
 *
 *		KStatementTypeDelete // optional -- not needed if you specify ranges.
 *		KStatementTypeInsert // required
 *		KStatementTypeCommit // required
 */
const TObjectUniqueKey KPropertyObjectItem = {KCID_MMessagingDataAccessConnection, 0x1040};



const TInt KIID_MDataAccessResultArrayFilter = 0x1000027;


/**
 * Some MDataAccessResultArray's may support application of a filter.
 *
 */
class MDataAccessResultArrayFilter : public MUnknown
{
public:
	virtual void SetMessageIdRange( MMessage::TMessageId aRangeStart, MMessage::TMessageId aRangeEnd ) = 0;
	virtual void SetTimeRange( TTime aRangeStart, TTime aRangeEnd ) = 0;
	
	virtual void SetSendingStateFilter( TMessageAttributes::TMessageSendingState aSendingStateFilter ) = 0;
	virtual void ClearSendingStateFilter() = 0;
	
	virtual void ApplyL() = 0;
protected:
	virtual ~MDataAccessResultArrayFilter() {}
};


// "TODO: Find a better place of creating the temp file, or at least a better way of obtaining the temp path."

#if (__S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__) || (__UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__)
_LIT( KTempStorageDirectory, "store\\_temp\\");
#else // __S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__

#if __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
_LIT( KTempStorageDirectory, "c:\\system\\apps\\bluewhalemail\\messaging\\__TEMP\\");
#else // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__
_LIT( KTempStorageDirectory, "c:\\system\\messaging\\__TEMP\\");
#endif // __S60_VERSION__ > __S60_V1_FP2_VERSION_NUMBER__

#endif // __S60_VERSION__ >= __S60_V2_FP2_VERSION_NUMBER__





/**
 * This is the from address which will be used by default on messages if not already set.
 *
 * Note that we keep everything as UNICODE UCS-2 for "as long as possible" through message handling process.
 */
const TStringUniqueKey KPropertyStringAccountFromAddress		= {KCID_MMessagingDataAccessConnection, 0x93ef51 };



const TInt KMaxBodySizeToStoreInternally = 10000;

/**
 * TODO: These will be cleaned up soon to support multiple accounts.
 */
_LIT( KDefaultAccount, "account1");
_LIT( KDefaultInboxFolder, "Inbox");
_LIT( KDefaultSentItemsFolder, "Sent Items");


_LIT( KLoggingAccount, "__LOGGING" );
_LIT( KLoggingErrorFolder, "__ERROR" );
_LIT( KLoggingInfoFolder, "__INFO" );
_LIT( KLoggingDebugFolder, "__DEBUG" );





#endif /* __MESSAGING_DATA_ACCESS_CONNECTION_H__ */
