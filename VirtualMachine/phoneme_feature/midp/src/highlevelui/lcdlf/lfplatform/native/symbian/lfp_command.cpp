/*
 *   
 *
 * Copyright  1990-2006 Sun Microsystems, Inc. All Rights Reserved.
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
#include <e32std.h>
#include <midpMalloc.h>
#include <midpEvents.h>
#include <midpEventUtil.h>
#include <midpError.h>
#include <midputilkni.h>

#include <pcsl_memory.h>
#include <ROMStructs.h>
#include <stdio.h>
#include <OS_symbian.hpp>

//#define __DEBUGCOMMAND__
#ifdef __DEBUGCOMMAND__
	#include <pcsl_print.h>
#endif

static HBufC* JStringToHBufC(jstring java_str)
{
	HBufC* ret = NULL;
	if (KNI_IsNullHandle(java_str)) 
	{
		return KNullDesC().Alloc();
	}
	const jsize length  = KNI_GetStringLength(java_str);
    if (length < 0) 
	{
    	return NULL;
	} 
	else if (length == 0) 
	{
      	return KNullDesC().Alloc();
    } 
	else 
	{
		jchar * buffer = static_cast<jchar *>(pcsl_mem_malloc(length * sizeof(jchar)));
		if (buffer == NULL) 
		{
			return NULL;
		}
		KNI_GetStringRegion(java_str, 0, length, buffer);
	
		ret = HBufC::New(length);
		if(ret)
		{
			TPtr ptr(ret->Des());
			ptr.Copy(static_cast<const TUint16 *>(buffer),length);
		}
		pcsl_mem_free(buffer);
	}
	return ret;
}

#define cppunhand(_type,_ptr)  (*static_cast<_type**>(((void*)_ptr )))

#define cpphand(__ptr)  ((static_cast<jobject>( (void*) & (__ptr) )))

#define getMidpCommandPtr(handle) cppunhand(Java_javax_microedition_lcdui_Command,handle)

#if 0
/**
 * @file
 * Cross platform Abstract Commands related functions.
 */
#endif
/**
 * Free an abstract command list.
 *
 * @param commands pointer to the command list
 * @param length size of the list
 */
static void
freeCommandList(TMidpCommand *commands, int length) {
    int i;
    
    if (commands != NULL) {
        /* Free all of the command strings */
        for (i = 0; i < length; ++i) {
            delete commands[i].shortLabel_str;
            delete commands[i].longLabel_str;
	}
        midpFree(commands);
    }
}
#if 0
/**
 * Called upon VM startup to allocate menu resource.
 */
void
initMenus() {
    cmdmanager_create(&commandManager);
}

/**
 * Called upon VM exit to release menu resource.
 */
void
finalizeMenus() {
    commandManager.hideAndDelete(&commandManager, KNI_TRUE);
}
#endif

/**
 * Re-populate contents of menu and soft buttons.
 * Upon return, the passed-in command list will be freed already.
 *
 * @param commands abstract command list that has been sorted by priority
 * @param length size of the list
 */
static void
MidpCommandSetAll(TMidpCommand *commands, int length) 
{
#ifdef __DEBUGCOMMAND__
	jbyte *buffer = (jbyte *)midpMalloc(256);
	for(int i=0;i<length;i++)
	{
		pcsl_print("Command ");
		TBuf8<256> buffer8;
		buffer8.Copy(commands[i].shortLabel_str->Des());
		pcsl_print((const char*)buffer8.PtrZ());
		pcsl_print(" ");
		buffer8.Copy(commands[i].longLabel_str->Des());
		pcsl_print((const char*)buffer8.PtrZ());
		
		char debugBuffer[256];
		sprintf(debugBuffer," type %d id %d priority %d\n",commands[i].type,commands[i].id,commands[i].priority);
		pcsl_print(debugBuffer);
	}
	midpFree(buffer);
#endif
	// Application code takes ownership of the commands and must free the strings.
	static_cast<MApplication*>(Dll::Tls())->SetMenus(commands,length);
    
}

#define COMMAND_SORT_ALL_TABLE { 		\
	    127, /* COMMAND_TYPE_NONE */ 	\
	    2,   /* COMMAND_TYPE_SCREEN */ 	\
	    5,   /* COMMAND_TYPE_BACK */ 	\
	    7,   /* COMMAND_TYPE_CANCEL */	\
	    3,   /* COMMAND_TYPE_OK */		\
	    4,   /* COMMAND_TYPE_HELP */	\
	    8,   /* COMMAND_TYPE_STOP */	\
	    6,   /* COMMAND_TYPE_EXIT */	\
	    1,   /* COMMAND_TYPE_ITEM */	\
	}

	#define COMMAND_SORT_NEGATIVE_TABLE { 		\
	    127, /* COMMAND_TYPE_NONE */ 	\
	    127, /* COMMAND_TYPE_SCREEN */ 	\
	    2,   /* COMMAND_TYPE_BACK */ 	\
	    1,   /* COMMAND_TYPE_CANCEL */	\
	    127, /* COMMAND_TYPE_OK */		\
	    127, /* COMMAND_TYPE_HELP */	\
	    3,   /* COMMAND_TYPE_STOP */	\
	    4,   /* COMMAND_TYPE_EXIT */	\
	    127, /* COMMAND_TYPE_ITEM */	\
	}
#define COMMAND_SORT_POSITIVE_TABLE { 		\
	    127, /* COMMAND_TYPE_NONE */ 	\
	    127, /* COMMAND_TYPE_SCREEN */ 	\
	    127, /* COMMAND_TYPE_BACK */ 	\
	    127, /* COMMAND_TYPE_CANCEL */	\
	    1,   /* COMMAND_TYPE_OK */		\
	    127, /* COMMAND_TYPE_HELP */	\
	    127, /* COMMAND_TYPE_STOP */	\
	    127, /* COMMAND_TYPE_EXIT */	\
	    127, /* COMMAND_TYPE_ITEM */	\
	}

static const char SORT_ALL_TABLE[10] = COMMAND_SORT_ALL_TABLE;

static const char SORT_NEGATIVE_TABLE[10] = COMMAND_SORT_NEGATIVE_TABLE;

static const char SORT_POSITIVE_TABLE[10] = COMMAND_SORT_POSITIVE_TABLE;


static int
compare(const TMidpCommand  *a, const  TMidpCommand *b, const char table[]) {
    if (a->type == b->type) {
        return a->priority - b->priority;
    } else {
        int aPos =
            ((a->type < 1) || (a->type > 8)) ? 127 : table[a->type];
        int bPos =
            ((b->type < 1) || (b->type > 8)) ? 127 : table[b->type];

        return aPos - bPos;
    }
}

static int
compareForAll(const void *first, const void *second) {
    return compare((TMidpCommand *)first, (TMidpCommand *)second, SORT_ALL_TABLE);
}


TMidpCommand* MidpCommandSortAll(jobject ItemCmdArray, int numItemCmds,
				jobject DispCmdArray, int numDispCmds) {
    int nc = numItemCmds + numDispCmds;
    /* Need to allocate an extra command for qsort. */
    TMidpCommand *c = (TMidpCommand*)midpMalloc((nc+1)*sizeof(TMidpCommand));

    jobjectArray  itemCmds = (jobjectArray)ItemCmdArray;
    jobjectArray  cmds = (jobjectArray)DispCmdArray;

    int      j;

    if (c == NULL) {
        return NULL;
    }

    KNI_StartHandles(2);
    KNI_DeclareHandle(i);
    KNI_DeclareHandle(str);

    /*
     * We need to copy the string data (not just keep a
     * pointer to it) because if the garbage collector is allowed
     * to move the contents of the heap, the pointers will become
     * invalid.
     */
    for (j = 0; j < nc; ++j) {
        /* First fill c array with Item commands from ItemCmdArray;
         * then fill it with Displayable commands from DispCmdArray;
         * Later all these commands will be sorted together.
         */
        if (j < numItemCmds) {
            KNI_GetObjectArrayElement(itemCmds, j, i);
        } else {
            KNI_GetObjectArrayElement(cmds, j - numItemCmds, i);
        }

		str = cpphand( ( getMidpCommandPtr(i)->shortLabel) );
		HBufC* label = JStringToHBufC(str);
		if(!label) 
		{
			break;
		}
		c[j].shortLabel_str = label;
		
		str = cpphand(getMidpCommandPtr(i)->longLabel);
		label = JStringToHBufC(str);
		if(!label)
		{
			delete c[j].shortLabel_str;
			break;
		}
		c[j].longLabel_str = label;
		c[j].priority  = (int)(getMidpCommandPtr(i)->priority);
		c[j].type  = (int)(getMidpCommandPtr(i)->commandType);
		c[j].id  = (int)(getMidpCommandPtr(i)->id);
    } /* end for (j=0; j<nc; ++j); */

    if (j < nc) {
        /* Whoops! We need to undo all previous allocs */
        for (j--; j >= 0; j--) {
            delete c[j].longLabel_str;
            delete c[j].shortLabel_str;
        }
        midpFree(c);
        c = NULL;
    } else if (nc > 1) {
	qsort(c, nc, sizeof(TMidpCommand), compareForAll);
    }

    KNI_EndHandles();
    return c;
}

/**
 * Set the current set of active Abstract Commands.
 * <p>
 * Java declaration:
 * <pre>
 *     updateCommands([Ljavax/microedition/lcdui/Command;I
 *                    [Ljavax/microedition/lcdui/Command;I)V
 * </pre>
 * Java parameters:
 * <pre>
 *     itemCommands    The list of Item Commands that 
 *                     should be active
 *     numItemCommands The number of commands in the list 
 *                     of Item Commands
 *     commands        The list of Commands that should be active
 *     numCommands     The number of commands in the list of 
 *                     Commands
 *     returns:        void
 * </pre>
 */
 
KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_NativeMenu_updateCommands() {
    int numItemCmds = KNI_GetParameterAsInt(2);
    int numCmds     = KNI_GetParameterAsInt(4);

    KNI_StartHandles(2);

    KNI_DeclareHandle(itemCmds);
    KNI_DeclareHandle(cmds);

    KNI_GetParameterAsObject(1, itemCmds);
    KNI_GetParameterAsObject(3, cmds);

	if (numItemCmds == 0 && numCmds == 0) 
	{
        MidpCommandSetAll(NULL, 0);
    } 
	else 
	{
        TMidpCommand *menuList = MidpCommandSortAll(itemCmds, numItemCmds,cmds, numCmds);
						   
        if (menuList != NULL ) 
		{
            MidpCommandSetAll(menuList, numItemCmds + numCmds);
			midpFree(menuList);
        } 
		else 
		{
            MidpCommandSetAll(NULL, 0);
            //KNI_ThrowNew(midpOutOfMemoryError, NULL);
        }
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Calls to platform specific function to draw the command menu on the screen.
 * <p>
 * Java declaration:
 * <pre>
 *     showMenu()V
 * </pre>
 * Java parameters:
 * <pre>
 *   parameters:  none
 *   returns:     void
 * </pre>
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_NativeMenu_showMenu() {
  //  commandManager.show(&commandManager);
    KNI_ReturnVoid();
}

/**
 * Call to platform specific function to dismiss the current menu or
 * popup in the case of setCurrent() being called while the Display
 * is suspended by a system screen.
 * <p>
 * Java declaration:
 * <pre>
 *     dismissMenuAndPopup()V
 * </pre>
 * Java parameters:
 * <pre>
 *   parameters:  none
 *   returns:     void
 * </pre>
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_javax_microedition_lcdui_NativeMenu_dismissMenuAndPopup() {
    // Static commandManager is shared across Displays,
    // following call should only dismiss but not delete
    // the platform widget. It should only be deleted in
    // finalizeMenus() function
    //commandManager.hideAndDelete(&commandManager, KNI_FALSE);
    //lfpport_choicegroup_dismiss_popup();
    KNI_ReturnVoid();
}
#if 0
/**
 * Java notification function for command activated either through menu or
 * soft button.
 *
 * This is NOT a platform dependent function and does NOT need to be ported.
 *
 * @param cmdId identifier of the selected command
 *
 * @return an indication of success or the reason for failure
 */
MidpError
MidpCommandSelected(int cmdId) {
    MidpEvent event;

    MIDP_EVENT_INITIALIZE(event);

    event.type = MIDP_COMMAND_EVENT;
    event.COMMAND = cmdId;

    midpStoreEventAndSignalForeground(event);

    return ALL_OK;  
}
#endif
