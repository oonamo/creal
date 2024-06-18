@echo off
setlocal

set INSTALL_DIR=%USERPROFILE%\creal
echo "%~dp0creal.exe"
mkdir "%~dp0\empty"
echo "%INSTALL_DIR%"

if not exist "%INSTALL_DIR%\" (
    echo making folder at "%INSTALL_DIR%"
    mkdir "%INSTALL_DIR%"
)

if not exists "%~dp0creal.exe" make build

echo Installing at "%INSTALL_DIR%\"
copy /Y "%~dp0creal.exe" "%INSTALL_DIR%\"
