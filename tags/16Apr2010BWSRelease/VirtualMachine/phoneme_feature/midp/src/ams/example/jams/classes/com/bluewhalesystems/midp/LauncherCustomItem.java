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

public class LauncherCustomItem extends CustomItem {
    private static boolean iDebug  = false;
    private static final int     EASTER_EGG_KEY_BUFFER_LENGTH        = 5;
    private int[]                iEasterEggKeyBuffer                 = new int[EASTER_EGG_KEY_BUFFER_LENGTH];
    private static final int[]   iEasterEgg                          = { 52, 52, 50, 50, 54 };
    private boolean iPreprod;
    private Form iForm;

    private static void debugMessage(String aMessage) {
        if (iDebug) {
            System.out.println(aMessage);
        }
    }

    LauncherCustomItem(Form aForm) {
        super("");
        iPreprod = false;
        iForm = aForm;
    }

    public boolean getPreprod() {
        return iPreprod;
    }

    protected void paint(Graphics g, int w, int h) {
    }

    protected int getPrefContentHeight(int width) {
        return 0;
    }

    protected int getPrefContentWidth(int height) {
        return 0;
    }

    protected int getMinContentHeight() {
        return 0;
    }

    protected int getMinContentWidth() {
        return 0;
    }

    protected void keyPressed(int keyCode) {
        debugMessage("keyPressed " + keyCode);

        // Store the latest key entered.
        System.arraycopy( iEasterEggKeyBuffer, 1, iEasterEggKeyBuffer, 0, EASTER_EGG_KEY_BUFFER_LENGTH - 1 );
        iEasterEggKeyBuffer[EASTER_EGG_KEY_BUFFER_LENGTH - 1] = keyCode;

        // See if a relevant 5-digit easter egg code has been entered.
        for ( int i = 0; i < EASTER_EGG_KEY_BUFFER_LENGTH; ++i ) {
            if ( iEasterEggKeyBuffer[i] != iEasterEgg[i] ) {
                // Not of interest -- return
                return;
            }
        }

        // If we got here it means the user has entered the easter egg code.
        iPreprod = !iPreprod;
        if (iPreprod) {
            iForm.setTitle("Download [preprod]");
        } else {
            iForm.setTitle("Download");
        }
    }
}
