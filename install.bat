@echo off
setlocal

rem define installation directory
set INSTALL_DIR=%APPDATA%\creal

if not exist "%INSTALL_DIR%\" (
    echo making folder at "%INSTALL_DIR%"
    mkdir "%INSTALL_DIR%"
)

rem Copy executable
echo Installing at "%INSTALL_DIR%"
move /Y "%~dp0\creal.exe" "%INSTALL_DIR%"

rem Add to path
setx PATH "%PATH%;%INSTALL_DIR%"
set "PATH=%PATH%;%INSALL_DIR%"

echo Installed creal.
