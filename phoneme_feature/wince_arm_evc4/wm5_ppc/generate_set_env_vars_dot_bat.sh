# Generate the batch file we're going to call to setup the environment variables for the build

CURRENT_DIR_UNIX=`pwd`
CURRENT_DIR_MIXED=`cygpath -m $CURRENT_DIR_UNIX`
OutputFile=$CURRENT_DIR_MIXED/set_env_vars.bat

rm -rf $OutputFile

#  General stuff

echo "@echo off" >> $OutputFile

echo "set VCVARS="`cygpath -d "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"` >> $OutputFile
echo "call %VCVARS%" >> $OutputFile

echo "call \"%WINDOWS_PLATFORM_SDK%\SetEnv.cmd\" /XP32 /RETAIL" >> $OutputFile

echo "set JDK_DIR="`cygpath -m $JAVA_1_4_2_HOME` >> $OutputFile
echo "set PATH=%JAVA_1_4_2_HOME%\bin;%PATH%" >> $OutputFile

# eVC + SDK

echo "set USE_VS2005=false" >> $OutputFile

echo "set EVC_ROOT="`cygpath -dm "C:\Program Files\Microsoft eMbedded C++ 4.0"` >> $OutputFile
echo "set EVC_COMMON_PATH=%EVC_ROOT%/Common/EVC/bin" >> $OutputFile
echo "set VC_SDK_TOOLS_PATH=%EVC_COMMON_PATH%" >> $OutputFile

echo "set WM5_SDK_INCLUDE="`cygpath -dm "C:\Program Files\Windows CE Tools\wce500\Windows Mobile 5.0 Pocket PC SDK\INCLUDE\ARMV4i"` >> $OutputFile
echo "set WM5_SDK_LIB="`cygpath -dm "C:\Program Files\Windows CE Tools\wce500\Windows Mobile 5.0 Pocket PC SDK\LIB\ARMV4i"` >> $OutputFile

echo "set EVC_ARM_INCLUDE=%WM5_SDK_INCLUDE%" >> $OutputFile
echo "set EVC_ARM_LIB=%WM5_SDK_LIB%" >> $OutputFile
echo "set EVC_ARM_PATH=%EVC_ROOT%/EVC/wce420/bin" >> $OutputFile

echo "Title Windows Mobile 5 Pocket PC SDK - ARM - eVC++ 4.0" >> $OutputFile

