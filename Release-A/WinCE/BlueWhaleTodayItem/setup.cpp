#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <windowsx.h>
#include <aygshell.h>
#include <commctrl.h>
#include <sipapi.h>

#include "ce_setup.h"

TCHAR sPlatformName[128] = TEXT("BlueWhale");
TCHAR sPartnerName[128] = TEXT("");
TCHAR sAppName[128] = TEXT("BlueWhaleMail");
TCHAR sAppFullName[128] = TEXT("com.bluewhalesystems.client.midlet.BlueWhaleMail");

static void ToLower(TCHAR* s) {
    while (*s) {
        *s = tolower(*s);
        s++;
    }
}

static BOOL GetStringEnv(TCHAR *pszVar, TCHAR *pszValue) {
    HKEY                hKey;
    DWORD               dwSize, dwValType, rc;
    BOOL                ret;

    ret = FALSE;
    dwSize = 0;
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Apps\\Blue Whale Systems Ltd BlueWhale"), 0, KEY_QUERY_VALUE, &hKey);
    if (rc == ERROR_SUCCESS) {
        rc = RegQueryValueEx(hKey, pszVar, NULL, &dwValType, NULL, &dwSize);
        if (rc == ERROR_SUCCESS) {
            if (dwSize > 0) {
                rc = RegQueryValueEx(hKey, pszVar, NULL, &dwValType, (LPBYTE)pszValue, &dwSize);
                if (rc == ERROR_SUCCESS) {
                    ret = TRUE;
                }
            }
        }
    }
    RegCloseKey(hKey);
    return ret;
}

static BOOL SetStringEnv(TCHAR *pszVar, TCHAR *pszValue) {
    HKEY                hKey;
    DWORD               dwDisposition, rc;
    BOOL                ret;

    ret = FALSE;
    rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Apps\\Blue Whale Systems Ltd BlueWhale"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (rc == ERROR_SUCCESS) {
        RegSetValueEx(hKey, pszVar, 0, REG_SZ, (LPBYTE)pszValue, (wcslen(pszValue)+1) * sizeof(TCHAR));
        ret = TRUE;
    }
    RegCloseKey(hKey);
    return ret;
}

BOOL FindCabinet(const TCHAR* sHaystack) {
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    BOOL bFileFound;
    TCHAR sDirPath[MAX_PATH];
    TCHAR sFileName[MAX_PATH];
    TCHAR sFileFound[MAX_PATH];

    wcscpy(sDirPath, sHaystack);
    wcscat(sDirPath, TEXT("\\*.*"));
    wcscpy(sFileName, sHaystack);
    wcscat(sFileName, TEXT("\\"));

    hFind = FindFirstFile(sDirPath, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) return FALSE;
    wcscpy(sDirPath, sFileName);

    bFileFound = FALSE;
    while (FindNextFile(hFind, &FindFileData)) {
        if (!wcscmp(FindFileData.cFileName, TEXT(".")) || !wcscmp(FindFileData.cFileName, TEXT(".."))) 
            continue;

        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            wcscat(sFileName, FindFileData.cFileName);
            if (FindCabinet(sFileName)) {
                bFileFound = TRUE;
                break;
            }
        } else {
            wcscpy(sFileName, FindFileData.cFileName);
            ToLower(sFileName);
            if (wcsstr(sFileName, TEXT("bluewhale")) && wcsstr(sFileName, TEXT(".cab"))) {
                wcscpy(sFileFound, sDirPath);
                wcscat(sFileFound, FindFileData.cFileName);
                if (wcsstr(sFileName, TEXT("-sky-"))) {
                    wcscpy(sPartnerName, TEXT("sky"));
                    wcscpy(sAppName, TEXT("SkyMobileEmail"));
                }
                bFileFound = TRUE;
                break;
            }
        }
        wcscpy(sFileName, sDirPath);
    }
    FindClose(hFind);
    return bFileFound;
}

///////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT START OF INSTALLATION

///////////////////////////////////////////////////////////

codeINSTALL_INIT Install_Init(HWND hwndparent,
  BOOL ffirstcall,BOOL fpreviouslyinstalled,LPCTSTR pszinstalldir)
{
    BOOL bFileFound;

    bFileFound = FindCabinet(TEXT("\\My Documents"));
    if (!bFileFound)
        bFileFound = FindCabinet(TEXT("\\Storage Card"));

    SetStringEnv(TEXT("x-bw-platform-name"), sPlatformName);
    SetStringEnv(TEXT("x-bw-partner-name"), sPartnerName);
    SetStringEnv(TEXT("x-bw-app-name"), sAppName);
    SetStringEnv(TEXT("x-bw-app-full-name"), sAppFullName);

    //return value

    return codeINSTALL_INIT_CONTINUE;
}

///////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT END OF INSTALLATION

///////////////////////////////////////////////////////////

codeINSTALL_EXIT Install_Exit(
    HWND hwndparent,LPCTSTR pszinstalldir,
    WORD cfaileddirs,WORD cfailedfiles,WORD cfailedregkeys,
    WORD cfailedregvals,
    WORD cfailedshortcuts)
{
    WIN32_FIND_DATA findfiledata;
    HANDLE hfind;
    TCHAR pszfilepath[255], pszlink[255];

    memset(pszfilepath,0,sizeof(pszfilepath));
    wcscpy(pszfilepath,pszinstalldir);
    wcscat(pszfilepath,TEXT("\\bin\\arm\\runMidlet.exe"));
    hfind = FindFirstFile(pszfilepath,&findfiledata);
    if (hfind != INVALID_HANDLE_VALUE) {
        memset(pszfilepath,0,sizeof(pszfilepath));
        wcscpy(pszfilepath,TEXT("\\Windows\\Start Menu\\Programs\\BlueWhale.lnk"));
        memset(pszlink,0,sizeof(pszlink));
        wcscpy(pszlink,TEXT("\""));
        wcscat(pszlink,pszinstalldir);
        wcscat(pszlink,TEXT("\\bin\\arm\\runMidlet.exe\""));
        SHCreateShortcut(pszfilepath, pszlink);

        memset(pszfilepath,0,sizeof(pszfilepath));
        wcscpy(pszfilepath,TEXT("\\Windows\\Start Menu\\Programs\\"));
        wcscat(pszfilepath,sAppName);
        wcscat(pszfilepath,TEXT(".lnk"));
        memset(pszlink,0,sizeof(pszlink));
        wcscpy(pszlink,TEXT("\""));
        wcscat(pszlink,pszinstalldir);
        wcscat(pszlink,TEXT("\\bin\\arm\\runMidlet.exe\" -1 com.bluewhalesystems.midp.Launcher"));
        SHCreateShortcut(pszfilepath, pszlink);

        /*
        // Davy: We should not run the BlueWhaleMail this way anymore after a cold start.
        // The Today Item should take care of autostarting the VM if required.

        memset(pszfilepath,0,sizeof(pszfilepath));
        wcscpy(pszfilepath,TEXT("\\Windows\\StartUp\\BlueWhale.lnk"));
        memset(pszlink,0,sizeof(pszlink));
        wcscpy(pszlink,TEXT("\""));
        wcscat(pszlink,pszinstalldir);
        wcscat(pszlink,TEXT("\\bin\\arm\\runMidlet.exe\" -1 com.bluewhalesystems.midp.Launcher"));
        SHCreateShortcut(pszfilepath, pszlink);
        */
    }

    return codeINSTALL_EXIT_DONE;
}

///////////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT BEGINNING OF UNINSTALLATION

///////////////////////////////////////////////////////////////

codeUNINSTALL_INIT Uninstall_Init(
  HWND hwndparent,LPCTSTR pszinstalldir)
{
    TCHAR sTmp[256];

    //pszinstalldir variable will contain the 
    //application path (eg : \Program Files\TestApp)

    WIN32_FIND_DATA findfiledata;
    HANDLE hfind;
    TCHAR pszfilepath[255];

    wcscpy(pszfilepath,TEXT("\\Windows\\Start Menu\\Programs\\BlueWhale.lnk"));
    hfind = FindFirstFile(pszfilepath, &findfiledata);
    if (hfind != INVALID_HANDLE_VALUE) {
        DeleteFile(pszfilepath);
    }

    wcscpy(pszfilepath,TEXT("\\Windows\\Start Menu\\BlueWhale.lnk"));
    hfind = FindFirstFile(pszfilepath, &findfiledata);
    if (hfind != INVALID_HANDLE_VALUE) {
        DeleteFile(pszfilepath);
    }

    wcscpy(pszfilepath,TEXT("\\Windows\\StartUp\\BlueWhale.lnk"));
    hfind = FindFirstFile(pszfilepath, &findfiledata);
    if (hfind != INVALID_HANDLE_VALUE) {
        DeleteFile(pszfilepath);
    }

    GetStringEnv(TEXT("x-bw-app-name"), sTmp);
    wcscpy(pszfilepath,TEXT("\\Windows\\Start Menu\\Programs\\"));
    wcscat(pszfilepath,sTmp);
    wcscat(pszfilepath,TEXT(".lnk"));
    hfind = FindFirstFile(pszfilepath, &findfiledata);
    if (hfind != INVALID_HANDLE_VALUE) {
        DeleteFile(pszfilepath);
    }

    return codeUNINSTALL_INIT_CONTINUE;
}


///////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT END OF UNINSTALLATION

///////////////////////////////////////////////////////////

codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndparent)
{
    //do nothing

    //return value

    return codeUNINSTALL_EXIT_DONE;
}
