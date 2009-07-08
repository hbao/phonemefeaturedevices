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

import java.util.Hashtable;
import java.io.InputStream;
import java.io.IOException;

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.Locator;
import org.xml.sax.InputSource;
import org.xml.sax.Attributes;
import org.xml.sax.SAXParseException;
import org.xml.sax.SAXException;
import org.xml.sax.SAXNotSupportedException;
import org.xml.sax.SAXNotRecognizedException;

/**
 * XML non-validating push parser.
 *
 * This non-validating parser conforms to <a href="http://www.w3.org/TR/REC-xml"
 * >Extensible Markup Language (XML) 1.0</a> and <a href="http://www.w3.org/TR/REC-xml-names"
 * >"Namespaces in XML"</a> specifications. 
 * The API supported by the parser are <a 
 * href="http://java.sun.com/aboutJava/communityprocess/final/jsr030/index.html">CLDC 1.0</a> 
 * and <a href="http://www.jcp.org/en/jsr/detail?id=280">JSR-280</a>, a JavaME subset of 
 * <a href="http://java.sun.com/xml/jaxp/index.html">JAXP</a> and <a 
 * href="http://www.saxproject.org/">SAX2</a>.
 */

/* pkg */ final class ParserSAX
	extends Parser
	implements Locator
{
	public final static String FEATURE_NS   = 
		"http://xml.org/sax/features/namespaces";
	public final static String FEATURE_PREF = 
		"http://xml.org/sax/features/namespace-prefixes";

	//		SAX feature flags
	private boolean mFNamespaces;
	private boolean mFPrefixes;

	//		SAX handlers
	/* pkg */ DefaultHandler mHand;  // the default handler

	/**
	 * Constructor.
	 */
	/* pkg */ ParserSAX()
	{
		super();

		//		SAX feature default values
		mFNamespaces = true;
		mFPrefixes   = false;

		//		Default handler which will be used in case the application
		//		do not set one of handlers.
		mHand = new DefaultHandler();
	}

	/**
	 * Look up the value of a feature.
	 *
	 * @param name The feature name, which is a fully-qualified URI.
	 * @return The current state of the feature (true or false).
	 * @exception org.xml.sax.SAXNotRecognizedException When the
	 *            parser does not recognize the feature name.
	 * @exception org.xml.sax.SAXNotSupportedException When the
	 *            parser recognizes the feature name but 
	 *            cannot determine its value at this time.
	 * @see #setFeature
	 */
	/* pkg */ boolean getFeature(String name)
		throws SAXNotRecognizedException, SAXNotSupportedException
	{
		if (name.equals(FEATURE_NS) == true) {
			return mFNamespaces;
		} else if (FEATURE_PREF.equals(name) == true) {
			return mFPrefixes;
		} else {
			throw new SAXNotRecognizedException(name);
		}
	}

	/**
	 * Set the state of a feature.
	 *
	 * @param name The feature name, which is a fully-qualified URI.
	 * @param value The requested value of the feature (true or false).
	 * @exception org.xml.sax.SAXNotRecognizedException When the
	 *            parser does not recognize the feature name.
	 * @exception org.xml.sax.SAXNotSupportedException When the
	 *            parser recognizes the feature name but 
	 *            cannot set the requested value.
	 * @see #getFeature
	 */
	/* pkg */ void setFeature(String name, boolean value) 
		throws SAXNotRecognizedException, SAXNotSupportedException
	{
		if (name.equals(FEATURE_NS) == true) {
			//		Cannot be changed during parsing
			if (mPh >= PH_DOC_START)
				throw new SAXNotSupportedException(name);
			mFNamespaces = value;
		} else if (FEATURE_PREF.equals(name) == true) {
			//		Cannot be changed during parsing
			if (mPh >= PH_DOC_START)
				throw new SAXNotSupportedException(name);
			mFPrefixes   = value;
		} else {
			throw new SAXNotRecognizedException(name);
		}
	}

	/**
	 * Return the public identifier for the current document event.
	 *
	 * <p>The return value is the public identifier of the document
	 * entity or of the external parsed entity in which the markup
	 * triggering the event appears.</p>
	 *
	 * @return A string containing the public identifier, or
	 *	null if none is available.
	 *
	 * @see #getSystemId
	 */
	public String getPublicId()
	{
		return (mInp != null)? mInp.pubid: null;
	}

	/**
	 * Return the system identifier for the current document event.
	 *
	 * <p>The return value is the system identifier of the document
	 * entity or of the external parsed entity in which the markup
	 * triggering the event appears.</p>
	 *
	 * <p>If the system identifier is a URL, the parser must resolve it
	 * fully before passing it to the application.</p>
	 *
	 * @return A string containing the system identifier, or null
	 *	if none is available.
	 *
	 * @see #getPublicId
	 */
	public String getSystemId()
	{
		return (mInp != null)? mInp.sysid: null;
	}

	/**
	 * Return the line number where the current document event ends.
	 *
	 * @return Always returns -1 indicating the line number is not 
	 *	available.
	 *
	 * @see #getColumnNumber
	 */
	public int getLineNumber()
	{
		return -1;
	}

	/**
	 * Return the column number where the current document event ends.
	 *
	 * @return Always returns -1 indicating the column number is not 
	 *	available.
	 *
	 * @see #getLineNumber
	 */
	public int getColumnNumber()
	{
		return -1;
	}

	/**
	 * Parse an XML document from a system identifier (URI).
	 *
	 * <p>This method is a shortcut for the common case of reading a
	 * document from a system identifier.  It is the exact
	 * equivalent of the following:</p>
	 *
	 * <pre>
	 * parse(new InputSource(systemId));
	 * </pre>
	 *
	 * <p>If the system identifier is a URL, it must be fully resolved
	 * by the application before it is passed to the parser.</p>
	 *
	 * @param systemId The system identifier (URI).
	 * @exception org.xml.sax.SAXException Any SAX exception, possibly
	 *            wrapping another exception.
	 * @exception java.io.IOException An IO exception from the parser,
	 *            possibly from a byte stream or character stream
	 *            supplied by the application.
	 * @see #parse(org.xml.sax.InputSource)
	 */
	public void parse(String systemId)
		throws IOException, SAXException
	{
		if (systemId == null)
			throw new NullPointerException("");

		parse(new InputSource(systemId));
	}

	/**
	 * Parse an XML document.
	 *
	 * <p>The application can use this method to instruct the XML
	 * parser to begin parsing an XML document from any valid input
	 * source (a character stream, a byte stream, or a URI).</p>
	 *
	 * <p>Applications may not invoke this method while a parse is in
	 * progress (they should create a new SAXParser instead for each
	 * nested XML document).  Once a parse is complete, an
	 * application may reuse the same SAXParser object, possibly with a
	 * different input source.</p>
	 *
	 * <p>During the parse, the parser will provide information
	 * about the XML document through the registered event
	 * handlers.</p>
	 *
	 * <p>This method is synchronous: it will not return until parsing
	 * has ended.  If a client application wants to terminate 
	 * parsing early, it should throw an exception.</p>
	 *
	 * @param is The input source for the top-level of the
	 *        XML document.
	 * @exception org.xml.sax.SAXException Any SAX exception, possibly
	 *            wrapping another exception.
	 * @exception java.io.IOException An IO exception from the parser,
	 *            possibly from a byte stream or character stream
	 *            supplied by the application.
	 * @see org.xml.sax.InputSource
	 * @see #parse(java.lang.String)
	 */
	public void parse(InputSource is)
		throws IOException, SAXException
	{
		if (is == null)
			throw new NullPointerException("");
		//		Set up the document
		mInp = new Input(BUFFSIZE_READER);
		mPh  = PH_BEFORE_DOC;  // before parsing
		try {
			setinp(is);
		} catch(SAXException saxe) {
			throw saxe;
		} catch(IOException ioe) {
			throw ioe;
		} catch(RuntimeException rte) {
			throw rte;
		} catch(Exception e) {
			panic(e.toString());
		}
		parse();
	}

	/**
	 * Parse the XML document content using specified handlers and an 
	 * input source.
	 *
	 * @exception IOException If any IO errors occur.
	 * @exception SAXException If the underlying parser throws a
	 * SAXException while parsing.
	 */
	private void parse()
		throws SAXException, IOException
	{
		if ((mFNamespaces == true) && (mFPrefixes == false))
			mIsNSAware = true;
		else if ((mFNamespaces == false) && (mFPrefixes == true))
			mIsNSAware = false;
		else
			throw new SAXNotSupportedException(FEATURE_NS + FEATURE_PREF);

		init();
		try {
			mHand.setDocumentLocator(this);
			mHand.startDocument();

			if (mPh != PH_MISC_DTD)
				mPh = PH_MISC_DTD;  // misc before DTD
			int evt = EV_NULL;
			//		XML document prolog
			do {
				switch (evt = step()) {
				case EV_ELM:
				case EV_ELMS:
					mPh = PH_DOCELM;
					break;

				case EV_COMM:
				case EV_PI:
				case EV_PENT:
				case EV_UENT:
				case EV_NOT:
					break;

				case EV_DTDS:
					if (mPh >= PH_DTD)
						panic(FAULT);
					mPh = PH_DTD;  // DTD
					break;

				case EV_DTDE:
					if (mPh != PH_DTD)
						panic(FAULT);
					mPh = PH_DTD_MISC;  // misc after DTD
					break;

				default:
					panic(FAULT);
				}
			} while (mPh < PH_DOCELM);
			//		XML document starting with document's element
			do {
				switch (evt) {
				case EV_ELM:
				case EV_ELMS:
					//		Report the element
					if (mIsNSAware == true) {
						mHand.startElement(
							(mElm.ns != null)? mElm.ns: "",
							mElm.name,
							"",
							mAttrs);
					} else {
						mHand.startElement(
							"",
							"",
							mElm.name,
							mAttrs);
					}
					if (evt == EV_ELMS) {
						evt = step();
						break;
					}

				case EV_ELME:
					//		Report the end of element
					if (mIsNSAware == true)
						mHand.endElement(
							(mElm.ns != null)? mElm.ns: "", mElm.name, "");
					else
						mHand.endElement("", "", mElm.name);
					//		Restore the top of the prefix stack
					while (mPref.list == mElm) {
						mHand.endPrefixMapping(mPref.name);
						mPref = del(mPref);
					}
					//		Remove the top element tag
					mElm = del(mElm);
					if (mElm == null)
						mPh = PH_DOCELM_MISC;
					else
						evt = step();
					break;

				case EV_TEXT:
				case EV_WSPC:
				case EV_CDAT:
				case EV_COMM:
				case EV_PI:
				case EV_ENT:
					evt = step();
					break;

				default:
					panic(FAULT);
				}
			} while (mPh == PH_DOCELM);
			//		Misc after document's element
			do {
				if (wsskip() == EOS)
					break;

				switch (step()) {
				case EV_COMM:
				case EV_PI:
					break;

				default:
					panic(FAULT);
				}
			} while (mPh == PH_DOCELM_MISC);
			mPh = PH_AFTER_DOC;  // parsing is completed

		} catch(SAXException saxe) {
			throw saxe;
		} catch(IOException ioe) {
			throw ioe;
		} catch(RuntimeException rte) {
			throw rte;
		} catch(Exception e) {
			panic(e.toString());
		} finally {
			mHand.endDocument();
			cleanup();
		}
	}

	/**
	 * Reports document type.
	 *
	 * @param name The name of the entity.
	 * @param pubid The public identifier of the DTD or <code>null</code>.
	 * @param sysid The system identifier of the DTD or <code>null</code>.
	 * @param dtdint The DTD internal subset or <code>null</code>.
	 */
	protected void docType(
		String name, String pubid, String sysid, char[] dtdint)
	{
	}

	/**
	 * Reports a comment.
	 *
	 * @param text The comment text starting from first character.
	 * @param length The number of characters in comment.
	 */
	protected void comm(char[] text, int length)
	{
	}

	/**
	 * Reports a processing instruction.
	 *
	 * @param target The processing instruction target name.
	 * @param body The processing instruction body text.
	 */
	protected void pi(String target, String body)
		throws SAXException
	{
		mHand.processingInstruction(target, body);
	}

	/**
	 * Reports new namespace prefix. 
	 * The Namespace prefix (<code>mPref.name</code>) being declared and 
	 * the Namespace URI (<code>mPref.value</code>) the prefix is mapped 
	 * to. An empty string is used for the default element namespace, 
	 * which has no prefix.
	 */
	protected void newPrefix()
		throws SAXException
	{
		mHand.startPrefixMapping(mPref.name, mPref.value);
	}

	/**
	 * Reports skipped entity name.
	 *
	 * @param name The entity name.
	 */
	protected void skippedEnt(String name)
		throws SAXException
	{
		mHand.skippedEntity(name);
	}

	/**
	 * Returns an <code>InputSource</code> for specified entity or 
	 * <code>null</code>.
	 *
	 * @param name The name of the entity.
	 * @param pubid The public identifier of the entity.
	 * @param sysid The system identifier of the entity.
	 */
	protected InputSource resolveEnt(String name, String pubid, String sysid)
		throws SAXException, IOException
	{
		return mHand.resolveEntity(pubid, sysid);
	}

	/**
	 * Reports internal parsed entity.
	 *
	 * @param name The entity name.
	 * @param value The entity replacement text.
	 */
	protected void intparsedEntDecl(String name, char[] value)
		throws Exception
	{
	}

	/**
	 * Reports external parsed entity.
	 *
	 * @param name The entity name.
	 * @param pubid The entity public identifier, may be null.
	 * @param name The entity system identifier, may be null.
	 */
	protected void extparsedEntDecl(String name, String pubid, String sysid)
		throws Exception
	{
	}

	/**
	 * Reports notation declaration.
	 *
	 * @param name The notation's name.
	 * @param pubid The notation's public identifier, or null if none was given.
	 * @param sysid The notation's system identifier, or null if none was given.
	 */
	protected void notDecl(String name, String pubid, String sysid)
		throws SAXException
	{
		mHand.notationDecl(name, pubid, sysid);
	}

	/**
	 * Reports unparsed entity name.
	 *
	 * @param name The unparsed entity's name.
	 * @param pubid The entity's public identifier, or null if none was given.
	 * @param sysid The entity's system identifier.
	 * @param notation The name of the associated notation.
	 */
	protected void unparsedEntDecl(
			String name, String pubid, String sysid, String notation)
		throws SAXException
	{
		mHand.unparsedEntityDecl(name, pubid, sysid, notation);
	}

	/**
	 * Notifies the handler about fatal parsing error.
	 *
	 * @param msg The problem description message.
	 */
	protected void panic(String msg)
		throws SAXException
	{
		SAXParseException spe = new SAXParseException(msg, this);
		mHand.fatalError(spe);
		throw spe;  // [#1.2] fatal error definition
	}

	/**
	 * Reports characters and empties the parser's buffer.
	 * This method is called only if parser is going to return control to 
	 * the main loop. This means that this method may use parser buffer 
	 * to report white space without copying characters to temporary 
	 * buffer.
	 */
	protected void bflash()
		throws SAXException
	{
		if (mBuffIdx >= 0) {
			//		Textual data has been read
			mHand.characters(mBuff, 0, (mBuffIdx + 1));
			mBuffIdx = -1;
		}
	}

	/**
	 * Reports white space characters and empties the parser's buffer.
	 * This method is called only if parser is going to return control to 
	 * the main loop. This means that this method may use parser buffer 
	 * to report white space without copying characters to temporary 
	 * buffer.
	 */
	protected void bflash_ws()
		throws SAXException
	{
		if (mBuffIdx >= 0) {
			if ((mElm.id & FLAG_XMLSPC_PRESERVE) == 0) {
				mHand.ignorableWhitespace(mBuff, 0, (mBuffIdx + 1));
			} else {
				//		Textual data has been read
				mHand.characters(mBuff, 0, (mBuffIdx + 1));
			}
			mBuffIdx = -1;
		}
	}
}
