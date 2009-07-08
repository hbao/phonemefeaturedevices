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

/*
 * @(#)jsr211_registry.h	1.18 06/04/05 @(#)
 *
 */

/**
 * @file
 * @defgroup chapi JSR 211 Content Handler API (CHAPI)
 * @ingroup msa
 * @brief This is the API definition for content handler registry access.
 * ##include <jsr211_registry.h>
 * @{
 * <P>
 * Content handler registry API defines a target-specific component which is
 * responsible for managing registered content handlers. A registery MUST 
 * support registration, unregistration and search facilities; 
 * registered content handlers information MUST be saved between device
 * power cycles.
 * <P>
 * Implementation should release any allocated memory buffer.
 * Search and handler data load results are placed to the special 
 * JSR211_RESULT_* structures which should be filled out by the dedicated 
 * routines: <UL>
 *  <LI> @link jsr211_fillHandler
 *  <LI> @link jsr211_fillHandlerArray
 *  <LI> @link jsr211_fillStringArray
 * </UL>
 * The usage pattern is following:
 *  <PRE>
 *  jsr211_result jsr211_someFunction(..., JSR211_RESULT_STRARRAY* result) {
 *      jsr211_result status;   
 *      const jchar* *buffer;
 *      int n;
 *      n = <determine number of selected strings>
 *      buffer = <allocate memory for n strings>
 *      <select strings from the storage into buffer>
 *      if (status == JSR211_OK) {
 *          status = jsr211_fillStringArray(buffer, n, result);
 *      }
 *      <free allocated buffer>
 *      return status
 *  }
 *  </PRE>
 */

#ifndef _JSR211_REGISTRY_H_
#define _JSR211_REGISTRY_H_

#include "javacall_chapi_registry.h"
#include "jsr211_result.h"


#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/**
 * Result codes for jsr211_execute_handler() method.
 */
typedef enum {
    JSR211_LAUNCH_OK                = 0,    /** OK, handler started */
    JSR211_LAUNCH_OK_SHOULD_EXIT    = 1,    /** OK, handler started 
                                            or is ready to start, 
                                            invoking app should exit. */
    JSR211_LAUNCH_ERR_NOTSUPPORTED  = -1,   /** ERROR, not supported */
    JSR211_LAUNCH_ERR_NO_HANDLER    = -2,    /** ERROR, no requested handler */
    JSR211_LAUNCH_ERR_NO_INVOCATION = -3,    /** ERROR, no invocation queued for 
                                                       requested handler */
    JSR211_LAUNCH_ERROR             = -4    /** common error */
} jsr211_launch_result;

/**
 * Content handler fields enumeration.
 */
typedef enum {
  JSR211_FIELD_ID = 0,     /**< Handler ID */
  JSR211_FIELD_TYPES,      /**< Types supported by a handler */
  JSR211_FIELD_SUFFIXES,   /**< Suffixes supported by a handler */
  JSR211_FIELD_ACTIONS,    /**< Actions supported by a handler */
  JSR211_FIELD_LOCALES,    /**< Locales supported by a handler */
  JSR211_FIELD_ACTION_MAP, /**< Handler action map */
  JSR211_FIELD_ACCESSES,   /**< Access list */
  JSR211_FIELD_COUNT       /**< Total number of fields */
} jsr211_field;

/**
 * Content handlers flags enumeration
 */
typedef enum {
  JSR211_REGISTER_TYPE_STATIC_FLAG = REGISTERED_STATIC_FLAG      /**< Indicates content handler is statically registered */
} jsr211_register_type;

/**
 * Search modes for getHandler0 native implementation.
 * Its values MUST correspond RegistryStore SEARCH_* constants.
 */
typedef enum {
    JSR211_SEARCH_EXACT  = 0,        /** Search by exact match with ID */
    JSR211_SEARCH_PREFIX = 1,        /** Search by prefix of given value */
} jsr211_search_flag;


/**
 * Full content handler informaton for jsr211_register() method.
 */
typedef struct jsr211_content_handler_ {
	const jchar*			id;         /**< Content handler ID */
	const jchar*			suite_id;   /**< Storage where the handler is */
	const jchar*			class_name; /**< Content handler class name */
	jsr211_register_type	flag;		/**< Flag for registered content handlers. */
	int						type_num;   /**< Number of types */
	const jchar**			types;      /**< The types that are supported by this content handler */
	int						suff_num;   /**< Number of suffixes */
	const jchar**			suffixes;   /**< The suffixes of URLs that are supported by this content handler */
	int						act_num;	/**< Number of actions */
	const jchar**			actions;    /**< The actions that are supported by this  content handler */
	int						locale_num; /**< Number of locales */
	const jchar**			locales;    /**< The locales that are supported by this content handler */
	const jchar**			action_map; /**< The action names that are defined by this content handler;
											  size is act_num x locale_num  */
	int						access_num;	/**< Number of accesses */
	const jchar**			accesses;	/**< The accessRestrictions for this ContentHandler */
} jsr211_content_handler;


#define JSR211_CONTENT_HANDLER_INITIALIZER   {        \
    NULL,							/* id         */  \
    NULL,							/* suite_id   */  \
    NULL,							/* class_name */  \
    0,                              /* flag       */  \
    0,                              /* type_num   */  \
    NULL,                           /* types      */  \
    0,                              /* suff_num   */  \
    NULL,                           /* suffixes   */  \
    0,                              /* act_num    */  \
    NULL,                           /* actions    */  \
    0,                              /* locale_num */  \
    NULL,                           /* locales    */  \
    NULL,                           /* action_map */  \
    0,                              /* access_num */  \
    NULL,                           /* accesses   */  \
}


/**
 * Initializes content handler registry.
 *
 * @return JSR211_OK if content handler registry initialized successfully
 */
jsr211_result jsr211_initialize(void);

/**
 * Finalizes content handler registry.
 *
 * @return JSR211_OK if content handler registry finalized successfully
 */
jsr211_result jsr211_finalize(void);

/**
 * Store content handler information into a registry.
 *
 * @param handler description of a registering handler. Implementation MUST NOT 
 * retain pointed object
 * @return JSR211_OK if content handler registered successfully
 */
jsr211_result jsr211_register_handler(const jsr211_content_handler* handler);


/**
 * Deletes content handler information from a registry.
 *
 * @param handler_id content handler ID
 * @return JSR211_OK if content handler unregistered successfully
 */
jsr211_result jsr211_unregister_handler(const jchar* handler_id);

/**
 * Searches content handler using specified key and value.
 *
 * @param caller_id calling application identifier
 * @param key search field id. Valid keys are: <ul> 
 *   <li>JSR211_CHAPI_FIELD_TYPES, <li>JSR211_CHAPI_FIELD_SUFFIXES, 
 *   <li>JSR211_CHAPI_FIELD_ACTIONS. </ul>
 * The special case of JSR211_CHAPI_FIELD_ID is used for testing new handler ID.
 * @param value search value
 * @param result the buffer for Content Handlers normalized result array. 
 *  <br>Use the @link jsr211_appendHandler() jsr211_appendHandler function to fill this structure.
 * @return status of the operation
 */
jsr211_result jsr211_find_handler(const jchar* caller_id,
                        jsr211_field key, const jchar* value,
                        /*OUT*/ JSR211_RESULT_CHARRAY result);

/**
 * Fetches handlers registered for the given suite.
 *
 * @param suiteID requested suite ID.
 * @param result the buffer for Content Handlers result array. 
 *  <br>Use @link jsr211_fillHandlerArray function to fill this structure.
 * @return status of the operation
 */
jsr211_result jsr211_find_for_suite(const jchar*  suite_id, 
                        /*OUT*/ JSR211_RESULT_CHARRAY result);

/**
 * Searches content handler using content URL. This function MUST define
 * content type and return default handler for this type if any.
 *
 * @param caller_id calling application identifier
 * @param url content URL
 * @param action requested action
 * @param handler output value - requested handler.
 *  <br>Use @link jsr211_fillHandler function to fill this structure.
 * @return JSR211_OK if the appropriate handler found
 */
jsr211_result jsr211_handler_by_URL(const jchar* caller_id, 
                        const jchar* url, const jchar* action, 
                        /*OUT*/ JSR211_RESULT_CH handler);

/**
 * Returns all found values for specified field. The allowed fields are: <ul>
 *    <li> JSR211_FIELD_ID, <li> JSR211_FIELD_TYPES, <li> JSR211_FIELD_SUFFIXES,
 *    <li> and JSR211_FIELD_ACTIONS. </ul>
 * Values should be selected only from handlers accessible for given caller_id.
 *
 * @param caller_id calling application identifier.
 * @param field search field id
 * @param result output structure where result is placed to.
 *  <br>Use @link jsr211_fillStringArray function to fill this structure.
 * @return status of the operation
 */
jsr211_result jsr211_get_all(const jchar* caller_id, jsr211_field field,
                        /*OUT*/ JSR211_RESULT_STRARRAY result);

/**
 * Gets the registered content handler for the ID.
 * The query can be for an exact match or for the handler
 * matching the prefix of the requested ID.
 *  <BR>Only a content handler which is visible to and accessible to the 
 * given @link caller_id should be returned.
 *
 * @param caller_id calling application identifier.
 * @param id handler ID.
 * @param mode flag indicating whether exact or prefixed search should be 
 * performed according to @link JSR211_SEARCH_MODE constants.
 * @param handler output value - requested handler.
 *  <br>Use @link jsr211_fillHandler function to fill this structure.
 * @return status of the operation
 */
jsr211_result jsr211_get_handler(const jchar* caller_id, 
                        const jchar* id, jsr211_search_flag mode,
                        /*OUT*/ JSR211_RESULT_CH handler);

/**
 * Loads the handler's data field. Allowed fields are: <UL>
 *  <LI> JSR211_FIELD_TYPES, <LI> JSR211_FIELD_SUFFIXES, 
 *  <LI> JSR211_FIELD_ACTIONS, <LI> JSR211_FIELD_LOCALES, 
 *  <LI> JSR211_FIELD_ACTION_MAP, <LI> and JSR211_FIELD_ACCESSES. </UL>
 *
 * @param id requested handler ID.
 * @param field_id requested field.
 * @param result output structure where requested array is placed to.
 *  <br>Use @link jsr211_fillStringArray function to fill this structure.
 * @return status of the operation
 */
jsr211_result jsr211_get_handler_field(const jchar* id, jsr211_field field_id, 
                        /*OUT*/ JSR211_RESULT_STRARRAY result);

/**
 * Checks whether the internal handlers, if any, are installed.
 * Implemented in jsr211_deploy.c accordingly to JAMS/NAMS mode.
 * @return JSR211_OK or JSR211_FAILED - if registry corrupted or OUT_OF_MEMORY.
 */
jsr211_result jsr211_check_internal_handlers(void);

/** @} */

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif  /* _JSR211_REGISTRY_H_ */
