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


#include <e32base.h>
#include <W32STD.H>
#include <hal.h>
#include <keymap_input.h>
#include <OSVersion.h>
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <DeviceKeys.h>
#endif
#include "KeyMapperFactory.h"

#define MAPDEVICETOMIDPKEY(__AA,__BB) if(aKeyEvent.iScanCode == __AA) {return __BB;}

#define MAPDEVICETOMIDPKEY4(__AA,__BB,__CC,__DD,__EE) \
	if(aKeyEvent.iScanCode == __AA && (aKeyEvent.iModifiers & EModifierShift) && (aKeyEvent.iModifiers & EModifierFunc)) \
		{return __BB;} \
	else if(aKeyEvent.iScanCode == __AA && aKeyEvent.iModifiers & EModifierShift)	\
		{return __CC;} \
	else if(aKeyEvent.iScanCode == __AA && aKeyEvent.iModifiers & EModifierFunc)	\
		{return __DD;} \
	else if(aKeyEvent.iScanCode == __AA) \
		{return __EE;} \
	else if (aKeyEvent.iScanCode == 0 && aKeyEvent.iCode == __AA) \
		{return __EE;}
	
class CQwertyKeyboard : public CBase, public MKeyMapper
{
	public:
		virtual TChar CharFromScanCode(const TKeyEvent& aKeyEvent)
		{
			TChar ret = 0;
			MAPDEVICETOMIDPKEY4('Q','Q','Q','!','q')
			MAPDEVICETOMIDPKEY4('W','W','W','\"','w')
			MAPDEVICETOMIDPKEY4('E','E','E','&','e')
			MAPDEVICETOMIDPKEY4('A','A','A','£','a')
			MAPDEVICETOMIDPKEY4('S','S','S','$','s')
			MAPDEVICETOMIDPKEY4('D','D','D','€','d')
			MAPDEVICETOMIDPKEY4('Z','Z','Z','z','z')
			MAPDEVICETOMIDPKEY4('X','X','X','z','x')
			MAPDEVICETOMIDPKEY4('C','C','C','z','c')

			MAPDEVICETOMIDPKEY4('1','1','R','1','r')
			MAPDEVICETOMIDPKEY4('2','2','T','2','t')
			MAPDEVICETOMIDPKEY4('3','3','Y','3','y')
			MAPDEVICETOMIDPKEY4('*','*','U','*','u')
			MAPDEVICETOMIDPKEY4('4','4','F','4','f')
			MAPDEVICETOMIDPKEY4('5','5','G','5','g')
			MAPDEVICETOMIDPKEY4('6','6','H','6','h')
			MAPDEVICETOMIDPKEY4(EStdKeyHash,'#','J','#','j')
			MAPDEVICETOMIDPKEY4('7','7','V','7','v')
			MAPDEVICETOMIDPKEY4('8','8','B','8','b')
			MAPDEVICETOMIDPKEY4('9','9','N','9','n')
			MAPDEVICETOMIDPKEY4('0','0','M','0','m')

			MAPDEVICETOMIDPKEY4('I','I','I','-','i')
			MAPDEVICETOMIDPKEY4('O','O','O','+','o')
			MAPDEVICETOMIDPKEY4('P','P','P','=','p')
			MAPDEVICETOMIDPKEY4('K','K','K','_','k')
			MAPDEVICETOMIDPKEY4('L','L','L','\\','l')
			MAPDEVICETOMIDPKEY4(EStdKeyBackspace,KEYMAP_KEY_CLEAR,KEYMAP_KEY_CLEAR,KEYMAP_KEY_CLEAR,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY4(EStdKeySemiColon,',',';',',',',')
			MAPDEVICETOMIDPKEY4(EStdKeySingleQuote,'.',':','.','.')
			MAPDEVICETOMIDPKEY4(EStdKeyEnter,'\n','\n','\n','\n')
			
			MAPDEVICETOMIDPKEY4(EStdKeyComma,'(','(','/','/')
			MAPDEVICETOMIDPKEY4(EStdKeyFullStop,')',')','@','@')
			MAPDEVICETOMIDPKEY4('.','.',':','.','.')
			
			MAPDEVICETOMIDPKEY(EStdKeyDevice0,KEYMAP_KEY_SOFT1)
			MAPDEVICETOMIDPKEY(EStdKeyDevice1,KEYMAP_KEY_SOFT2)
			MAPDEVICETOMIDPKEY(EStdKeyDevice3,KEYMAP_KEY_SELECT)
			MAPDEVICETOMIDPKEY(EStdKeyUpArrow,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EStdKeyDownArrow,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EStdKeyLeftArrow,KEYMAP_KEY_LEFT)
			MAPDEVICETOMIDPKEY(EStdKeyRightArrow,KEYMAP_KEY_RIGHT)
			MAPDEVICETOMIDPKEY(EStdKeyBackspace,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY(EStdKeySpace,KEYMAP_KEY_SPACE)

			return ret;
		}
		virtual void Release() { delete this;}
	protected:
		virtual ~CQwertyKeyboard() {}
};


class CStandardKeyboard : public CBase, public MKeyMapper
{
	public:
		virtual TChar CharFromScanCode(const TKeyEvent& aKeyEvent)
		{
			TChar ret = 0;
			if(aKeyEvent.iScanCode >= 'A' && aKeyEvent.iScanCode <= 'Z')
			{
				if(aKeyEvent.iModifiers & EModifierShift)
				{
					ret = aKeyEvent.iScanCode & ~32;
				}
				else
				{
					ret = aKeyEvent.iScanCode | 32;
				}
				return ret;
			}
#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
			MAPDEVICETOMIDPKEY(EStdKeyDevice0,KEYMAP_KEY_SOFT1)
			MAPDEVICETOMIDPKEY(EStdKeyDevice1,KEYMAP_KEY_SOFT2)
			MAPDEVICETOMIDPKEY(EStdKeyDevice3,KEYMAP_KEY_SELECT)
			MAPDEVICETOMIDPKEY(EStdKeyUpArrow,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EStdKeyDownArrow,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EStdKeyLeftArrow,KEYMAP_KEY_LEFT)
			MAPDEVICETOMIDPKEY(EStdKeyRightArrow,KEYMAP_KEY_RIGHT)
			MAPDEVICETOMIDPKEY(EStdKeyBackspace,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY(EStdKeySpace,KEYMAP_KEY_SPACE)
			MAPDEVICETOMIDPKEY(EStdKeyNkpAsterisk,KEYMAP_KEY_ASTERISK)
			MAPDEVICETOMIDPKEY(EStdKeyHash,KEYMAP_KEY_POUND)
			MAPDEVICETOMIDPKEY(EKeyUpArrow,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EKeyDownArrow,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EKeyLeftArrow,KEYMAP_KEY_LEFT)
			MAPDEVICETOMIDPKEY(EKeyRightArrow,KEYMAP_KEY_RIGHT)
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
			MAPDEVICETOMIDPKEY(EStdKeyF13,KEYMAP_KEY_SOFT1);		// left softkey on Z10
			MAPDEVICETOMIDPKEY(EStdKeyF14,KEYMAP_KEY_SOFT2);		// right softkey on Z10
			MAPDEVICETOMIDPKEY(EDeviceKeyClear,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY(EStdDeviceKeyClear,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY(EStdKeyBackspace,KEYMAP_KEY_CLEAR)
			MAPDEVICETOMIDPKEY(EStdKeyDevice1,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EStdKeyDevice2,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EStdKeyDevice4,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EStdKeyDevice5,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EStdKeyDevice6,KEYMAP_KEY_LEFT)
			MAPDEVICETOMIDPKEY(EStdKeyDevice7,KEYMAP_KEY_RIGHT)
			MAPDEVICETOMIDPKEY(EStdKeyDevice8,KEYMAP_KEY_SELECT)
			MAPDEVICETOMIDPKEY(EStdKeyDevice1B,KEYMAP_KEY_SELECT)	// thumbwheel press on P1i
			MAPDEVICETOMIDPKEY(EStdKeyUpArrow,KEYMAP_KEY_UP)
			MAPDEVICETOMIDPKEY(EStdKeyDownArrow,KEYMAP_KEY_DOWN)
			MAPDEVICETOMIDPKEY(EStdKeyLeftArrow,KEYMAP_KEY_LEFT)
			MAPDEVICETOMIDPKEY(EStdKeyRightArrow,KEYMAP_KEY_RIGHT)
			MAPDEVICETOMIDPKEY(EStdKeyNkp1,KEYMAP_KEY_1)
			MAPDEVICETOMIDPKEY(EStdKeyNkp2,KEYMAP_KEY_2)
			MAPDEVICETOMIDPKEY(EStdKeyNkp3,KEYMAP_KEY_3)
			MAPDEVICETOMIDPKEY(EStdKeyNkp4,KEYMAP_KEY_4)
			MAPDEVICETOMIDPKEY(EStdKeyNkp5,KEYMAP_KEY_5)
			MAPDEVICETOMIDPKEY(EStdKeyNkp6,KEYMAP_KEY_6)
			MAPDEVICETOMIDPKEY(EStdKeyNkp7,KEYMAP_KEY_7)
			MAPDEVICETOMIDPKEY(EStdKeyNkp8,KEYMAP_KEY_8)
			MAPDEVICETOMIDPKEY(EStdKeyNkp9,KEYMAP_KEY_9)
			MAPDEVICETOMIDPKEY(EStdKeyNkp0,KEYMAP_KEY_0)
			MAPDEVICETOMIDPKEY(EStdKeyNkpAsterisk,KEYMAP_KEY_ASTERISK)
			MAPDEVICETOMIDPKEY(EStdKeyHash,KEYMAP_KEY_POUND)
#endif
			else if(aKeyEvent.iScanCode >= 32 && aKeyEvent.iScanCode < 127)
			{
				ret = aKeyEvent.iScanCode;
			}
			return ret;
		}
		
		virtual void Release() { delete this;}
	protected:
	virtual ~CStandardKeyboard() {}
};

MKeyMapper* TKeyMapperFactory::GetPlatformKeyMapperL()
{
	
	TInt uid = 0;
	HAL::Get(HALData::EMachineUid, uid);
	switch(uid)
	{
		// E61, E61i, E62 qwerty devices
		case 0x20001858:
		case 0x20002D7F:
		case 0x20001859:
			return new (ELeave) CQwertyKeyboard;
		default:
			return new (ELeave) CStandardKeyboard;
	}
}
