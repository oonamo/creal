@echo off
setlocal

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
for /f "tokens=2*" %%a in ('reg query HKCU\Environment /v Path ^| find "Path"') do (
    set "CURRENT_PATH=%%b"
)

rem Check if the NEW_PATH already exists in the current PATH
echo %CURRENT_PATH% | find /I "%NEW_PATH%;" > nul
if errorlevel 1 (
    rem Append the NEW_PATH to the current PATH
    set "INSTALL_DIR=%CURRENT_PATH%;%INSTALL_DIR%"
    rem Update the registry with the new PATH
    reg add HKCU\Environment /v Path /t REG_EXPAND_SZ /d "%INSTALL_DIR%" /f
    rem Display a message indicating success
    echo Install path has been added to the user PATH variable.
) else (
    echo Install path already exists in the user PATH variable.
)

endlocal

echo Installed creal.
