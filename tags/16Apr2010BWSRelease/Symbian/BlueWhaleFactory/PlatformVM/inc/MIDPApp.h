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


#ifndef __MIDPAPP_H__
#define __MIDPAPP_H__

#include <e32std.h>
#include <eikbtgpc.h>
#include <OS_Symbian.hpp>

class MMIDPCanvas
{
public:
	
	virtual TInt GetDisplayWidth()= 0;
	virtual TInt GetDisplayHeight()= 0;
	virtual TUint32* CreateDisplayBitmap(TInt aWidth,TInt aHeight)= 0;
	virtual void Refresh(const TRect& aRect)=0;
	virtual void SetFullScreenMode(TBool aFullscreen) = 0;
	virtual TBool FullScreenMode() = 0;
	virtual void RequestOrdinalChange(TBool aForeground) = 0;
	virtual CFbsBitmap* Bitmap() = 0;
	virtual void BeginEditSession(TInt aId, TInt aConstraints) = 0;
	virtual void EndEditSession() = 0;
	virtual TInt GetNextState() = 0;
	virtual HBufC* GetInlineText(TInt aId)= 0;
	virtual TInt SetTextFieldString(TInt aId, HBufC* aText, TInt aParentId, TInt aMaxSize) = 0;
	virtual void SetCursorPosition(TInt aPosition) = 0;
	virtual void GetEditData(TInt& aEditPos, TInt& aEditLength) = 0;
	virtual TBool GetUnderline() = 0;
	virtual void ShowDisplayableNotify(TInt aDisplayableId) = 0;
	virtual void SetMenus(TMidpCommand *aCommands, int aLength) = 0;
	virtual RArray<TMidpCommand>& LockCommands() = 0;
	virtual void UnlockCommands() = 0;
	virtual void SetCba(CEikButtonGroupContainer* aCba) = 0;
	virtual TInt BackCommand() = 0;
	virtual TInt OkCommand() = 0;
	virtual void HandleCommandL(TInt aCommand) = 0;
protected:
	virtual ~MMIDPCanvas(){}
};



#endif /*__MIDPAPP_H__*/
