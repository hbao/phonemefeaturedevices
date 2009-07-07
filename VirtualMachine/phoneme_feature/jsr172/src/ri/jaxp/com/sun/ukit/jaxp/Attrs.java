/*
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

package com.sun.ukit.jaxp;

import org.xml.sax.Attributes;

public class Attrs
	implements Attributes
{
	/**
	 * Element owner of attributes.
	 */
	private Pair mElm;

	/**
	 * Number of attributes in the attributes string array.
	 */
	private int mLength;

	/**
	 * Attributes string array. Each individual attribute is represented by 
	 * four strings: namespace URL(+0), qname(+1), local name(+2), value(+3),
	 * type(+4), declared["d"] and default["D"](+5). 
	 * In order to find attribute by the attribute index, the attribute 
	 * index MUST be multiplied by 8. The result will point to the attribute 
	 * namespace URL. 
	 */
	/* pkg */ String[] mItems;

	/**
	 * Constructor.
	 */
	/* pkg */ Attrs()
	{
		//		The default number of attributes capacity is 8.
		mItems = new String[(8 << 3)];
	}

	/**
	 * Sets new element owner. This method also invalidate <code>mItems</code> 
	 * array so any attempt to call public methods of this object will insure 
	 * correct length of the <code>mItems</code> array and fill out the array 
	 * with new attributes using <code>mElm.list</code>.
	 *
	 * @param elm The new element owner.
	 */
	/* pkg */ void set(Pair elm)
	{
		mElm    = elm;
		mLength = -1;
	}

	/**
	 * Converts attributes of mElm to the internal structure.
	 */
	private void update()
	{
		//		Ensure the capacity of the attribute string array.
		if (mElm.num > (mItems.length >> 3)) {
			mItems = new String[mElm.num << 3];
		}

		boolean withns = ((mElm.id & Parser.FLAG_NSAWARE) == 0)? false: true;
		int idx = mElm.num;
		for (Pair attr = mElm.list; attr != null; attr = attr.next) {
			if (attr.list != null && withns)  // NS declaration
				continue;  // do not add NS declarations
			int base = (idx - 1) << 3;
			idx--;  // next attribute index
			mItems[base]     = (attr.ns != null)? attr.ns: ""; // namespace
			mItems[base + 1] = attr.qname();                   // qName
			mItems[base + 2] = (withns)? attr.local(): "";     // local name
			mItems[base + 3] = attr.value;                     // value
			//		Attribute type
			switch (attr.id) {
			case 'i':
				mItems[base + 4] = "ID";
				break;
	
			case 'r':
				mItems[base + 4] = "IDREF";
				break;
	
			case 'R':
				mItems[base + 4] = "IDREFS";
				break;
	
			case 'n':
				mItems[base + 4] = "ENTITY";
				break;
	
			case 'N':
				mItems[base + 4] = "ENTITIES";
				break;
	
			case 't':
				mItems[base + 4] = "NMTOKEN";
				break;
	
			case 'T':
				mItems[base + 4] = "NMTOKENS";
				break;
	
			case 'u':
				mItems[base + 4] = "NMTOKEN";
				break;
	
			case 'o':
				mItems[base + 4] = "NOTATION";
				break;
	
			case 'c':
			default:
				mItems[base + 4] = "CDATA";
				break;
			}
			//		Declared and default value flags
			switch(attr.num & 0x3) {
			case 0x0:
				mItems[base + 5] = null;
				break;

			case 0x1:  // declared attribute
				mItems[base + 5] = "d";
				break;

			default:  // 0x2, 0x3 - default attribute always declared
				mItems[base + 5] = "D";
				break;
			}
		}
		mLength	= mElm.num;
	}

	/**
	 * Return the number of attributes in the list.
	 *
	 * <p>Once you know the number of attributes, you can iterate
	 * through the list.</p>
	 *
	 * @return The number of attributes in the list.
	 * @see #getURI(int)
	 * @see #getLocalName(int)
	 * @see #getQName(int)
	 * @see #getType(int)
	 * @see #getValue(int)
	 */
	public int getLength()
	{
		if (mLength < 0)
			update();

		return mLength;
	}

	/**
	 * Look up an attribute's Namespace URI by index.
	 *
	 * @param index The attribute index (zero-based).
	 * @return The Namespace URI, or the empty string if none
	 *	is available, or null if the index is out of
	 *	range.
	 * @see #getLength
	 */
	public String getURI(int index)
	{
		if (mLength < 0)
			update();

		return ((index >= 0) && (index < mLength))? 
			(mItems[index << 3]): 
			null;
	}

	/**
	 * Look up an attribute's local name by index.
	 *
	 * @param index The attribute index (zero-based).
	 * @return The local name, or the empty string if Namespace
	 *	processing is not being performed, or null
	 *	if the index is out of range.
	 * @see #getLength
	 */
	public String getLocalName(int index)
	{
		if (mLength < 0)
			update();

		return ((index >= 0) && (index < mLength))? 
			(mItems[(index << 3) + 2]):
			null;
	}

	/**
	 * Look up an attribute's XML 1.0 qualified name by index.
	 *
	 * @param index The attribute index (zero-based).
	 * @return The XML 1.0 qualified name, or the empty string
	 *	if none is available, or null if the index
	 *	is out of range.
	 * @see #getLength
	 */
	public String getQName(int index)
	{
		if (mLength < 0)
			update();

		if ((index < 0) || (index >= mLength))
			return null;
		return mItems[(index << 3) + 1];
	}

	/**
	 * Look up an attribute's type by index.
	 *
	 * <p>The attribute type is one of the strings "CDATA", "ID",
	 * "IDREF", "IDREFS", "NMTOKEN", "NMTOKENS", "ENTITY", "ENTITIES",
	 * or "NOTATION" (always in upper case).</p>
	 *
	 * <p>If the parser has not read a declaration for the attribute,
	 * or if the parser does not report attribute types, then it must
	 * return the value "CDATA" as stated in the XML 1.0 Recommendation
	 * (clause 3.3.3, "Attribute-Value Normalization").</p>
	 *
	 * <p>For an enumerated attribute that is not a notation, the
	 * parser will report the type as "NMTOKEN".</p>
	 *
	 * @param index The attribute index (zero-based).
	 * @return The attribute's type as a string, or null if the
	 *         index is out of range.
	 * @see #getLength
	 */
	public String getType(int index)
	{
		if (mLength < 0)
			update();

		return ((index >= 0) && (index < (mItems.length >> 3)))? 
			(mItems[(index << 3) + 4]): 
			null;
	}

	/**
	 * Look up an attribute's value by index.
	 *
	 * <p>If the attribute value is a list of tokens (IDREFS,
	 * ENTITIES, or NMTOKENS), the tokens will be concatenated
	 * into a single string with each token separated by a
	 * single space.</p>
	 *
	 * @param index The attribute index (zero-based).
	 * @return The attribute's value as a string, or null if the
	 *         index is out of range.
	 * @see #getLength
	 */
	public String getValue(int index)
	{
		if (mLength < 0)
			update();

		return ((index >= 0) && (index < mLength))? 
			(mItems[(index << 3) + 3]):
			null;
	}

	/**
	 * Look up the index of an attribute by Namespace name.
	 *
	 * @param uri The Namespace URI, or the empty string if
	 *	the name has no Namespace URI.
	 * @param localName The attribute's local name.
	 * @return The index of the attribute, or -1 if it does not
	 *	appear in the list.
	 */
	public int getIndex(String uri, String localName)
	{
		if (mLength < 0)
			update();

		int len = mLength;
		for (int idx = 0; idx < len; idx++) {
			if ((mItems[idx << 3]).equals(uri) &&
				mItems[(idx << 3) + 2].equals(localName))
				return idx;
		}
		return -1;
	}

	/**
	 * Look up the index of an attribute by Namespace name.
	 *
	 * @param uri The Namespace URI, or the empty string if
	 *	the name has no Namespace URI. <code>null</code> value enforce the 
	 *  search by the local name only.
	 * @param localName The attribute's local name.
	 * @return The index of the attribute, or -1 if it does not
	 *	appear in the list.
	 */
	/* pkg */ int getIndexNullNS(String uri, String localName)
	{
		if (mLength < 0)
			update();

		int len = mLength;
		if (uri != null) {
			for (int idx = 0; idx < len; idx++) {
				if ((mItems[idx << 3]).equals(uri) &&
					mItems[(idx << 3) + 2].equals(localName))
					return idx;
			}
		} else {
			for (int idx = 0; idx < len; idx++) {
				if (mItems[(idx << 3) + 2].equals(localName))
					return idx;
			}
		}
		return -1;
	}

	/**
	 * Look up the index of an attribute by XML 1.0 qualified name.
	 *
	 * @param qName The qualified (prefixed) name.
	 * @return The index of the attribute, or -1 if it does not
	 *	appear in the list.
	 */
	public int getIndex(String qName)
	{
		if (mLength < 0)
			update();

		int len = mLength;
		for (int idx = 0; idx < len; idx++) {
			if (mItems[(idx << 3) + 1].equals(qName))
				return idx;
		}
		return -1;
	}

	/**
	 * Look up an attribute's type by Namespace name.
	 *
	 * <p>See {@link #getType(int) getType(int)} for a description
	 * of the possible types.</p>
	 *
	 * @param uri The Namespace URI, or the empty String if the
	 *	name has no Namespace URI.
	 * @param localName The local name of the attribute.
	 * @return The attribute type as a string, or null if the
	 *	attribute is not in the list or if Namespace
	 *	processing is not being performed.
	 */
	public String getType(String uri, String localName)
	{
		if (mLength < 0)
			update();

		int idx = getIndex(uri, localName);
		return (idx >= 0)? (mItems[(idx << 3) + 4]): null;
	}

	/**
	 * Look up an attribute's type by XML 1.0 qualified name.
	 *
	 * <p>See {@link #getType(int) getType(int)} for a description
	 * of the possible types.</p>
	 *
	 * @param qName The XML 1.0 qualified name.
	 * @return The attribute type as a string, or null if the
	 *	attribute is not in the list or if qualified names
	 *	are not available.
	 */
	public String getType(String qName)
	{
		if (mLength < 0)
			update();

		int idx = getIndex(qName);
		return (idx >= 0)? (mItems[(idx << 3) + 4]): null;
	}

	/**
	 * Look up an attribute's value by Namespace name.
	 *
	 * <p>See {@link #getValue(int) getValue(int)} for a description
	 * of the possible values.</p>
	 *
	 * @param uri The Namespace URI, or the empty String if the
	 *	name has no Namespace URI.
	 * @param localName The local name of the attribute.
	 * @return The attribute value as a string, or null if the
	 *	attribute is not in the list.
	 */
	public String getValue(String uri, String localName)
	{
		if (mLength < 0)
			update();

		int idx = getIndex(uri, localName);
		return (idx >= 0)? (mItems[(idx << 3) + 3]): null;
	}

	/**
	 * Look up an attribute's value by XML 1.0 qualified name.
	 *
	 * <p>See {@link #getValue(int) getValue(int)} for a description
	 * of the possible values.</p>
	 *
	 * @param qName The XML 1.0 qualified name.
	 * @return The attribute value as a string, or null if the
	 *	attribute is not in the list or if qualified names
	 *	are not available.
	 */
	public String getValue(String qName)
	{
		if (mLength < 0)
			update();

		int idx = getIndex(qName);
		return (idx >= 0)? (mItems[(idx << 3) + 3]): null;
	}
}
