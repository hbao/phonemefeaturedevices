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

/**
 * @file
 *
 * PCSL memory functions.
 *
 */

#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <pcsl_memory.h>
#include <pcsl_print.h>
#include <OS_symbian.hpp>

#ifdef __DEBUG_PCSL_MEMORY__
	#define MYDEBUG_PRINT(_XX) pcsl_print(_XX)
#else
	#define MYDEBUG_PRINT(_XX) 
#endif

/**
 * Allocate a memory chunk that can be shrunk, or expanded (up to max_size)
 * The returned pointer is <alignment>-bytes aligned.
 *
 * This function is called by the ObjectHeap class in the CLDC package
 * to manage the memory space for the Java heap.
 *
 * @param initial_size Initial size of the memory chunk
 * @param max_size Maximum size
 * @param alignment Alignment
 *
 * @return Pointer to allocated memory chunk if successful
 *	   NULL if unsucessful.
 *
 */
void* pcsl_mem_allocate_chunk(unsigned int initial_size,
                              unsigned int max_size,
                              unsigned int alignment) 
{
#ifdef __WINS__
	TBuf8<256> buffer;
	buffer.Format(_L8("pcsl_mem_allocate_chunk %d %d %d\n"),initial_size,max_size,alignment);
	MYDEBUG_PRINT((const char*)buffer.PtrZ());
#endif
	void* ptr = static_cast<MApplication*>(Dll::Tls())->NewChunk(initial_size,max_size+alignment,alignment,alignment);
#ifdef __WINS__
	buffer.Format(_L8("Allocated chunk 0x%08x\n"),ptr);
	MYDEBUG_PRINT((const char*)buffer.PtrZ());
#endif
   return ptr;
}
/**
 * Expand or shrink a chunk returned by allocate_chunk().
 * The chunk is never moved.
 *
 * This function is called by the ObjectHeap class in the CLDC package
 * to manage the memory space for the Java heap.
 *
 * @param chunk_ptr Pointer to memory chunk
 * @param new_size New size of the chunk
 *
 * @return old size if successful; 0 if fails to expand (shrink will
 * always succeed).
 *
 */
unsigned int pcsl_mem_adjust_chunk(void *chunk_ptr, unsigned int new_size) 
{
	MYDEBUG_PRINT("pcsl_mem_adjust_chunk\n");
    return static_cast<MApplication*>(Dll::Tls())->AdjustChunk(chunk_ptr,new_size);
}

/**
 * Free a chunk returned by pcsl_allocate_chunk()
 *
 * This function is called by the ObjectHeap class in the CLDC package
 * to manage the memory space for the Java heap.
 *
 * @param chunk_ptr Pointer to memory chunk
 *
 */
void pcsl_mem_free_chunk(void *chunk_ptr) 
{
	static_cast<MApplication*>(Dll::Tls())->FreeChunk(chunk_ptr);
}

/**
 * Initializes a PCSL memory pool.
 * <p><b>NOTE:</b> This function must only be called once.
 *
 * @param startAddr Starting address of memory pool. If NULL, it will
 *                  be either statically or dynamically allocated.
 * @param size size, in bytes, of the memory pool; if size is -1, the
 *	  default memory pool size will be used
 *
 * @return 0 if the function returns successfully; a non-zero value
 * otherwise
 */
int pcsl_mem_initialize_impl0(void *startAddr, int size) 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_initialize_impl0 !!!!!\n");
    return 0;
}

/**
 * Takes any actions necessary to safely terminate the memory
 * subsystem.
 */
void pcsl_mem_finalize_impl0() 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_finalize_impl0 !!!!!\n");
}

/**
 * Gets the maximum amount of heap space, in bytes, available to the system
 * for allocation. This value is constant throughout the lifetime of the
 * process.
 *
 * @return the maximum number of bytes available on the heap, or -1 if the
 * information is not available
 */
int   pcsl_mem_get_total_heap_impl0() 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_get_total_heap_impl0 !!!!!\n");
    return -1;
}

/**
 * Gets the current amount of unused heap space, in bytes. This value
 * changes with every <tt>pcslMalloc</tt> and <tt>pcslFree</tt>
 * function call.
 *
 * @return the number of bytes of heap space that are currently unused, or -1
 * if the information is not available
 */
int pcsl_mem_get_free_heap_impl0() 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_get_free_heap_impl0 !!!!!\n");
    return -1;
}

/**
 * Displays the current state of the memory sub-system.
 *
 * @param countMemoryLeaksOnly amount of data to gather: if is non-zero,
 *	  display allocated blocks of memory; otherwise display both allocated
 *	  and free blocks
 *
 * @return the number of allocated blocks, or -1 if there was an error
 */
int pcsl_mem_malloc_dump_impl0(int countMemoryLeaksOnly) 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_malloc_dump_impl0 !!!!!\n");
    return -1;
}

/**
 * Allocates memory from the private PCSL memory pool.
 *
 * @param size number of bytes to allocate
 *
 * @return pointer to the newly allocated memory, or NULL if the system cannot
 * fulfil the allocation request
 */

void* pcsl_mem_malloc_impl0(unsigned int size) 
{
	TAny* ptr = User::Alloc(size);
#ifdef __WINS__
	TBuf8<256> buffer;
	buffer.Format(_L8("pcsl_mem_malloc_impl0 %d 0x%08x\n"),size,ptr);
	MYDEBUG_PRINT((const char*)buffer.PtrZ());
#endif
    return ptr;
}

/**
 * Allocates memory from the private PCSL memory pool, and clears the
 * memory.
 *
 * @param nelem number of elements to allocate
 * @param elsize size of one element
 *
 * @return pointer to the newly allocated and cleared memory, or NULL if the
 * system cannot fulfil the allocation request
 */
void* pcsl_mem_calloc_impl0(unsigned int nelem, unsigned int size) 
{
	pcsl_print("!!!!! NOT IMPLEMENTED pcsl_mem_calloc_impl0 !!!!!\n");
    return NULL;
}

/**
 * Re-allocates memory from the private PCSL memory pool.
 *
 * @param ptr original memory pointer, or null if this function should
 *	  act like a call to pcsl_malloc_impl
 * @param size new amount of memory needed, in bytes, or 0 to have
 *	  this function act like a call to pcslFreeImpl
 *
 * @return pointer to the re-allocated memory, or NULL if the system cannot
 * fulfil the allocation request
 */
void* pcsl_mem_realloc_impl0(void* ptr, unsigned int size) 
{
	TAny* ret = User::ReAlloc(ptr,size);
	return ret;
}

/**
 * Duplicates the given string after allocating the memory for it.
 *
 * @param s1 string to duplicate
 *
 * @return pointer to the duplicate string, or NULL if the system cannot
 * fulfil the allocation request
 */
char* pcsl_mem_strdup_impl0(const char* s1) 
{
    char *p = (char *)pcsl_mem_malloc_impl0(strlen(s1) + 1);

    if ( p != NULL ) {
        strcpy(p, s1);
    }
    return(p);
}

/**
 * Frees memory allocated from the private PCSL memory pool
 *
 * @param ptr pointer to the allocated memory
 */
void  pcsl_mem_free_impl0(void* ptr) 
{
	MYDEBUG_PRINT("pcsl_mem_free_impl0\n");
	if(ptr)
	{
		User::Free(ptr);
	}
}
