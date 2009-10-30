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


#ifndef __MANAGEMENTOBJECTFACTORY_H__
#define __MANAGEMENTOBJECTFACTORY_H__

#include "Thread.h"
#include "Logger.h"
#include "application.h"

const TInt KCID_MVMObjectFactory = 0xA0003F57;
const TInt KIID_MVMObjectFactory = 0xA0003F58;
const TInt KIID_MVMObjectFactoryClient = 0xA0003F50;
const TInt KIID_MShortcutName = 0xA0003F52;
const TString8UniqueKey KPropertyString8ShortcutName		= { KIID_MShortcutName, 10101 };
const TInt KIID_MAutoStarted = 0xA0003F53;
const TIntUniqueKey KPropertyIntAutoStarted					= { KIID_MAutoStarted, 10101 };

class MVMObjectFactory : public MUnknown
{
public:
	virtual MThread* CreateVMThreadObject(const TDesC& aName) = 0;
	virtual MThread* CreateVMManagerThreadObject(const TDesC& aName) = 0;
	virtual MDebugApplication* CreateVMManagerObject() = 0;
	virtual void SetCanvas(MMIDPCanvas* aCanvas) = 0;
	virtual MEventQueue* EventQueue() = 0;
protected:
	virtual ~MVMObjectFactory(){}
};


class MVMObjectFactoryClient : public MUnknown
{
public:
	virtual void SetClient(MVMObjectFactory* aFactory) = 0;
protected:
	virtual ~MVMObjectFactoryClient(){}
};

class CVMObjectFactory : public CRefCountedBase, public MVMObjectFactory
{
public:
	static MUnknown* NewL(TAny* aContructionParameters);
protected:
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId);
	virtual void AddRef() { CRefCountedBase::AddRef();}
	virtual void Release(){ CRefCountedBase::Release();}
	
	virtual MThread* CreateVMThreadObject(const TDesC& aName);
	virtual MThread* CreateVMManagerThreadObject(const TDesC& aName);
	virtual MDebugApplication* CreateVMManagerObject();
	virtual void SetCanvas(MMIDPCanvas* aCanvas);
	virtual MEventQueue* EventQueue() { return iQueue;}
private:
	CVMObjectFactory(TAny* aContructionParameters);
	virtual ~CVMObjectFactory();
	void ConstructL();
private:
	MThread* iVMManager;
	MMIDPCanvas* iCanvas;
	MEventQueue* iQueue;
};


#endif /*__MANAGEMENTOBJECTFACTORY_H__*/
