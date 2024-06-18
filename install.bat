@echo on
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

set KEY_NAME="HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
set VALUE_NAME=Path

FOR /F "usebackq skip=4 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v %VALUE_NAME% 2^>nul`) DO (
  set ValueName=%%A
  set ValueValue=%%C
)

if defined ValueName (

  set "newPath=%INSTALL_DIR%;C:\bin"

  reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%newPath%" /f

  set "path=%path%;%INSTALL_DIR%"

) else (
    @echo %KEY_NAME%\%VALUE_NAME% not found.
)

echo Installed creal.
