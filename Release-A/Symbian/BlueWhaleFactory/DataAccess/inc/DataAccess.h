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



#ifndef __DATA_ACCESS_H__
#define __DATA_ACCESS_H__

#include "Properties.h"




class MDataAccessStatement;



const TInt KIID_MDataAccessConnection = 0x01000001;

/**
 * A connection to a particular storage system.
 *
 * The data access interfaces provide a means to access data
 * in a somewhat standard, object-oriented database kind of way.
 *
 * We follow the usual database pattern:
 *
 * 1) Create a connection to a data source .
 *		[Done using ECOMPLUS -- component Uid is documented elsewhere 
 *		for a given store -- e.g. in MessagingDataAccessConnection.h]
 *
 * 2) Create a statement on the connection.
 *
 * 3) Modify the statement as appropriate.
 *		[Setting key/value pairs on its MProperties interface.]
 *
 * 4) Execute the statement, retrieving a result.
 *
 * 5) Interrogate the result as needed.
 *		[QueryInterfaceL for expected result set interfaces such as 
 *		MDataAccessResultArray.]
 * 
 *
 * For now, there will probably only be one MDataAccessConnection
 * implementation on our system, the "messaging" one.
 *
 * In the future, it may be expedient to add support for different
 * ones such as "calendar" and "contacts".  
 *
 * Discussion: We may even choose to allow for storage of 
 * specific messaging types through the use of separate 
 * MDataAccessConnection's, e.g. "shortmessaging" might be
 * a store presenting MDataAccessConnection which is optimized
 * for many small, short messages.  But I think it will possible
 * to create a single "messaging" implementation generic enough to handle
 * all messages.
 *
 * We may conceivable implement an MDataAccessConnection which
 * is able to open several other (or all) MDataAccessConnection
 * so that statements can be executed across many different underlying
 * stores all at once (e.g. search).  This might provide the right way forward
 * if at some point later we find that "messaging" needs to be 
 * broken down into "emailmessaging" and "shortmessaging" --
 * "messaging" would remain available as before but under the covers
 * would be implemented as a (generic) container pointing to all the
 * different messaging storage mechanisms.
 *
 */
class MDataAccessConnection : public MUnknown
{
public:

	/**
	 * Each connection may implement statements in a different way
	 * and with different capabilities.  For this reason,
	 * when we want an MDataAccessStatement, we must ask the 
	 * MDataAccessConnection to create one for us.
	 * (Of course, there's nothing stopping it from sharing a
	 * common underlying statement implementation with other 
	 * connection types.)
	 */
	virtual MDataAccessStatement * CreateStatementL() = 0;

protected:
	virtual ~MDataAccessConnection() {}
};






class MDataAccessResult;




const TInt KIID_MDataAccessStatement = 0x01000002;

/**
 * This is the interface to a statement component which is obtained
 * from a particular connection.  
 *
 * Because we are mostly an Object-Oriented Database system here, we have no 
 * way of knowing what kind of parameters will be required for a statement.
 * For this reason, the MDataAccessStatement is-a property bag as well,
 * and implements our generic MProperties interface.
 *
 * Specific connections will document which kinds of properties they 
 * support or expect to be set on their MDataAccessStatement,
 * and will document which kinds of MDataAccessResult's they produce.
 *
 */
class MDataAccessStatement : public MProperties
{
public:

	/**
	 * A C++ type for the statement type which will should set in
	 * the KPropertyInt64DataAccessStatementType on the 
	 * MProperties base interface of this MDataAccessStatement.
	 */
	typedef TConstructorlessInt64<KIID_MDataAccessStatement> TStatementType;

	virtual void SetType( const TStatementType & aStatementType ) = 0;

	/**
	 * After setting the desired statement type and any appropriate additional
	 * properties, call this to execute the statement, possibly returning a result.
	 *
	 * Results can be of a different kind for different ExecuteL's
	 * and the expected aResult will be documented for specific
	 * MDataAccessConnections.
	 *
	 * The caller is responsible for lowering the reference count
	 * on any returned result.
	 *
	 * For example, for database "select" statements, usually you will
	 * be able to QueryInterfaceL for the MDataAccessResultArray interface
	 * below.
	 *
	 * For database "insert" or "delete" type statements, a result set on 
	 * return would not make sense -- usually no aResult will have
	 * been set at all, rather ExecuteL not leaving indicates success.
	 *
	 * Some other ExecuteL's might produce e.g. an MProperties
	 * object with requested information.
	 *
	 * You must always specify an aInterfaceId to which you wish 
	 * the result to be casted.  As is our convention, if you would 
	 * like to pay the cost of executing the statement and then 
	 * QueryInterfaceL at your leisure for an appropriate interface,
	 * then request KIID_Unknown which should always succeed
	 * if the ExecuteL returns a result.
	 */
	virtual MUnknown * ExecuteL( TInt aInterfaceId ) = 0;

protected:
	virtual ~MDataAccessStatement() {}
};



/**
 * Macros can often be evil, but the following macro is entirely optional
 * and provides for type safety as well more concise code.
 *
 * It works provided you follow the convention of naming your interface Id's
 * as KIID_MSomeInterface, where MSomeInterface is the name of your interface.
 *
 * e.g.
 *		MSomeInterface * someInterface = ExL( dataAccessStatement, MSomeInterface );
 */
#define ExL( DATA_ACCESS_STATEMENT, INTERFACE )	reinterpret_cast<INTERFACE*>( (DATA_ACCESS_STATEMENT)->ExecuteL( KIID_##INTERFACE ) );






/**
 * Some common statements which most connections will understand.
 * Custom statements for particular connections, as well other
 * parameters which are expected to be set for each statement type
 * are documented in header files for that connection.
 */



const MDataAccessStatement::TStatementType KStatementTypeCommit = {KIID_MDataAccessStatement, 0x130};

const TInt KIID_MDataAccessResultArrayCallback = 0x01000004;

class MDataAccessResultArray;

/**
 * The interface clients implement to obtain updates on a result array.
 *
 */
class MDataAccessResultArrayCallback : public MUnknown
{
public:

	typedef TConstructorlessInt64<KIID_MDataAccessResultArrayCallback> TNotificationType;

	/**
	 * An insertion has taken place.  The new item should be
	 * placed so as to have an index of aIndex.
	 *
	 * Note: aWhichResultArray can be used to disambiguate when you are listening
	 * on multiple MDataAccessResultArray's.
	 */
	virtual void Notify( MDataAccessResultArray * aWhichResultArray, TNotificationType aEventType, TInt aIndex, TInt aNumber = 1) = 0;

protected:
	virtual ~MDataAccessResultArrayCallback() {}
};



const MDataAccessResultArrayCallback::TNotificationType KResultArrayNotificationChange = {KIID_MDataAccessResultArrayCallback, 0x10 };
const MDataAccessResultArrayCallback::TNotificationType KResultArrayNotificationDeletion = {KIID_MDataAccessResultArrayCallback, 0x20 };

/**
 * New item should be placed so as to have index of aIndex.
 */
const MDataAccessResultArrayCallback::TNotificationType KResultArrayNotificationInsertion	= {KIID_MDataAccessResultArrayCallback, 0x30 };

/**
 * A radical change has taken place to the result set
 * data, and the most practical way for reconciling
 * is to reload all data.
 */
const MDataAccessResultArrayCallback::TNotificationType KResultArrayNotificationReset	= {KIID_MDataAccessResultArrayCallback, 0x40 };




const TInt KIID_MDataAccessResultArray = 0x01000003;


/**
 * The type for a uniquely specified action.
 */
typedef TConstructorlessInt64<KIID_MDataAccessResultArray> TAction;


const TAction KActionOpen			= {KIID_MDataAccessResultArray, 0x001};
const TAction KActionDelete			= {KIID_MDataAccessResultArray, 0x002};
const TAction KActionMarkViewed		= {KIID_MDataAccessResultArray, 0x003};
const TAction KActionUnmarkViewed	= {KIID_MDataAccessResultArray, 0x004};
const TAction KActionForward		= {KIID_MDataAccessResultArray, 0x005};
const TAction KActionReplyCompact	= {KIID_MDataAccessResultArray, 0x006};




/**
 * A particular result sub-interface for representing a set of 
 * ordered items.
 *
 * We have chosen not to use the typical [0..Size()-1] way of indexing,
 * as we wish to allow for an optimization in cases were changes occur
 * only to the beginning or end of the array, in which case an implementation 
 * might possibly leave all existing indices alone and just change Min and/or Max.
 *
 * One should always iterate between [Min()..Max()] inclusive.
 *
 */
class MDataAccessResultArray : public MUnknown
{
public:
	/**
	 * Smallest index in result set.
	 *
	 * If there is no data in the result set, Min() > Max().
	 */
	virtual TInt Min() = 0;

	/**
	 * Largest index in result set.
	 *
	 * If there is no data in the result set, Min() > Max().
	 */
	virtual TInt Max() = 0;

	/**
	 * Retrieve an item at the given index.
	 */
	virtual MUnknown * AtL(TInt aIndex, TInt aInterfaceId = KIID_MUnknown) = 0;

	/**
	 * Allow clients to register a callback to be notified of changes.
	 */
	virtual void RegisterForNotificationsL( MDataAccessResultArrayCallback *aNotificationCallback ) = 0;

	/**
	 * Allow clients to perform an action on a range of items.
	 */
	virtual void PerformRangeActionL( const TAction & aAction, TInt aStartIndex = KHugeNegativeInteger, TInt aEndIndex = KHugePositiveInteger) = 0;


protected:
	virtual ~MDataAccessResultArray() {}
};




/**
 * Macros can often be evil, but the following macro is entirely optional
 * and provides for type safety as well more concise code.
 *
 * It works provided you follow the convention of naming your interface Id's
 * as KIID_MSomeInterface, where MSomeInterface is the name of your interface.
 *
 * e.g.
 *		MSomeInterface * someInterface = AiL( resultArray, MSomeInterface );
 */
#define AiL( RESULT_ARRAY, INDEX, INTERFACE )	reinterpret_cast<INTERFACE*>( (RESULT_ARRAY)->AtL( (INDEX), KIID_##INTERFACE ) );





#endif /* __DATA_ACCESS_H__ */
