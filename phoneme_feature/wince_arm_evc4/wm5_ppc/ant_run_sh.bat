@echo off

REM cygwin environment
call add_cyg4me_tools_to_path.bat

REM buld environment
call sh generate_set_env_vars_dot_bat.sh
call set_env_vars.bat

REM execute the script
call sh %1

