@echo off

set OLDPATH=%PATH%

set OUTPUT=c:\wince_vm\wm2003_ppc\today_item
set EVC_ROOT=C:\Program Files\Microsoft eMbedded C++ 4.0
set PPC2003_ROOT=C:\Program Files\Windows CE Tools\wce420\POCKET PC 2003
set PATH=%EVC_ROOT%\Common\EVC\bin;%EVC_ROOT%\EVC\wce420\bin;%PATH%

set CL_ARGS=-nologo -c -W0 -D__STDC__ -O1 -Ob1 
set CL_DEFINES=-D_WIN32_WCE=420 -DUNDER_CE=420 -DWINCE -DNDEBUG -D_WINDOWS -DARM -D_ARM -D_ARM_ -DUNICODE -D_UNICODE -DPOCKETPC2003_UI_MODEL -DPOCKETPC2003_UI_MODEL
set CL_INCLUDES=-I"." -I"inc" -I"%PPC2003_ROOT%\Include\Armv4" -I"%PPC2003_ROOT%\Mfc\Include"

set LINK_ARGS=-nologo -entry:DllMain -dll -subsystem:windowsce,4.20 -machine:ARM -incremental:no
set LINK_LIBS=coredll.lib aygshell.lib -nodefaultlib:libc.lib -nodefaultlib:libcd.lib -nodefaultlib:libcmt.lib -nodefaultlib:libcmtd.lib -nodefaultlib:msvcrt.lib -nodefaultlib:msvcrtd.lib -nodefaultlib:oldnames.lib
set LINK_LIBPATH=-libpath:"%PPC2003_ROOT%\Lib\Armv4"

mkdir %OUTPUT%
del %OUTPUT%\*.* /s /q /f

echo Compiling...
CLARM.EXE %CL_ARGS% %CL_DEFINES% %CL_INCLUDES% -Fo"%OUTPUT%\StdAfx.o" StdAfx.cpp
CLARM.EXE %CL_ARGS% %CL_DEFINES% %CL_INCLUDES% -Fo"%OUTPUT%\bluewhale.o" bluewhale.cpp
CLARM.EXE %CL_ARGS% %CL_DEFINES% %CL_INCLUDES% -Fo"%OUTPUT%\setup.o" setup.cpp

echo Compiling resources...
RC.EXE -l 1033 -Fo"%OUTPUT%\todayitem.res" %CL_DEFINES% %CL_INCLUDES% -r todayitem.rc
echo Linking...
LINK.exe %LINK_ARGS% %LINK_LIBPATH% %LINK_LIBS% -def:bluewhale.def -out:%OUTPUT%\bluewhale.dll -implib:%OUTPUT%\bluewhale.lib %OUTPUT%\bluewhale.o %OUTPUT%\StdAfx.o %OUTPUT%\todayitem.res
LINK.exe %LINK_ARGS% %LINK_LIBPATH% %LINK_LIBS% -def:setup.def -out:%OUTPUT%\setup.dll -implib:%OUTPUT%\setup.lib %OUTPUT%\setup.o

echo Creating cab...
copy BlueWhale.inf %OUTPUT%\bluewhale.inf
"%PPC2003_ROOT%\Tools\Cabwiz.exe" %OUTPUT%\bluewhale.inf

mkdir c:\wince_vm\wm2003_ppc\deployed
del c:\wince_vm\wm2003_ppc\deployed\bluewhale_wm2003ppcarm_*.cab /s /q /f
move %OUTPUT%\bluewhale.CAB c:\wince_vm\wm2003_ppc\deployed\bluewhale_wm2003ppcarm_%1.cab

rem copy c:\wince_vm\wm2003_ppc\deployed\bluewhale_wm2003ppcarm_%1.cab bluewhale_wm2003ppcarm_%1.cab

set PATH=%OLDPATH%
