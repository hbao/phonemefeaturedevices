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

/*
 * NOTICE: Portions Copyright (c) 2007-2009 Davy Preuveneers.
 * This file has been modified by Davy Preuveneers on 2009/01/11. The
 * changes are licensed under the terms of the GNU General Public
 * License version 2. This notice was added to meet the conditions of
 * Section 3.a of the GNU General Public License version 2.
 */

#include <windows.h>
#include <string.h>
#include <midlet.h>
#include <midp_properties_port.h>
#include <midpMalloc.h>

/**
 * @file
 *
 * Platform implementation of the platformRequest() method call.
 */

/** Property name for platform request handler */
static const char* const PLATFORM_REQUEST_KEY =
    "com.sun.midp.midlet.platformRequestCommand";

/**
 * Starts a new process to handle the given URL. The new process executes
 * the value of the <tt>com.sun.midp.midlet.platformRequestCommand</tt>
 * system property. The URL is passed as this process' sole command-line
 * argument.
 *
 * @param pszUrl The 'C' string URL
 *
 * @return true if the platform request is configured
 */
int platformRequest(char* pszUrl) {
    char *pszCommand;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR command[MAX_PATH+1];
    TCHAR commandLine[1024];

    if (strlen(pszUrl) == 0) {
        /*
         * This is a request to cancel. Since a process was already spawned
         * to handle the previous URL, it too late.
         */
        return 1;
    }

    /*
     * #3202: Symbian: platformRequest for things beginning 
     * with "x-bw-" should be harmlessly ignored    
     */
    if (strncmp(pszUrl, "x-bw-", 5) == 0) {
        return 1;
    }

    /*
     * #2997: WinCE: platformRequest (e.g. from Help menu)
     * does not appear to be supported
     */
    pszCommand = (char *)getInternalProperty(PLATFORM_REQUEST_KEY);
    if (pszCommand == NULL) {
        REPORT_WARN(LC_AMS, "PlatformRequest is not configured.");
        return 0;
    }

    MultiByteToWideChar(CP_ACP, 0, pszCommand, -1, command, MAX_PATH);
    MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, commandLine, 1023);

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    /* spawn the request using the configured URL handler and URL parameter
     * do not inherit handles
     */
    if (CreateProcess(command, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        REPORT_WARN(LC_AMS, "Spawning a handler process failed. Check the platformRequest configuration. ");
    }

    return 1;
}
