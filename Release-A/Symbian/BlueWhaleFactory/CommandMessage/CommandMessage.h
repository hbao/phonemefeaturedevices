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




#ifndef __COMMANDMESSAGE_H__
#define __COMMANDMESSAGE_H__

const TStringUniqueKey PROPERTY_STRING_LOG_COMMAND_MESSAGE 					= {0x59231483, 0x00059000};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_NOTHING					= {0x59231483,0};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_SHOW_SETTINGS			= {0x59231483,1};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_SHOW_ADVANCED_SETTINGS	= {0x59231483,2};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_SHOW_ACCESS_POINT_HELP	= {0x59231483,3};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_SHOW_PROGRESS			= {0x59231483,4};
const TIntUniqueKey REQUESTED_USER_FOLLOWUP_ACTION_SHOW_EULA				= {0x59231483,5};

/**
 * EComPlus component id for an MMessage implementation which also implements MProperties.
 *
 * Recall that the MMessage interface does not imply a user-displayed message.
 * This message is not intended to be displayed or directly manipulated by the user.
 * Rather, we use this message as a mechanism for message passing between different
 * components of the messaging system.
 */
const TInt KCID_MCommandMessage = 0x1020DFE7;



/**
 * The key in which a serialized MProperties command will store the command ID itself.
 *
 * Set the command you wish to be executed at this key and place the message
 * in the appropriate folder for the component you wish to pick up and execute the command.
 *
 * Set any other parameters for the command on the KCID_MCommandMessage you create.
 */
const TInt64UniqueKey KPropertyInt64Command = {KCID_MCommandMessage,0x122201};


/**
 * When KStatementTypeInsert'ing the command message, set the string at
 * KPropertyStringAccount to this value.
 */
_LIT( KCommandsAccount, "__COMMANDS");

/**
 * When KStatementTypeInsert'ing the command message, set the string at
 * KPropertyStringFolder to this value.
 */
_LIT( KCommandsFolder, "commands");
#endif /* __COMMANDMESSAGE_H__ */
