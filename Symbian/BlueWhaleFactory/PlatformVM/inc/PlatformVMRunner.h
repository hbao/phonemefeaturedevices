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


#ifndef __PLATFORMVMRUNNER_H__
#define __PLATFORMVMRUNNER_H__
#include "thread.h"
#include <os_symbian.hpp>

// See Symbian FAQ 1333
const TInt KUidValueManufacturer_SonyEricsson	= 0x101F6CED;
const TInt KUidValueManufacturer_ARM			= 0x101F4ED9;
const TInt KUidValueManufacturer_Samsung		= 0x101F7E7B;
const TInt KUidValueManufacturer_Siemens		= 0x101F9072;
const TInt KUidValueManufacturer_Sendo			= 0x101FA032;
const TInt KUidValueManufacturer_BENQ			= 0x101FD278;
const TInt KUidValueManufacturer_LG				= 0x10200A8C;
const TInt KUidValueManufacturer_Lenovo			= 0x1020E440;
const TInt KUidValueManufacturer_NEC			= 0x102740C4;

_LIT8(KManufacturerEricsson,			"Ericsson");
_LIT8(KManufacturerMotorola,			"Motorola");
_LIT8(KManufacturerNokia,				"Nokia");
_LIT8(KManufacturerPanasonic,			"Panasonic");
_LIT8(KManufacturerPsion,				"Psion");
_LIT8(KManufacturerIntel,				"Intel");
_LIT8(KManufacturerCogent,				"Cogent");
_LIT8(KManufacturerCirrus,				"Cirrus");
_LIT8(KManufacturerLinkup,				"Linkup");
_LIT8(KManufacturerTexasInstruments,	"TexasInstruments");
_LIT8(KManufacturerSonyEricsson,		"SonyEricsson");
_LIT8(KManufacturerARM,					"ARM");
_LIT8(KManufacturerSamsung,				"Samsung");
_LIT8(KManufacturerSiemens,				"Siemens");
_LIT8(KManufacturerSendo,				"Sendo");
_LIT8(KManufacturerBENQ,				"BENQ");
_LIT8(KManufacturerLG,					"LG");
_LIT8(KManufacturerLenovo,				"Lenovo");
_LIT8(KManufacturerNEC,					"NEC");
_LIT8(KManufacturerUnknown,				"Unknown");

class CMyUndertaker;

class TThreadInfo
{
public:
	TThreadInfo(MThread* aThread,CMyUndertaker* aUndertaker,TThreadId aId): iThread(aThread),iUndertaker(aUndertaker),iId(aId){}
public:
	MThread* iThread;
	CMyUndertaker* iUndertaker;
	TThreadId iId;
};


class CJVMRunner : public CActive, public MRunnable
{
	public:
		CJVMRunner(MApplication* aApplication);
		~CJVMRunner();
		void ConstructL();
		void StartL(RThread& aThread);
		void StopL();
		void SetThread(MThread* aThread){iThread = aThread;}
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
		
	protected:
		TFileName VMInstallFileName();
		TBuf8<128> QuitReasonText();
		TBuf8<32> ManufacturerName(TInt aManufacturer);

	protected:
		TInt RunVML();
		MApplication* iApplication;
		MThread* iThread;
};


#endif /* __PLATFORMVMRUNNER_H__ */
