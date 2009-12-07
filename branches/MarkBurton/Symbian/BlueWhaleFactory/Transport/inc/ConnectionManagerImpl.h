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

#ifndef __CONNECTIONMANAGERIMPL_H__
#define __CONNECTIONMANAGERIMPL_H__

#include <CommDbConnPref.h>
#include "EcomPlusRefcountedActive.h"
#include "ConnectionManager.h"

class CConnectionManager : public CEComPlusRefCountedActive, public MConnectionManager
{
public:
	static MConnectionManager* NewL( TAny * aConstructionParameters );
private:
	CConnectionManager(TAny * aConstructionParameters );
	virtual ~CConnectionManager();
private: // MConnectionManager
	virtual void RaiseConnectionL(RSocketServ& aSocketServer);
	virtual void Start(TRequestStatus& aStatus);
	virtual RConnection& Connection();
	virtual void Close();
private: // MInitialize
	virtual void InitializeL(MProperties * aInitializationProperties);
private: // MUnknown
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	virtual void AddRef();
	virtual void Release();

	
private: // CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	MProperties*	iProperties;
	RConnection		iConnection;
	TCommDbConnPref	iCommDbConnPref; 
	TRequestStatus* iClientStatus;
	TBool iStarted;

};
#endif // __CONNECTIONMANAGERIMPL_H__ 
