#include <windows.h>

#include "ce_setup.h"


///////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT START OF INSTALLATION

///////////////////////////////////////////////////////////

codeINSTALL_INIT Install_Init(HWND hwndparent,
  BOOL ffirstcall,BOOL fpreviouslyinstalled,LPCTSTR pszinstalldir)
{
    //do nothing

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
	wcscat(pszlink,TEXT("\\bin\\arm\\runMidlet.exe\" -1 com.bluewhalesystems.midp.Launcher"));
	SHCreateShortcut(pszfilepath, pszlink);

	memset(pszfilepath,0,sizeof(pszfilepath));
	wcscpy(pszfilepath,TEXT("\\Windows\\StartUp\\BlueWhale.lnk"));
	memset(pszlink,0,sizeof(pszlink));
	wcscpy(pszlink,TEXT("\""));
	wcscat(pszlink,pszinstalldir);
	wcscat(pszlink,TEXT("\\bin\\arm\\runMidlet.exe\" -1 com.bluewhalesystems.midp.Launcher"));
	SHCreateShortcut(pszfilepath, pszlink);
    }

    return codeINSTALL_EXIT_DONE;
}

///////////////////////////////////////////////////////////////

//PURPOSE : HANDLES TASKS DONE AT BEGINNING OF UNINSTALLATION

///////////////////////////////////////////////////////////////

codeUNINSTALL_INIT Uninstall_Init(
  HWND hwndparent,LPCTSTR pszinstalldir)
{
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
