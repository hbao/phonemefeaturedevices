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


#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include "Properties.h"


const TInt KIID_MInitialize = 0x100001D;


/**
 * Since ConstructL (the conventional Symbian 2nd stage constructor) is
 * already called by the time the object is created using ECOM,
 * in order to initialize the object with any needed start-up information,
 * we introduce here a 3rd stage constructor.  
 *
 * Call this method once you have created the component using NewL or ECOMPLUS
 * in order to set it up with all the information it will need to initialize
 * itself.
 *
 * The MProperties interface points to a property bag which contains
 * information, pointers to data and possibly callback interfaces
 * which can be used by the component.
 *
 * Discussion:  While passing this MProperties interface to set
 * properties on a component is very generic, it is also dangerous
 * in that it can lead down the path towards global variables.
 * However, we have chosen this route in an attempt to find common
 * interfaces and reduce the overall apparent complexity of the system
 * by minimizing the number of different interfaces on different kinds
 * of state machines.
 *
 * To make a predictable and extensible system, it will be important
 * for components to document which keys they expect to find in
 * the MProperties object, and how they will change those properties.
 *
 * TODO: Consider working things out with the initialization 
 * parameter passed to CreateImplementationL.  Can do this
 * in the future using this interface.
 * 
 */
class MInitialize : public MUnknown
{
public:
	virtual void InitializeL(MProperties * aInitializationProperties) = 0;

protected:
	virtual ~MInitialize() {}
};

#endif /* __INITIALIZE_H__ */
