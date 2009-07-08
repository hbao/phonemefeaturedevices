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

/** \file GlobalDefinitions_cwwin.hpp
 * Global definitions for Metrowerks CodeWarrior for Windows
 *
 * This file holds compiler-dependent includes,
 * globally used constants & types, class (forward)
 * declarations and a few frequently used utility functions.
 */

#include <e32def.h>
#include <e32math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define vsnprintf(b,s,f,a) vsprintf(b,f,a)

// Compiler-specific primitive types
typedef unsigned int     uintptr_t;
typedef signed   int     intptr_t;

//----------------------------------------------------------------------------
// Additional Java basic types

typedef unsigned  char   jubyte;
typedef TUint16 jushort;
typedef TUint32 juint;
typedef unsigned long long int julong;
typedef long long int jlong;
//----------------------------------------------------------------------------
// Special (possibly not-portable) casts
// Cast floats into same-size integers and vice-versa w/o changing bit-pattern

inline jint    jint_cast   (jfloat  x)           { return *(jint*   )&x; }
inline jlong   jlong_cast  (jdouble x)           { return *(jlong*  )&x; }

inline jfloat  jfloat_cast (jint    x)           { return *(jfloat* )&x; }
inline jdouble jdouble_cast(jlong   x)           { return *(jdouble*)&x; }

//----------------------------------------------------------------------------
// Debugging

// IMPL_NOTE: CodeWarrior preprocessor is very unhappy about __asm
// IMPL_NOTE: and _IGNORE_ME_(0 combined in one macro...
#ifdef __WINS__
static inline void breakpoint_impl() { __asm { int 3 }; }
#define BREAKPOINT breakpoint_impl()
#else // __WINS__

#if ENABLE_THUMB_VM

#  define BREAKPOINT __asm__ ("swi 0x01")
#  define panic() __asm__ ("swi 0x01")
inline unsigned int _rotr(unsigned int x, unsigned int y) {
    int result;
    asm("ror %0, %2"         : "=l" (result) : "0" (x), "l" (y));
    return result;
}

#else //ENABLE_THUMB_VM
#  define BREAKPOINT __asm__ ("swi 0x9f0001")
#  define panic()  __asm__(".word 0xE6000010")
inline unsigned int _rotr(unsigned int x, unsigned int y) {
    int result;
    asm("mov %0, %1, ror %2" : "=r" (result) : "r" (x), "r" (y));
    return result;
}
#endif // ENABLE_THUMB_VM

inline unsigned int _rotl(unsigned int x, unsigned int y) 
{
  return _rotr(x, 32 - y);
}

#define __cdecl
#endif // __WINS__


//----------------------------------------------------------------------------
// Checking for NaN-ness

inline int g_isnan(jfloat  f)                    { return Math::IsNaN(f); }
inline int g_isnan(jdouble f)                    { return Math::IsNaN(f); }

//----------------------------------------------------------------------------
// Checking for finiteness

inline int g_isfinite(jfloat  f)                 { return Math::IsInfinite(f); }
inline int g_isfinite(jdouble f)                 { return Math::IsInfinite(f); }

//----------------------------------------------------------------------------
// Constant for jlong (specifying an long long constant is C++ compiler specific)

const jlong min_jlong = 0x8000000000000000ULL;
const jlong max_jlong = 0x7fffffffffffffffLL;

//----------------------------------------------------------------------------
// Miscellaneous

//  inline int vsnprintf(char* buf, size_t count, const char* fmt, va_list argptr) {
//    return _vsnprintf(buf, count, fmt, argptr);
//  }

//----------------------------------------------------------------------------
// Macros about compiler-specific behavior.

// See comments in UsingFastOops::pre_fast_oops_verification().
#define C_COMPILER_STRICT_INCREASING_STACK

// Turn off innocuous "unreferenced formal parameter" warning
#pragma warning (disable:4100)

// Turn off innocuous "conditional expression is constant" warning
#pragma warning (disable:4127)

// Turn off innocuous "assignment operator could not be generated" warning
#pragma warning (disable:4512)

// // Always enable warning "local variable is initialized but not referenced"
// #pragma warning (enable:4189)
