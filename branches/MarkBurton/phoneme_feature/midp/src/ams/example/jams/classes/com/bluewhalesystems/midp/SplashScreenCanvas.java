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

package com.bluewhalesystems.midp;

import javax.microedition.lcdui.*;

public class SplashScreenCanvas extends Canvas
{
    protected SplashScreenCanvas()
    {
        super();
        setTitle( null );
        setFullScreenMode( true );
    }

    private Image iCachedImage;

    private int   iStartColor = 0x44BEF6;
    private int   iEndColor   = 0x142A64;

    private int adjustColor( int aStartColor, int aEndColor, int aTotal, int aCurrent )
    {
        if( aStartColor == aEndColor )
        {
            return aStartColor;
        }
        float proportion = ( (float) aCurrent ) / ( (float) aTotal );
        int delta = (int) ( Math.abs( aEndColor - aStartColor ) * proportion );
        if( aStartColor > aEndColor )
        {
            return aStartColor - delta;
        }
        else
        {
            return aStartColor + delta;
        }
    }

    public void fillRadialGradient( Graphics aGraphics, int aStartColor, int aEndColor, int x, int y, int aWidth, int aHeight )
    {
        int startRed = aStartColor >> 16 & 0xff;
        int startGreen = aStartColor >> 8 & 0xff;
        int startBlue = aStartColor & 0xff;
        int endRed = aEndColor >> 16 & 0xff;
        int endGreen = aEndColor >> 8 & 0xff;
        int endBlue = aEndColor & 0xff;
        int oldColor = aGraphics.getColor();
        int currentHeight = aHeight;
        while( ( 0 < aWidth ) && ( 0 < aHeight ) )
        {
            int r = adjustColor( startRed, endRed, aHeight, currentHeight );
            int g = adjustColor( startGreen, endGreen, aHeight, currentHeight );
            int b = adjustColor( startBlue, endBlue, aHeight, currentHeight );
            int color = ( ( r << 16 ) & 0xff0000 ) | ( ( g << 8 ) & 0xff00 ) | ( b & 0xff );
            aGraphics.setColor( color );

            aGraphics.fillArc( x, y, aWidth, currentHeight, 0, 360 );
            x++;
            y++;
            aWidth -= 2;
            currentHeight -= 2;
        }
        aGraphics.setColor( oldColor );
    }

    public void paint( Graphics aGraphics )
    {
        // Don't leave the background colour up to chance.
        // Set it explicitly -- see ticket:2366 "Symbian: Seeing a black screen instead of the splash screen on startup."
        aGraphics.setColor( 0xFFFFFF /* white to avoid flashing since first screen is white in MR3 VM */);

        int x = aGraphics.getClipX();
        int y = aGraphics.getClipY();
        int width = aGraphics.getClipWidth();
        int height = aGraphics.getClipHeight();

        int size = 2 * Math.max( width, height );
        if( null == iCachedImage || iCachedImage.getWidth() != size )
        {
            iCachedImage = Image.createImage( size, size );
            Graphics g2 = iCachedImage.getGraphics();

            g2.setColor( iEndColor );
            g2.fillRect( 0, 0, size, size );
            fillRadialGradient( g2, iStartColor, iEndColor, 0, 0, size, size );
        }
        aGraphics.drawImage( iCachedImage, width - size / 2, y - size / 2, Graphics.TOP | Graphics.LEFT );
    }
}
