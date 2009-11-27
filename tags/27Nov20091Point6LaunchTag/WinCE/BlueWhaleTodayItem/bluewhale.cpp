/*
 * Copyright 2009 Davy Preuveneers, Blue Whale Systems Ltd. All Rights Reserved.
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
 */

#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include "stdafx.h"

#define FLAGS_START_POWERUP       0x1
#define FLAGS_START_RESUME        0x2

#include <Msgqueue.h>
#include <PM.h>
#define WM_POWER_STATE_RESUME     (WM_USER+0)

#ifndef MAX_MESSAGES
#define MAX_MESSAGES              5
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE                (MAX_MESSAGES * (sizeof(POWER_BROADCAST) + (MAX_PATH * sizeof(TCHAR))))
#endif

#define MAX_BUF_SIZE              4096

HANDLE      hMsgQueue            = NULL;
HANDLE      hPowerNotify         = NULL;
HANDLE      hPowerNotifyThread   = NULL;

HICON       hIcon;
HINSTANCE   hInst                = NULL;
HWND        hWnd                 = NULL;
BOOL        fStartPowerUp        = TRUE;
BOOL        fStartResume         = TRUE;
DWORD       dwAutostartFlags     = FLAGS_START_POWERUP | FLAGS_START_RESUME;

static INT InitializeClasses();

BOOL GetInstallDir(TCHAR *pszInstallDir) {
    DWORD               dwSize;
    DWORD               dwValType;
    DWORD               rc;
    HKEY                hKey;
    BOOL                ret;

    ret = FALSE;
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      TEXT("Software\\Apps\\Blue Whale Systems Ltd BlueWhale"), 0, KEY_QUERY_VALUE, &hKey);

    dwSize = 0;
    rc = RegQueryValueEx(hKey, TEXT("InstallDir"), NULL, &dwValType, NULL, &dwSize);
    if (dwSize > 0) {
        rc = RegQueryValueEx(hKey, TEXT("InstallDir"), NULL, &dwValType, (LPBYTE)pszInstallDir, &dwSize);
        if (rc == ERROR_SUCCESS) {
            ret = TRUE;
        }
    }
    RegCloseKey(hKey);
    return ret;
}

BOOL GetRunMidlet(TCHAR *pszFilePath) {
    WIN32_FIND_DATA     findFileData;
    HANDLE              hFind;
    BOOL                ret;

    ret = FALSE;
    if (GetInstallDir(pszFilePath)) {
        wcscat(pszFilePath, TEXT("\\bin\\arm\\runMidlet.exe"));
        hFind = FindFirstFile(pszFilePath, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
            ret = TRUE;
        }
    }
    return ret;
}

BOOL GetMidletDir(TCHAR *pszPathName) {
    BOOL    ret;

    ret = FALSE;
    if (GetInstallDir(pszPathName)) {
        wcscat(pszPathName, TEXT("\\midlets"));
        CreateDirectory(pszPathName, NULL);
        ret = TRUE;
    }
    return ret;
}

BOOL RunApplication() {
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    TCHAR pszRunMidlet[MAX_PATH];
    TCHAR pszParameters[MAX_BUF_SIZE];
    TCHAR pszPathName[MAX_BUF_SIZE];

    if (GetRunMidlet(pszRunMidlet)) {
        memset(pszPathName, 0, sizeof(pszPathName));
        GetMidletDir(pszPathName);
        wsprintf(pszParameters, TEXT(" -1 com.bluewhalesystems.midp.Launcher"));

        if (!CreateProcess(pszRunMidlet, pszParameters, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {
            MessageBox(NULL, TEXT("Cannot run MIDP application."), TEXT("BlueWhale"), MB_OK);
            return FALSE;
        }
        return TRUE;
    } else {
        MessageBox(NULL, TEXT("VM is not installed!"), TEXT("BlueWhale"), MB_OK);
    }
    return FALSE;
}

int WINAPI PowerNotifyThread(LPVOID pvParam) {
    char    buffer[QUEUE_SIZE];
    int     numberOfBytesRead = 0;
    DWORD   dwFlags = 0;

    while (TRUE) {
        if (WaitForSingleObject(hMsgQueue, INFINITE) != WAIT_OBJECT_0) {
            return 0;
        }
        memset(buffer, 0, QUEUE_SIZE);
        if (ReadMsgQueue(hMsgQueue, buffer, QUEUE_SIZE, (LPDWORD)&numberOfBytesRead, INFINITE, &dwFlags)) {
            if (numberOfBytesRead >= sizeof(POWER_BROADCAST)) {
                PPOWER_BROADCAST pPB = (PPOWER_BROADCAST)buffer;
                if (pPB->Message == PBT_RESUME) {
                    SendMessage(hWnd, WM_POWER_STATE_RESUME, 0, 0);
                }
            }
        }
    }
    return 0;
}

BOOL StartPowerNotifyThread() {
    MSGQUEUEOPTIONS msgQueueOptions = {0};

    if (hPowerNotifyThread)
        return FALSE;

    msgQueueOptions.dwSize = sizeof(MSGQUEUEOPTIONS);
    msgQueueOptions.dwFlags = 0;
    msgQueueOptions.dwMaxMessages = MAX_MESSAGES;
    msgQueueOptions.cbMaxMessage = sizeof(POWER_BROADCAST) + MAX_PATH;
    msgQueueOptions.bReadAccess = TRUE;

    hMsgQueue = CreateMsgQueue(NULL, &msgQueueOptions);
    if (hMsgQueue == NULL) {
        return FALSE;
    }

    hPowerNotify = RequestPowerNotifications(hMsgQueue, POWER_NOTIFY_ALL);
    if (hPowerNotify == NULL) {
        return FALSE;
    }

    hPowerNotifyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PowerNotifyThread, NULL, 0, NULL);
    return TRUE;
}

BOOL ShutdownPowerNotifyThread() {
    if (hPowerNotifyThread) {
        TerminateThread(hPowerNotifyThread, -1);
        hPowerNotifyThread = NULL;
    }

    if (hPowerNotify) {
        StopPowerNotifications(hPowerNotify);
        hPowerNotify = NULL;
    }

    if (hMsgQueue) {
        CloseMsgQueue(hMsgQueue);
        hMsgQueue = NULL;
    }
    return TRUE;
}

BOOL CreateStartupScript() {
    TCHAR pszRunMidlet[MAX_PATH];
    TCHAR pszLink[MAX_PATH];
    TCHAR pszPathName[MAX_PATH];
    TCHAR pszFilePath[MAX_PATH];
    TCHAR pszParameters[MAX_BUF_SIZE];
    int   suiteID;

    memset(pszRunMidlet, 0, sizeof(pszRunMidlet));
    if (GetRunMidlet(pszRunMidlet)) {
        memset(pszFilePath, 0, sizeof(pszFilePath));
        wcscpy(pszFilePath, TEXT("\\Windows\\StartUp\\BlueWhale.lnk"));
        memset(pszLink, 0, sizeof(pszLink));
        wcscpy(pszLink, TEXT("\""));
        wcscat(pszLink, pszRunMidlet);

        memset(pszPathName, 0, sizeof(pszPathName));
        GetMidletDir(pszPathName);
        wsprintf(pszParameters, TEXT("\" -1 com.bluewhalesystems.midp.Launcher"));
        wcscat(pszLink, pszParameters);

        SHCreateShortcut(pszFilePath, pszLink);
        return TRUE;
    }
    return FALSE;
}

BOOL DeleteStartupScript() {
    TCHAR pszFilePath[MAX_PATH];

    memset(pszFilePath, 0, sizeof(pszFilePath));
    wcscpy(pszFilePath, TEXT("\\Windows\\StartUp\\BlueWhale.lnk"));
    return DeleteFile(pszFilePath);
}

void trim(char *line) {
    while ((line[strlen(line)-1] == '\n') || (line[strlen(line)-1] == '\r') || (line[strlen(line)-1] == ' '))
        line[strlen(line)-1] = '\0';

    while ((line[0] == '\n') || (line[0] == '\r') || (line[0] == ' '))
        strcpy(line, line+1);
}

BOOL WriteConfig() {
    TCHAR    pszFilePath[MAX_PATH];
    BOOL     ret;
    char     config[MAX_PATH];
    FILE     *f;

    ret = FALSE;
    if (GetInstallDir(pszFilePath)) {
        wcscat(pszFilePath, TEXT("\\launcher.txt"));
        WideCharToMultiByte(CP_ACP, 0, pszFilePath, -1, config, wcslen(pszFilePath), NULL, NULL);
        config[wcslen(pszFilePath)] = '\0';

        f = fopen(config, "wt");
        fprintf(f, "%d", dwAutostartFlags);
        fclose(f);
        ret = TRUE;
    }
    return ret;
}

BOOL ReadConfig() {
    TCHAR               pszFilePath[MAX_PATH];
    WIN32_FIND_DATA     findFileData;
    HANDLE              hFind;
    BOOL                ret;
    char                config[MAX_PATH], line[MAX_PATH];
    FILE                *f;

    ret = FALSE;
    if (GetInstallDir(pszFilePath)) {
        wcscat(pszFilePath, TEXT("\\launcher.txt"));
        hFind = FindFirstFile(pszFilePath, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);

            WideCharToMultiByte(CP_ACP, 0, pszFilePath, -1, config, wcslen(pszFilePath), NULL, NULL);
            config[wcslen(pszFilePath)] = '\0';

            f = fopen(config, "rt");
            fgets(line, sizeof(line), f);
            trim(line);
            fclose(f);

            dwAutostartFlags = atoi(line);

            ret = TRUE;
        } else {
            dwAutostartFlags = FLAGS_START_POWERUP | FLAGS_START_RESUME;
            WriteConfig();
        }
    }
    return ret;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        hInst = (HINSTANCE)hModule;
        hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_DISPLAYICON), IMAGE_ICON, DRA::SCALEX(16), DRA::SCALEY(16), LR_DEFAULTCOLOR);
        UnregisterClass((LPCTSTR)LoadString(hInst, IDS_TODAY_BLUEWHALE_APPNAME, 0, 0), hInst);
        InitializeClasses();
        hWnd = 0;
        break;
    case DLL_PROCESS_DETACH:
        DestroyIcon(hIcon);
        UnregisterClass((LPCTSTR)LoadString(hInst, IDS_TODAY_BLUEWHALE_APPNAME, 0, 0), hInst);
        hInst = NULL;
        break;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam) {
    switch (uimessage) {
    case WM_TODAYCUSTOM_QUERYREFRESHCACHE: {
        TODAYLISTITEM *ptliItem;
        INT iItemHeight;
        BOOL bReturn = FALSE;

        ptliItem = (TODAYLISTITEM*)wParam;
        if ((NULL == ptliItem) || (WaitForSingleObject(SHELL_API_READY_EVENT, 0) == WAIT_TIMEOUT)) {
            return FALSE;
        }

        fStartPowerUp = (ptliItem->grfFlags & FLAGS_START_POWERUP);
        fStartResume = (ptliItem->grfFlags & FLAGS_START_RESUME);
        iItemHeight = DRA::SCALEY(20);

        if (0 == ptliItem->cyp) {
            ptliItem->cyp = iItemHeight;
            bReturn = TRUE;
        }

        return bReturn;
    }
    case WM_CREATE:
        ReadConfig();

        if (dwAutostartFlags & FLAGS_START_POWERUP)
            CreateStartupScript();
        else
            DeleteStartupScript();

        if (dwAutostartFlags & FLAGS_START_RESUME)
            StartPowerNotifyThread();
        else
            ShutdownPowerNotifyThread();

        break;
    case WM_LBUTTONUP:
        RunApplication();
        break;
    case WM_PAINT:
        PAINTSTRUCT     ps;
        RECT            rcWindBounds;
        RECT            rcMyBounds;
        HDC             hDC;
        HFONT           hFontOld;
        COLORREF        crText;
        TCHAR           pszLine[256];

        GetWindowRect(hwnd, &rcWindBounds);
        hDC = BeginPaint(hwnd, &ps);

        rcMyBounds.left = 0;
        rcMyBounds.top = DRA::SCALEY(2);
        rcMyBounds.right = rcWindBounds.right - rcWindBounds.left;
        rcMyBounds.bottom = rcWindBounds.bottom - rcWindBounds.top;

        SetBkMode(hDC, TRANSPARENT);
        DrawIcon(hDC, 2, 0, hIcon);

        LOGFONT lf;
        HFONT hSysFont;
        HFONT hFont;

        hSysFont = (HFONT)GetStockObject(SYSTEM_FONT);
        GetObject(hSysFont, sizeof(LOGFONT), &lf);
        lf.lfHeight = (long) -((8.0 * (double)GetDeviceCaps(hDC, LOGPIXELSY) / 72.0)+.5);

        hFont = CreateFontIndirect(&lf);
        hFontOld = (HFONT)SelectObject(hDC, hFont);

        crText = SendMessage(GetParent(hwnd), TODAYM_GETCOLOR, (WPARAM)TODAYCOLOR_TEXT, NULL);
        SetTextColor(hDC, crText);
        rcMyBounds.left = rcMyBounds.left + DRA::SCALEX(28);

        wsprintf(pszLine, TEXT("BlueWhale"));

        DrawText(hDC, pszLine, -1, &rcMyBounds, DT_LEFT);

        SelectObject(hDC, hFontOld);
        DeleteObject(hFont);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY :
        WriteConfig();
        return 0 ;
    case WM_ERASEBKGND:
        TODAYDRAWWATERMARKINFO dwi;
        dwi.hdc = (HDC)wParam;
        GetClientRect(hwnd, &dwi.rc);

        dwi.hwnd = hwnd;
        SendMessage(GetParent(hwnd), TODAYM_DRAWWATERMARK, 0,(LPARAM)&dwi);
        return TRUE;
    case WM_POWER_STATE_RESUME:
        RunApplication();
        break;
    }

    return DefWindowProc(hwnd, uimessage, wParam, lParam);
}

INT InitializeClasses() {
    WNDCLASS         wc;

    memset(&wc, 0, sizeof(wc));
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.hInstance     = hInst;
    wc.hIcon         = 0;
    wc.hCursor       = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = (LPCTSTR)LoadString(hInst, IDS_TODAY_BLUEWHALE_APPNAME, 0, 0);

    if (!RegisterClass(&wc))
        return 0 ;

    return 1;
}

HWND InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent) {
    LPCTSTR appName = (LPCTSTR)LoadString(hInst, IDS_TODAY_BLUEWHALE_APPNAME, 0, 0);

    hWnd = CreateWindow(appName, appName, WS_VISIBLE | WS_CHILD,
                        CW_USEDEFAULT, CW_USEDEFAULT, 240, 0, hwndParent, NULL, hInst, NULL) ;

    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WndProc);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    return hWnd;
}

LRESULT WINAPI CustomItemOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static TODAYLISTITEM *s_ptliItem = NULL;

    switch (message) {
    case WM_INITDIALOG:
        SHINITDLGINFO shidi;
        BOOL fStartPowerUp, fStartResume;

        shidi.dwMask = SHIDIM_FLAGS;
        shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
        shidi.hDlg = hDlg;
        SHInitDialog(&shidi);

        s_ptliItem = (TODAYLISTITEM*)lParam;
        if (NULL != s_ptliItem) {
            ReadConfig();
            s_ptliItem->grfFlags = dwAutostartFlags;

            fStartPowerUp = (s_ptliItem->grfFlags & FLAGS_START_POWERUP);
            fStartResume = (s_ptliItem->grfFlags & FLAGS_START_RESUME);
        } else {
            fStartPowerUp = (dwAutostartFlags & FLAGS_START_POWERUP);
            fStartResume = (dwAutostartFlags & FLAGS_START_RESUME);
        }

        if (fStartPowerUp) {
            CheckDlgButton(hDlg, IDC_POWERUP, BST_CHECKED);
            CreateStartupScript();
        } else {
            CheckDlgButton(hDlg, IDC_POWERUP, BST_UNCHECKED);
            DeleteStartupScript();
        }

        if (fStartResume) {
            CheckDlgButton(hDlg, IDC_RESUME, BST_CHECKED);
            StartPowerNotifyThread();
        } else {
            CheckDlgButton(hDlg, IDC_RESUME, BST_UNCHECKED);
            ShutdownPowerNotifyThread();
        }
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            HWND hCurrentDlgButton;
            BOOL fStartPowerUp, fStartResume;

            hCurrentDlgButton = GetDlgItem(hDlg, IDC_POWERUP);
            if (BST_CHECKED == SendMessage(hCurrentDlgButton, BM_GETCHECK, NULL, NULL)) {
                fStartPowerUp = TRUE;
                CreateStartupScript();
            } else {
                fStartPowerUp = FALSE;
                DeleteStartupScript();
            }

            hCurrentDlgButton = GetDlgItem(hDlg, IDC_RESUME);
            if (BST_CHECKED == SendMessage(hCurrentDlgButton, BM_GETCHECK, NULL, NULL)) {
                fStartResume = TRUE;
                StartPowerNotifyThread();
            } else {
                fStartResume = FALSE;
                ShutdownPowerNotifyThread();
            }

            dwAutostartFlags = (fStartPowerUp ? FLAGS_START_POWERUP : 0) |
                                   (fStartResume ? FLAGS_START_RESUME : 0);

            s_ptliItem->grfFlags = dwAutostartFlags;

            WriteConfig();
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_DESTROY:
        s_ptliItem = NULL;
        break;
    }

    return FALSE;
}
