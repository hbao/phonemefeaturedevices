/*
 * Portions Copyright  2000-2008 Sun Microsystems, Inc. All Rights
 * Reserved.  Use is subject to license terms.
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

/*
 * Copyright (c) 2000 World Wide Web Consortium,
 * (Massachusetts Institute of Technology, Institut National de
 * Recherche en Informatique et en Automatique, Keio University). All
 * Rights Reserved. This program is distributed under the W3C's Software
 * Intellectual Property License. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 * See W3C License http://www.w3.org/Consortium/Legal/ for more details.
 */

package org.w3c.dom;

/**
 * <code>EntityReference</code> objects may be inserted into the structure 
 * model when an entity reference is in the source document, or when the 
 * user wishes to insert an entity reference. Note that character references 
 * and references to predefined entities are considered to be expanded by 
 * the HTML or XML processor so that characters are represented by their 
 * Unicode equivalent rather than by an entity reference. Moreover, the XML 
 * processor may completely expand references to entities while building the 
 * structure model, instead of providing <code>EntityReference</code> 
 * objects. If it does provide such objects, then for a given 
 * <code>EntityReference</code> node, it may be that there is no 
 * <code>Entity</code> node representing the referenced entity. If such an 
 * <code>Entity</code> exists, then the subtree of the 
 * <code>EntityReference</code> node is in general a copy of the 
 * <code>Entity</code> node subtree. However, this may not be true when an 
 * entity contains an unbound namespace prefix. In such a case, because the 
 * namespace prefix resolution depends on where the entity reference is, the 
 * descendants of the <code>EntityReference</code> node may be bound to 
 * different namespace URIs.
 * <p>As for <code>Entity</code> nodes, <code>EntityReference</code> nodes and 
 * all their descendants are readonly.
 * <p>See also the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>Document Object Model (DOM) Level 2 Core Specification</a>.
 */
public interface EntityReference extends Node {
}
