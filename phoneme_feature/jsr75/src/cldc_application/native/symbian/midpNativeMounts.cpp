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

#include <f32file.h>
#include <stdlib.h>
#include <midpNativeMounts.h>
#include <string.h>
#include <midpMalloc.h>
#include <pcsl_memory.h>

extern RFs gFs;


/**
 * Initialize native mount/unmount events listener.
 */
void initNativeFileSystemMonitor() {
}

//char testRoots[] = "root1/\nroot2/";

/**
 * Gets the mounted root file systems.
 *
 * Note: the caller is responsible for calling 'midpFree' after use.
 * @return A string containing currently mounted roots
 *          separated by '\n' character
 */
char* getMountedRoots() 
{
	char* roots = NULL;
	TDriveList list;
	if(gFs.DriveList(list) == KErrNone)
	{
		TBuf8<64> buffer;
		for(int i=0;i<KMaxDrives;i++)
		{
			if(list[i] != 0)
			{
				TChar driveChar;
				gFs.DriveToChar(i,driveChar);
				buffer.Append(driveChar);
				buffer.Append(_L(":/\n"));
			}
		}
		buffer.SetLength(buffer.Length() -1);
		roots = midpStrdup((const char*)buffer.PtrZ());
	}
	return roots;
}

/**
 * Gets OS path for the specified file system root.
 *
 * Note: the caller is responsible for calling 'midpFree' after use.
 * @param root Root name
 * @return The system-dependent path to access the root
 */ 
char* getNativePathForRoot(const char* root) 
{
	// root has the form "C:/", "D:/" etc.
	// just need to replace the slash with Symbian's separator (double backslash)
	TBuf8<KMaxFileName> path(reinterpret_cast<const TUint8*>(root));
	path.SetLength(2);
	path.Append(_L8("\\"));
	
	return midpStrdup((const char*)path.PtrZ());
}


 /**
 * Gets localized names corresponding to roots returned by
 * FileSystemRegistry.listRoots() method. There is one localized name
 * corresponding to each root returned by method. Localized names are in the
 * same order as returned by method and are separated by ';' symbol.
 * If there is no localized name for root, non-localized (logical) name is
 * returned in the property for this root. Root names returned through this
 * property cannot contain ';' symbol.
 *
 * This method is called when the <code>fileconn.dir.roots.names</code> system
 * property is retrieved.
 *
 * @return the localized names for mounted roots separated by ';' symbol
 */
extern "C" {
char* getLocalizedMountedRoots() {
    return NULL;
}
char* getLocalizedPrivateDir()
{
	return NULL;
}
}
