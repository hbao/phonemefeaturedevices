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
 * NOTICE: Portions Copyright (c) 2007-2009 Blue Whale Systems.
 * This file has been modified by Blue Whale Systems on 04May2009.
 * The changes are licensed under the terms of the GNU General Public
 * License version 2. This notice was added to meet the conditions of
 * Section 3.a of the GNU General Public License version 2.
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

# include "incls/_precompiled.incl"
# include "incls/_String.cpp.incl"

bool String::matches(String *that_string) {
  if (this->count() != that_string->count()) {
    return false;
  }
  jint thisHash = this->hashCodeValue();
  jint thatHash = that_string->hashCodeValue();
  if (thisHash && thatHash && thisHash != thatHash)
  {
    return false;
  }
  TypeArray::Raw this_array = this->value();
  TypeArray::Raw that_array = that_string->value();
  address this_base = this_array().base_address();
  address that_base = that_array().base_address();
  this_base += sizeof(jchar) * this->offset();
  that_base += sizeof(jchar) * that_string->offset();

  if (jvm_memcmp(this_base, that_base, count() * 2) == 0) {
    return true;
  } else {
    return false;
  }
}

jint String::compareTo(String *that_string)
{
  int len1 = count();
  int len2 = that_string->count();
  int n = min(len1, len2);

  TypeArray::Raw this_array = value();
  TypeArray::Raw that_array = that_string->value();
  jchar* this_ptr = (jchar*)this_array().base_address() + offset();
  jchar* that_ptr = (jchar*)that_array().base_address() + that_string->offset();
  
  jchar c1;
  jchar c2;
  while (n-- > 0)
  {
    c1 = *this_ptr++;
    c2 = *that_ptr++;
    if (c1 != c2)
    {
      return c1 - c2;
    }
  }
  return len1 - len2;
}

jint String::indexOf(String *that_string, jint fromIndex)
{
  if (fromIndex >= count())
  {
    if (count() == 0 && fromIndex == 0 && that_string->count() == 0)
	{
      // There is an empty string at index 0 in an empty string.
      return 0;
    }
    return -1;
  }
  if (fromIndex < 0)
  {
    fromIndex = 0;
  }
  if (that_string->count() == 0)
  {
	  return fromIndex;
  }

  TypeArray::Raw this_array = value();
  TypeArray::Raw that_array = that_string->value();
  address this_base = this_array().base_address();
  address that_base = that_array().base_address();
  this_base += sizeof(jchar) * (offset() + max(fromIndex, 0));
  that_base += sizeof(jchar) * that_string->offset();

  int max = count() - fromIndex - that_string->count();
  int n = 0;
  while (n <= max)
  {
    if (jvm_memcmp(this_base, that_base, that_string->count() * 2) == 0)
	{
      return fromIndex + n;
    }
    this_base += sizeof(jchar);
    n++;
  }
  return -1;
}

juint String::hash() {
  AllocationDisabler raw_pointers_used_in_this_function;

  juint value = 0;
  if (this->count() != 0)
  {
    value = this->hashCodeValue();
    if (value == 0)
	{
	  TypeArray::Raw char_array = this->value();
      jchar *ptr = (jchar*) char_array().base_address();
      ptr += this->offset();
      jchar *end = ptr + this->count();

      while (ptr < end) {
        value = 31 * value + (juint)(*ptr++);
      }
	  this->set_hashCodeValue(value);
	}
  }
  return value;
}

ReturnOop String::to_cstring(JVM_SINGLE_ARG_TRAPS) {
  UsingFastOops fast_oops;
  int len = count();
  int off = offset();
  TypeArray::Fast ustring = value();
  TypeArray::Fast cstring = Universe::new_byte_array(len+1 JVM_CHECK_0);
  jushort *uptr = ((jushort*)ustring().base_address()) + off;
  jubyte  *cptr = (jubyte*)cstring().base_address();

  for (int i=0; i<len; i++) {
    jushort c = *uptr++;
    *cptr++ = (jubyte)c;
  }

  return cstring;
}

ReturnOop String::replace(jchar aOldChar, jchar aNewChar JVM_TRAPS)
{
  if (aOldChar != aNewChar)
  {
	UsingFastOops fast_oops;
	TypeArray::Fast this_value = value();
    int len = count();
    int i = -1;
	const jchar* this_start = (jchar*)this_value().base_address() + offset();
    while (++i < len)
    {
      if (*(this_start + i) == aOldChar)
      {
        break;
      }
    }
    if (i < len)
    {
      TypeArray::Fast new_value = Universe::new_char_array(len JVM_CHECK_0);
      jchar* new_ptr = (jchar*)new_value().base_address();
      for (int j = 0 ; j < i ; j++)
      {
        *(new_ptr + j) = *(this_start + j);
      }     

      while (i < len)
      {
        jchar c = *(this_start + i);
        *(new_ptr + i) = (c == aOldChar) ? aNewChar : c;
		i++;
      }
	  return Universe::new_string(&new_value, 0, len JVM_CHECK_0);
	}
  }

  return *this;
}

void String::print_string_on(Stream* st, int max_len) {
  UsingFastOops fast_oops;
  TypeArray::Fast t = value();
  int index = offset();
  int end_index = index + count();
  int max_index = -1;
  if (max_len >= 0) {
    max_index = index + 100;
  }
  for (; index < end_index; index++) {
    jchar c =  t().char_at(index);
    const char *format = (c == '"')                ? "\\\""
                       : (c <= 0xFF && isprint(c)) ? "%c"
                       : (c == '\n')               ? "\\n"
                       : (c == '\t')               ? "\\t"
                       :                           "\\u%04x";
    st->print(format, c);
    if (index == max_index) {
      st->print("...");
      break;
    }
  }
}

jchar String::char_at(int index) {
  TypeArray::Raw ta = value();
  return ta().char_at(index + offset());
}

jint String::last_index_of(jchar ch, jint fromIndex) {
  if (count() == 0 || fromIndex < 0) {
    return -1;
  }

  if (fromIndex >= count()) {
    fromIndex = count() - 1;
  }
   
  TypeArray::Raw array = value();
  const jchar* const base = (jchar*)array().base_address() + offset();

  // Pointer to the start point of the search.
  const jchar* p = base + fromIndex;

  do {
    if (*p == ch) {
      return p - base;
    }
  } while (--p >= base);

  return -1;
}

#if !defined(PRODUCT) || ENABLE_TTY_TRACE
void String::print_value_on(Stream* st) {
  TypeArray::Raw t = value();
  if (t.is_null()) {
    st->print("(uninitialized)");
  } else {
    st->print("String \"");
    print_string_on(st, 100);
    st->print("\"");
  }
}


#endif
