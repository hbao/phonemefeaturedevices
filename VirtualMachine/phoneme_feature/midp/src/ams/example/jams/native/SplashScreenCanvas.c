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

#include <gxj_putpixel.h>

int AdjustColor(int aStartColor, int aEndColor, int aTotal, int aCurrent)
{
	int delta;
	float proportion;
	if( aStartColor == aEndColor )
	{
		return aStartColor;
	}
	proportion = ( (float) aCurrent ) / ( (float) aTotal );
	delta = (int) ( (aEndColor - aStartColor ) * proportion );
	return aStartColor + delta;
}

void DrawSplashScreen()
{
	int startColor = 0x44BEF6;
	int endColor   = 0x142A64;
	jshort clip[4];
	int height;
	int currentHeight;

	int startRed = startColor >> 16 & 0xff;
	int startGreen = startColor >> 8 & 0xff;
	int startBlue = startColor & 0xff;
	int endRed = endColor >> 16 & 0xff;
	int endGreen = endColor >> 8 & 0xff;
	int endBlue = endColor & 0xff;
	int width;
	if (gxj_system_screen_buffer.width > gxj_system_screen_buffer.height)
	{
		width = 2 * gxj_system_screen_buffer.width;
	}
	else
	{
		width = 2 * gxj_system_screen_buffer.height;
	}
	height = width;
	currentHeight = height;
	
	clip[0] = 0;
	clip[1] = 0;
	clip[2] = gxj_system_screen_buffer.width;
	clip[3] = gxj_system_screen_buffer.height;

	draw_roundrect(GXJ_RGB24TORGB16(endColor), clip, &gxj_system_screen_buffer, 0, 0, 0, gxj_system_screen_buffer.width, gxj_system_screen_buffer.height, 1, 0, 0);

	while( ( 0 < width ) && ( 0 < height ) )
	{
		int r = AdjustColor( startRed, endRed, height, currentHeight );
		int g = AdjustColor( startGreen, endGreen, height, currentHeight );
		int b = AdjustColor( startBlue, endBlue, height, currentHeight );
		int color = ( ( r << 16 ) & 0xff0000 ) | ( ( g << 8 ) & 0xff00 ) | ( b & 0xff );

		draw_arc(GXJ_RGB24TORGB16(color), clip, &gxj_system_screen_buffer, 0, gxj_system_screen_buffer.width - (width / 2), (currentHeight / 2) - currentHeight, width, currentHeight, 1, 0, 360);

		width -= 2;
		currentHeight -= 2;
	}
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_com_bluewhalesystems_midp_SplashScreenCanvas_DrawSplashScreen0()
{
	DrawSplashScreen();
	KNI_ReturnVoid();
}