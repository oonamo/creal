@echo off

rem define installation directory
set INSTALL_DIR=%USERPROFILE%\creal

if not exist "%INSTALL_DIR%\" (
    echo making folder at "%INSTALL_DIR%"
    mkdir "%INSTALL_DIR%"
)

make build

rem Copy executable
echo Installing at "%INSTALL_DIR%\"
copy /Y "%~dp0creal.exe" "%INSTALL_DIR%\"

rem Add to path
set "INSTALL_DIR=%CURRENT_PATH%;%INSTALL_DIR%"
reg add HKEY_CURRENT_USER\Environment /v Path /t REG_EXPAND_SZ/ d "%INSTALL_DIR%" /f

echo Installed creal.
