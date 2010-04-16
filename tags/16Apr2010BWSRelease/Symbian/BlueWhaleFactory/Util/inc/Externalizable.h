/**
 * Copyright (c) 2004-2008 Blue Whale Systems Ltd. All Rights Reserved. 
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
 *  
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 
 * 2 only, as published by the Free Software Foundation.  
 *  
 * This software is provided "as is," and the copyright holder makes no representations or warranties, express or
 * implied, including but not limited to warranties of merchantability or fitness for any particular purpose or that the
 * use of this software or documentation will not infringe any third party patents, copyrights, trademarks or other
 * rights.
 * 
 * The copyright holder will not be liable for any direct, indirect special or consequential damages arising out of any
 * use of this software or documentation.
 * 
 * See the GNU  General Public License version 2 for more details 
 * (a copy is included at /legal/license.txt).  
 *  
 * You should have received a copy of the GNU General Public License 
 * version 2 along with this work; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 
 * 02110-1301 USA  
 *  
 * Please contact Blue Whale Systems Ltd, Suite 123, The LightBox
 * 111 Power Road, Chiswick, London, W4 5PY, United Kingdom or visit 
 * www.bluewhalesystems.com if you need additional 
 * information or have any questions.  
 */ 


#ifndef __EXTERNALIZABLE_H__
#define __EXTERNALIZABLE_H__

#include <s32strm.h>
#include <unknown.h>


const TInt KIID_MExternalizable = 0x01000010;
class MExternalizable : public MUnknown
{
public:
	/**
	 * In order to internalize an object from a stream,
	 * we must first be able to instantiate the correct instance
	 * of the object somehow.  To do this, when writing the
	 * object into the stream we should first write the return
	 * value from this method to the stream, which is the
	 * correct component id of the object.  This component
	 * id can be used when internalizing the object from the
	 * stream in order to re-create the object from the stream.
	 */
	virtual TUid GetECOMPlusComponentId() const = 0;
	virtual TUint8 GetVersion() const = 0;
	
	virtual void ExternalizeL(RWriteStream& aStream) const = 0;
	virtual void InternalizeL(RReadStream& aStream) = 0;

	/**
	 * When loading an item from e.g. a file, the last modification
	 * time is useful information which may be used to determine versioning.
	 */
	virtual const TTime & GetModified() const = 0;

	/**
	 * When loading an item from e.g. a file, the last modification
	 * time is useful information which may be used to determine versioning.
	 */
	virtual void SetModified(const TTime & aModified ) = 0;
	

	/**
	 * Has anything changed about this object since it was last persisted?
	 */
	virtual TBool Dirty() const = 0;

	/**
	 * Usually objects implementing this interface will take care of 
	 * setting themselves to dirty when they are changed.
	 *
	 * Sometimes clients of an object will need to manually set an object
	 * to be dirty if they change sub parts of an object which the object
	 * won't know have changed.  For example, if a client changes an
	 * object added to an MProperties object, that MProperties object
	 * won't know the sub-object has changed.
	 *
	 * Code which persists this object must take care of setting this object
	 * to be 'not dirty' after it has been successfully persisted.
	 *
	 */
	virtual void SetDirty(TBool aDirty = ETrue) = 0;


protected:
	virtual ~MExternalizable() {}	
};
#endif /* __EXTERNALIZABLE_H__ */
