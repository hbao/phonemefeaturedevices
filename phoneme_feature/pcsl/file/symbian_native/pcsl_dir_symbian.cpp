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
#include <f32file.h>
#include <pcsl_directory.h>
#include <pcsl_print.h>
#include <bautils.h>

//#define __DEBUG_PCSL_FILE__

#ifdef __DEBUG_PCSL_FILE__
	#define DEBUG_PRINT(_XX) pcsl_print(_XX);
	#define DEBUG_PRINT2(_XX,_YY) { const signed char* __AA__str = (const signed char *)pcsl_string_get_utf8_data(_YY);pcsl_print(_XX);pcsl_print((const char*)__AA__str);pcsl_print("\n");pcsl_string_release_utf8_data(__AA__str,_YY);}
#else
	#define DEBUG_PRINT(_XX)
	#define DEBUG_PRINT2(_XX,_YY)
#endif

extern RFs gFs;

extern TPtr16 PCSLStringToDescriptor(const pcsl_string * aString);

/**
 * Check if the path is a directory and it exists in file system storage.
 * @param path name of file or directory
 * @return 1 if it exists and is a directory,
 *         0 otherwise,
 *         -1 in case of an error.
 */
int pcsl_file_is_directory(const pcsl_string * path) 
{
	int ret = -1;
	DEBUG_PRINT("pcsl_file_is_directory\n");
	TEntry entry;
	TPtr16 buf = PCSLStringToDescriptor(path);

	if(buf[buf.Length()-1] == '\\')
	{
		buf.Delete(buf.Length()-1,1);
	}

	TUint atts;
	TInt err = gFs.Att(buf, atts);
	if (err == KErrNone)
	{
		if (atts & KEntryAttVolume || atts & KEntryAttDir)
		{
			DEBUG_PRINT(" YES\n");
			ret = 1;
		}
		else
		{
			DEBUG_PRINT(" NO\n");
			ret = 0;
		}
	}
	else
	{
		TFileName buf2(buf);
		buf2.Append('\\');
		if (BaflUtils::PathExists(gFs, buf2) || err == KErrPermissionDenied)
		{
			DEBUG_PRINT(" YES\n");
			ret = 1;
		}
	}
	return ret;
}

/**
 * The mkdir function creates directory   
 * @param dirName name of directory to be created
 *                 In case of hierarchial systems, this can be a path
 * @return 0 on success, -1 otherwise
 * 
 */
int pcsl_file_mkdir(const pcsl_string * dirName) 
{
	int ret = -1;
	DEBUG_PRINT("pcsl_file_mkdir\n");
	TPtr16 buf = PCSLStringToDescriptor(dirName);
#ifdef __WINS__
	RDebug::Print(_L("%S"),&buf);
#endif
	TInt err = gFs.MkDirAll(buf);
	if(err  == KErrNone)
	{
		ret = 0;
	}
#ifdef __WINS__
	RDebug::Print(_L("err %d"),err);
#endif
    return ret;
}

/**
 * The function deletes a directory from the persistent storage.
 * @param dirName name of directory to be deleted
 * @return 0 on success, -1 otherwise
 */
int pcsl_file_rmdir(const pcsl_string * dirName) 
{
	int ret = -1;
	DEBUG_PRINT("pcsl_file_rmdir\n");
	TPtr16 buf = PCSLStringToDescriptor(dirName);
#ifdef __WINS__
	RDebug::Print(_L("%S"),&buf);
#endif

	if(gFs.RmDir(buf) == KErrNone)
	{
		ret = 0;
	}
#ifdef __WINS__
	RDebug::Print(_L("%d"),ret);
#endif
	return ret;
}

/**
 * Checks the size of storage space that is available for user.
 * @param path path is the path name of any file within the file system
 * @return size of available space in storage on success,
 *         -1 otherwise.
 */
jlong pcsl_file_getfreesize(const pcsl_string * path) 
{
	jlong ret = -1;
	DEBUG_PRINT("pcsl_file_getfreesize\n");
	TPtr16 buf = PCSLStringToDescriptor(path);
#ifdef __WINS__
	RDebug::Print(_L("%S"),&buf);
#endif
	TInt driveNumber;
	if (buf.Length() && RFs::CharToDrive(buf[0], driveNumber) == KErrNone)
	{
		TVolumeInfo volumeInfo;
		if (gFs.Volume(volumeInfo, driveNumber) == KErrNone)
		{
			ret = ((jlong)I64HIGH(volumeInfo.iFree) << 32) | (jlong)I64LOW(volumeInfo.iFree);
		}
	}
#ifdef __WINS__
	RDebug::Print(_L("%Ld"),ret);
#endif
    return ret;
}

/**
 * Checks the capacity of the storage.
 * @param path path is the path name of any file within the file system
 * @return size of total space in storage on success, -1 otherwise
 */
jlong pcsl_file_gettotalsize(const pcsl_string * path) 
{
	jlong ret = -1;
	pcsl_print("pcsl_file_gettotalsize\n");
	TPtr16 buf = PCSLStringToDescriptor(path);
#ifdef __WINS__
	RDebug::Print(_L("%S"),&buf);
#endif
	TInt driveNumber;
	if (buf.Length() && RFs::CharToDrive(buf[0], driveNumber) == KErrNone)
	{
		TVolumeInfo volumeInfo;
		if (gFs.Volume(volumeInfo, driveNumber) == KErrNone)
		{
			ret = ((jlong)I64HIGH(volumeInfo.iSize) << 32) | (jlong)I64LOW(volumeInfo.iSize);
		}
	}
#ifdef __WINS__
	RDebug::Print(_L("%Ld"),ret);
#endif
    return ret;
}

//-----------------------------------------------------------------------------

/**
 * Returns value of the attribute for the specified file.
 * @param fileName name of file
 * @param type type of attribute to be got.
 *             Valid values are PCSL_FILE_ATTR_READ, PCSL_FILE_ATTR_WRITE,
 *             PCSL_FILE_ATTR_EXECUTE and PCSL_FILE_ATTR_HIDDEN.
 * @param result returned attribute's value
 * @return 0 on success, -1 otherwise
 */
int pcsl_file_get_attribute(const pcsl_string * fileName, int type, int* result) 
{
	DEBUG_PRINT2("pcsl_file_get_attribute ",fileName);
	TPtr16 buf = PCSLStringToDescriptor(fileName);
	TUint symAtt;
	gFs.Att(buf, symAtt);
	if(type == PCSL_FILE_ATTR_READ && (symAtt == KEntryAttNormal || symAtt & KEntryAttArchive || symAtt & KEntryAttDir))
	{
		*result = 1;
	}
	else if(type == PCSL_FILE_ATTR_WRITE && !(symAtt & KEntryAttReadOnly))
	{
		*result = 1;
	}
	else if(type == PCSL_FILE_ATTR_EXECUTE && symAtt & (symAtt == KEntryAttNormal || symAtt & KEntryAttArchive))
	{
		*result = 1;
	}
	else if(type == PCSL_FILE_ATTR_HIDDEN && symAtt & KEntryAttHidden)
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}
    return 0;
}

/**
 * Sets value of the attribute for the specified file.
 * @param fileName name of file
 * @param type type of attribute to be setted
 *             Valid values are PCSL_FILE_ATTR_READ, PCSL_FILE_ATTR_WRITE,
 *             PCSL_FILE_ATTR_EXECUTE and PCSL_FILE_ATTR_HIDDEN. 
 * @param value 1 to set the attribute, 0 to reset
 * @return 0 on success, -1 otherwise
 */
int pcsl_file_set_attribute(const pcsl_string * fileName, int type, int value) 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_file_set_attribute !!!!!\n");
    return -1;
}

/**
 * Returns the specified time for the file.
 * @param fileName name of file
 * @param fileNameLen length of file name
 * @param type type of time to be got
 *             Valid type is PCSL_FILE_TIME_LAST_MODIFIED.
 * @param result returned time's value in seconds
 * @return 0 on success, -1 otherwise
 */
int pcsl_file_get_time(const pcsl_string * fileName, int type, long* result) 
{
	int ret = -1;
	if(type == PCSL_FILE_TIME_LAST_MODIFIED)
	{
	DEBUG_PRINT("pcsl_file_get_time\n");
	TPtr16 buf = PCSLStringToDescriptor(fileName);
	RFile file;
	if(file.Open(gFs,buf,EFileRead | EFileShareAny) == KErrNone)
	{
		TTime mod;
		if(file.Modified(mod) == KErrNone)
		{
			TTime NineteenSeventy(_L("19700101:000000.000000"));
			TTimeIntervalMicroSeconds interval = mod.MicroSecondsFrom(NineteenSeventy);
			TInt64 time = interval.Int64() / 1000000;
			*result = ((jlong)I64HIGH(time) << 32) | (jlong)I64LOW(time);
			ret = 0;
		}
		file.Close();
	}
	}
    return ret;
}

