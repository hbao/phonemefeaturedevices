/*
 *
 *
 * Copyright  1990-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */
package com.sun.jsr082.bluetooth;

import java.io.IOException;
import javax.bluetooth.UUID;
import javax.microedition.io.Connector;
import javax.microedition.io.Connection;
import com.sun.jsr082.bluetooth.btl2cap.Protocol;

/*
 * Contains common Service Discovery Protocol data.
 */
public class SDP {
    /* Internal security token that grants access to restricted API. */
//    private static SecurityToken internalSecurityToken = null;

    /* Special object used by SDP to authenticate internal URL's. */
    private static Object systemToken = new Object();

    /* SDP UUID. */
    public static final String UUID = new UUID(0x0001).toString();

    /* SDP server PSM. This value is defined by Bluetooth specification */
    public static final int PSM = 0x0001;

    /*
     * Initializes internal security token. Called by internal MIDP
     * initialization routines.
     *
     * @param token internal security token that grants access to restricted API
     */
/*
    public static void initSecurityToken(SecurityToken token) {
        if (internalSecurityToken == null) {
            internalSecurityToken = token;
        }
    }
*/
    /*
     * Creates and opens btl2cap connection for using by SDP.
     *
     * @param name btl2cap connection URL without protocol name
     * @return required connection instance, that is
     *        <code>L2CAPConnection</code> for client or
     *        <code>L2CAPConnectionNotifier</code> for server
     * @exception IOException if connection fails
     */
    static Connection getL2CAPConnection(String name) throws IOException {
        BluetoothUrl url = new BluetoothUrl(BluetoothUrl.L2CAP, name, systemToken);
        Protocol l2cap = new Protocol();
        return l2cap.openPrim(url, Connector.READ_WRITE);
    }

    /*
     * Checks that given token is an internal one. Used to authenticate
     * an URL as generated by SDP.
     * @param token token to check
     * @return <code>true</code> if the object given is exactly the internal
     *         system token, <code>false</code> otherwise
     */
    public static boolean checkSystemToken(Object token) {
        return token == systemToken;
    }
}
