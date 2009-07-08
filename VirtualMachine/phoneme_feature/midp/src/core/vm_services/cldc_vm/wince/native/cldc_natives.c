/*
 *
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
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

/*
 * NOTICE: Portions Copyright (c) 2007-2009 Davy Preuveneers.
 * This file has been modified by Davy Preuveneers on 2009/01/11. The
 * changes are licensed under the terms of the GNU General Public
 * License version 2. This notice was added to meet the conditions of
 * Section 3.a of the GNU General Public License version 2.
 */

#include <jvmconfig.h>
#include <kni.h>
#include <midp_logging.h>
#include <midpMalloc.h>
#include <midpAMS.h>
#include <midpInit.h>
#include <midp_mastermode_port.h>
#include <midp_foreground_id.h>
#include <keymap_input.h>

#include <windows.h>
#include <Msgqueue.h>

static BOOL sendMidpEvent(MidpEvent* event, int size) {
    return FALSE;
}

static BOOL readMidpEvent(MidpEvent* event) {
    return FALSE;
}

BOOL sendMidpKeyEvent(MidpEvent* event, int size) {
    event->stringParam1.length = 0;
    event->stringParam2.length = 0;
    event->stringParam3.length = 0;
    event->stringParam4.length = 0;
    event->stringParam5.length = 0;
    event->stringParam6.length = 0;

    return sendMidpEvent(event, size);
}
