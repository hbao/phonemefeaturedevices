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

#define MAX_URL 1024

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
    SHELLEXECUTEINFO shExecInfo;
    TCHAR command[MAX_PATH+1];
    TCHAR commandLine[MAX_URL+1];

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

    // #3519: WinCE VM: Support platformRequest("tel:+447786116478");
    // Pass all requests to a shell and let it figure out the most
    // application to execute the request

    MultiByteToWideChar(CP_ACP, 0, pszCommand, -1, command, MAX_PATH-1);
    MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, commandLine, MAX_URL-1);

    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = NULL;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = NULL;
    shExecInfo.lpFile = commandLine;
    shExecInfo.lpParameters = NULL;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_MAXIMIZE;
    shExecInfo.hInstApp = NULL;
    ShellExecuteEx(&shExecInfo);

    return 1;
}
