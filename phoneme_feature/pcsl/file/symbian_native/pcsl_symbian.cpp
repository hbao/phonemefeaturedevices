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

#include <pcsl_file.h>
#include <pcsl_print.h>
#include <OS_Symbian.hpp>
#include <bautils.h>

//#define __DEBUG_PCSL_FILE__

#ifdef __DEBUG_PCSL_FILE__
	#define DEBUG_PRINT(_XX) pcsl_print(_XX);
	#define RDEBUGPRINT(_XX) RDebug::Print(_XX);
	#define RDEBUGPRINT2(_XX,_YY) RDebug::Print(_XX,_YY);
	#define RDEBUGPRINT3(_XX,_YY,_ZZ) RDebug::Print(_XX,_YY,_ZZ);
#else
	#define DEBUG_PRINT(_XX)
	#define RDEBUGPRINT(_XX) 
	#define RDEBUGPRINT2(_XX,_YY) 
	#define RDEBUGPRINT3(_XX,_YY,_ZZ) 
#endif



static jchar FILESEP = '\\';
static jchar PATHSEP = ':';

// a static var !!! for testing only
RFs gFs;

TPtr16 PCSLStringToDescriptor(const pcsl_string * aString)
{
	const jchar * name = pcsl_string_get_utf16_data(aString);
	TInt length = pcsl_string_length(aString);
	return TPtr16(const_cast<unsigned short*>(name), length, length);
}

/**
 * The initialize function initials the File System
 */
int pcsl_file_init() 
{
	DEBUG_PRINT("pcsl_file_init\n");
	gFs.Connect();
    return 0;
}

/**
 * Cleans up resources used by file system. It is only needed for the 
 * Ram File System (RMFS).
 * @return 0 on success, -1 otherwise
 */
int pcsl_file_finalize() 
{
	DEBUG_PRINT("pcsl_file_finalize\n");
	gFs.Close();
    return 0;
}

/**
 * The open function creates and returns a new file identifier for the 
 * file named by filename. Initially, the file position indicator for the 
 * file is at the beginning of the file. The argument  creationMode  is used 
 * only when a file is created
 */
int pcsl_file_open(const pcsl_string * fileName, int flags, void **handle) 
{
	int ret = -1;
	RFile* file;
	TPtr16 buf = PCSLStringToDescriptor(fileName);

	RDEBUGPRINT2(_L("pcsl_file_open %S"),&buf);
	
	TUint symflags = EFileRead;
	switch((flags & 0x0f))
	{
		case PCSL_FILE_O_RDONLY:
			symflags = EFileRead | EFileShareAny;
			break;
		case PCSL_FILE_O_WRONLY:
			symflags = EFileWrite | EFileShareAny;
			break;
		case PCSL_FILE_O_RDWR:
			symflags = EFileRead | EFileWrite | EFileShareAny;
			break;
	}
	file = new RFile();
	if(file)
	{
		TInt err  = KErrNone;
		if(flags & PCSL_FILE_O_TRUNC)
		{
			err = file->Replace(gFs, buf,symflags);
		}
		else
		{
			err = file->Open(gFs,buf,symflags);
		}
		if(err != KErrNone && (flags & PCSL_FILE_O_CREAT)) // need to create the file
		{
			err  = file->Create(gFs,buf,symflags);
		}
		if(err == KErrNone)
		{
			*handle = (TAny*)file;
			ret = 0;
		}
		else
		{
			delete file;
			*handle = 0;
		}
	}
#ifdef __WINS__
	RDEBUGPRINT3(_L("open result %d 0x%08x"),ret,*handle);
#endif
    return ret;
}


/**
 * BlueWhaleSystems fix: - Michael Maguire - 15 Aug 2007
 *
 * PCSL already contained an open function, but Symbian supports the concept
 * of RFile::Replace, which opens a new file, nuking existing contents if already exists.
 *
 * From the Symbian docs:
 *
 * "Replaces a file. If there is an existing file with the same name, this function 
 * overwrites it. If the file does not already exist, it is created."
 * 
 * This is extremely handy for temporary file creation and beats checking
 * for existence, deleting and opening.
 */
int pcsl_file_replace(const pcsl_string * fileName, int flags, void **handle) 
{
	int ret = -1;
	RFile* file;
	TPtr16 buf = PCSLStringToDescriptor(fileName);

	TUint symflags = EFileRead;
	switch((flags & 0x0f))
	{
		case PCSL_FILE_O_RDONLY:
			symflags = EFileRead;
			break;
		case PCSL_FILE_O_WRONLY:
			symflags = EFileWrite;
			break;
		case PCSL_FILE_O_RDWR:
			symflags = EFileRead | EFileWrite ;
			break;
	}
	file = new RFile();
	if(file)
	{
		TInt err  = file->Replace(gFs,buf,symflags);
		if(err == KErrNone)
		{
			*handle = (TAny*)file;
			ret = 0;
		}
		else
		{
			delete file;
			*handle = 0;
		}
	}
#ifdef __WINS__
	RDEBUGPRINT3(_L("replace result %d 0x%08x"),ret,*handle);
#endif
    return ret;
}


/**
 * The close function  loses the file with descriptor identifier in FS. 
 */
int pcsl_file_close(void *handle) 
{
	RDEBUGPRINT2(_L("pcsl_file_close 0x%08x"),handle);
	if(handle)
	{
		RFile* file = static_cast<RFile*>(handle);
		file->Close();
		delete file;
		return 0;
	}
	return -1;
}

/**
 * The read function reads up to size bytes from the file with descriptor identifier , 
 * storing the results in the buffer.
 */
int pcsl_file_read(void *handle, unsigned  char *buf, long size)
{
	//DEBUG_PRINT("pcsl_file_read\n");
	TInt ret = -1;
	if(handle)
	{
		TPtr8 buffer(buf,size);
		ret = static_cast<RFile*>(handle)->Read(buffer,size);
		if (ret == KErrNone)
		{
			ret = buffer.Length();
		}
	}
    return ret;
}

/**
 * The write function writes up to size bytes from buffer to the file with descriptor 
 * identifier. 
 * The return value is the number of bytes actually written. This is normally the same 
 * as size, but might be less (for example, if the persistent storage being written to
 * fills up).
 */
int pcsl_file_write(void *handle, unsigned char* buffer, long length)
{
	DEBUG_PRINT("pcsl_file_write\n");
	if(handle)
	{
		TPtr8 buf(buffer,(TInt)length,(TInt)length);
		TInt read = static_cast<RFile*>(handle)->Write(buf);
		static_cast<RFile*>(handle)->Flush();
		return length;
	}
    return -1;
}

/**
 * The unlink function deletes the file named filename from the persistent storage.
 */
int pcsl_file_unlink(const pcsl_string * fileName)
{
	int ret = -1;
	DEBUG_PRINT("pcsl_file_unlink\n");
	TPtr16 buf = PCSLStringToDescriptor(fileName);
#ifdef __WINS__
	RDEBUGPRINT2(_L("%S"),&buf);
#endif	
	int status = gFs.Delete(buf);
	RDEBUGPRINT2(_L("Delete error %d"),status);
	if(status == KErrNone)
	{
		ret = 0;
	}
    return ret;
}

/**
 * The  truncate function is used to truncate the size of an open file in storage.
 */
int pcsl_file_truncate(void *handle, long size)
{
	TInt result = KErrGeneral;
	if(handle)
	{
		result = static_cast<RFile*>(handle)->SetSize(size);
	}
	if(result == KErrNone)
	{	
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
 * The lseek function is used to change the file position of the file with descriptor 
 * identifier 
 */
long pcsl_file_seek(void *handle, long offset, long position)
{
	//DEBUG_PRINT("pcsl_file_seek\n");
	if(handle)
	{
		TSeek mode;
		switch(position)
		{
			case PCSL_FILE_SEEK_SET:
				mode = ESeekStart;
				break;
			case PCSL_FILE_SEEK_CUR:
				mode = ESeekCurrent;
				break;
			case PCSL_FILE_SEEK_END:
				mode = ESeekEnd;
				break;
		}
		TInt off = offset;
		if(static_cast<RFile*>(handle)->Seek(mode,off) == KErrNone)
		{
			return off;
		}
	}
	return -1;
}

/**
 * FS only need to support MIDLets to quiry the size of the file. 
 * Check the File size by file handle
 */
long pcsl_file_sizeofopenfile(void *handle)
{
	DEBUG_PRINT("pcsl_file_sizeofopenfile\n");
	if(handle)
	{
		TInt size;
		if(static_cast<RFile*>(handle)->Size(size) == KErrNone)
		{
#ifdef __WINS__
		RDEBUGPRINT2(_L("File is %d bytes long"),size);
#endif
			return size;
		}
	}
    return -1;
}

/**
 * FS only need to support MIDLets to quiry the size of the file. 
 * Check the File size by file name
 */
long pcsl_file_sizeof(const pcsl_string * fileName)
{
	TPtr16 buf = PCSLStringToDescriptor(fileName);
	RDEBUGPRINT2(_L("pcsl_file_sizeof %S"),&buf);
	TEntry entry;
	if(gFs.Entry(buf,entry ) == KErrNone)
	{
		TInt size =  entry.iSize;
		RDEBUGPRINT2(_L("pcsl_file_sizeof %d"),size);
		return size;
	}
	else
	{
		return -1;
	}
}

/**
 * Check if the file exists in FS storage.
 */
int pcsl_file_exist(const pcsl_string * fileName)
{
	DEBUG_PRINT("pcsl_file_exist\n");
	int ret = -1;
	TEntry entry;
	TPtr16 buf = PCSLStringToDescriptor(fileName);
#ifdef __WINS__
	RDEBUGPRINT2(_L("%S"),&buf);
#endif	
	if(buf[buf.Length()-1] == '\\')
	{
		buf.Delete(buf.Length()-1,1);
	}
	RDEBUGPRINT2(_L("%S"),&buf);
	TUint atts;
	TInt err = gFs.Att(buf, atts);
	if (err == KErrNone)
	{
		if (atts & KEntryAttNormal || atts & KEntryAttArchive)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
	}
	else
	{
		TFileName buf2(buf);
		buf2.Append('\\');
		if (BaflUtils::PathExists(gFs, buf2) || err == KErrPermissionDenied)
		{
			ret = 0;
		}
	}
	RDEBUGPRINT2(_L("return %d"),ret);
	return ret;
}

/* Force the data to be written into the FS storage */
int pcsl_file_commitwrite(void *handle)
{
	if(handle)
	{
		RFile* file = static_cast<RFile*>(handle);
		return file->Flush();
	}
	return -1;
}

/**
 * The rename function updates the filename.
 */
int pcsl_file_rename(const pcsl_string * oldName, const pcsl_string * newName)
{
	int ret = -1;
	TFileName from = PCSLStringToDescriptor(oldName);
	if (from.Length() && (from[from.Length() - 1] == '\\' || from[from.Length() - 1] == '/'))
	{
		from.SetLength(from.Length() - 1);
	}
	TFileName to = PCSLStringToDescriptor(newName);
	if (to.Length() && (to[to.Length() - 1] == '\\' || to[to.Length() - 1] == '/'))
	{
		to.SetLength(to.Length() - 1);
	}
	if(gFs.Rename(from,to) != KErrNone)
	{
		if(gFs.Replace(from,to) == KErrNone)
		{
			ret = 0;
		}
	}
	else
	{
		ret = 0;
	}
    return ret;
}

/**
 * The getFreeSpace function checks the size of free space in storage. 
 */
long pcsl_file_getfreespace()
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_file_getfreespace !!!!!\n");
	return 0;
}

/**
 * The getUsedSpace function checks the size of used space in storage. 
 */
long pcsl_file_getusedspace(const pcsl_string * systemDir)
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_file_getusedspace !!!!!\n");
    return 0;
}

class CDirIterator : public CBase
{
	public:
		CDirIterator(const TDesC& aPath):iNext(0)
		{
            RDEBUGPRINT2(_L("CDirIterator %S"),&aPath);
			if(aPath[aPath.Length()-1] == '\\')
			{
				// get the whole directory
				iPath = aPath.Alloc();
                iWild = NULL;
			}
			else
			{
				// partial search
				_LIT(KWildcard,"*.*");
                TParsePtrC parser(aPath);
                if(parser.DrivePresent())
                {
                    iPath = parser.DriveAndPath().Alloc();
                }
                else
                {
                    iPath = parser.Path().Alloc();
                }
                
                iWild = HBufC::New(parser.Name().Length() + KWildcard().Length());
                iWild->Des().Copy(parser.Name());
                iWild->Des().Append(KWildcard());
			}
		}
		~CDirIterator()
		{
            delete iPath;
            delete iWild;
			iEntries.Close();
		}
		void GetEntries()
		{
			RDir dir;
			TEntryArray results;
            HBufC * path;
            if(iWild)
            {
                path = HBufC::New(iPath->Des().Length() + iWild->Des().Length());
                path->Des().Copy(*iPath);
                path->Des().Append(*iWild);
            }
            else
            {
                path = iPath->Des().Alloc();
            }
			if(dir.Open(gFs,*path,KEntryAttNormal|KEntryAttDir) == KErrNone)
			{
				TInt ret = KErrNone;
				TInt c = 0;
				while(ret == KErrNone)
				{
					ret = dir.Read(results);
					c = results.Count();
					for(TInt i=0;i<c;i++)
					{
						RDEBUGPRINT2(_L("CDirIterator::Entry %S"),&(results[i].iName));
						iEntries.Append(results[i]);
					}
				}
			}
            delete path;
			dir.Close();
		}

		TEntry GetNext()
			{
				return iEntries[iNext++];
			}
		TBool HasMore()
			{
				RDEBUGPRINT2(_L("HasMore %d"),(iNext < iEntries.Count()));
				return iNext < iEntries.Count() ? ETrue : EFalse;
			}
		TDesC& Path(){return *iPath;}
	private:
		HBufC* iPath;
        HBufC* iWild;
		TInt iNext;
		RArray<TEntry> iEntries;
};
/**
 * The opendir function opens directory named dirname. 
 */
void* pcsl_file_openfilelist(const pcsl_string * dirName)
{
	void* ret = 0;
	TPtr16 buf = PCSLStringToDescriptor(dirName);
	DEBUG_PRINT("pcsl_file_openfilelist\n");
	if(static_cast<MApplication*>(Dll::Tls()) == NULL)
	{
		DEBUG_PRINT("Application is NUL !!!!!\n");
	}
	CDirIterator* it = new CDirIterator(buf);
	if(it)
	{
		it->GetEntries();
		ret = (TAny*)it;
	}
	RDEBUGPRINT3(_L("pcsl_file_openfilelist 0x%08x %S"),ret,&buf);
    return ret;
}

/**
 * The mkdir function closes the directory named dirname. 
 */
int pcsl_file_closefilelist(void *handle)
{
	if(handle)
	{
		RDEBUGPRINT2(_L("pcsl_file_closefilelist 0x%08x"),handle);
		CDirIterator* it = static_cast<CDirIterator*>(handle);
		delete it;
	}
	return 0;
}



/* The getNextEntry function search the next file which is  specified DIR */
int pcsl_file_getnextentry(void *handle,const pcsl_string * string, pcsl_string * result)
{
	int ret = -1;
    *result = PCSL_STRING_NULL;
	DEBUG_PRINT("->pcsl_file_getnextentry\n");
	if(handle)
	{
		CDirIterator* it = static_cast<CDirIterator*>(handle);
		if(it->HasMore())
		{
			TEntry entry = it->GetNext();
			HBufC* name = HBufC::New(entry.iName.Length() + it->Path().Length());
			name->Des().Copy(it->Path());
			name->Des().Append(entry.iName);
			pcsl_string_convert_from_utf16(name->Des().Ptr(),name->Des().Length(),result);
			DEBUG_PRINT("<-pcsl_file_getnextentry OK\n");
			delete name;
			return 0;
		}
	}
    DEBUG_PRINT("<-pcsl_file_getnextentry with fail\n");
	return ret;
}

jchar pcsl_file_getfileseparator() 
{
    return FILESEP;
}

jchar pcsl_file_getpathseparator() 
{
	return PATHSEP;
}

