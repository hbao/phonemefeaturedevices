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


#ifndef BLUEWHALESISREADER_H
#define BLUEWHALESISREADER_H

#include <e32base.h>
#include <w32std.h>
#include <e32property.h>
#include "Application.h"

class CDirPlus : public CDir
{
public:
	void AddL(const TEntry &anEntry) { CDir::AddL(anEntry); };
};

class CBlueWhaleSisReader : public CBase
{
public:
	static CBlueWhaleSisReader* NewL();
	static void ReadSisL();

	virtual ~CBlueWhaleSisReader();
protected:
	CBlueWhaleSisReader();
	void ConstructL();
	void FindAndPublishSisFileNameL();
	void FindFileInSubDirsL(const TFileName& aDir);
	void PersistFileNameL(const TFileName& aSisFileName);
private:
	RFs iFs;
	TFileName iPersistSisFileName;
	CDir* iDir;
};

#endif
