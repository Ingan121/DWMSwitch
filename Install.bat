@echo off
title DWMSwitch Installer
if not exist "%~dp0Files" (
    echo Please uncompress the zip completely then try again.
    pause
    exit /b
)
cd /d "%~dp0Files"

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
    IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrator privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params= %*
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params:"=""%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%cd%"
    cd /d "%~dp0Files"
:--------------------------------------

echo DWMSwitch 0.1.0 by Ingan121
echo Licensed under the MIT License
echo https://github.com/Ingan121/DWMSwitch
echo.
echo ----------------------------------------------------------
echo.
echo Warning: DWMSwitch is not complete yet and not guaranteed to work properly on all devices. Use at your own risk!
echo You can also manually install DWMSwitch with the instructions in README.md.
pause

echo.
echo Creating directories...
mkdir C:\Windows\System32\DWM >nul 2>nul
if exist C:\Windows\System32\DWM (echo   C:\Windows\System32\DWM: Success) else echo   C:\Windows\System32\DWM: Fail
mkdir "C:\Program Files\Ingan121" >nul 2>nul
if exist "C:\Program Files\Ingan121" (echo   C:\Program Files\Ingan121: Success) else echo   C:\Program Files\Ingan121: Fail
mkdir "C:\Program Files\Ingan121\DWMSwitch" >nul 2>nul
if exist "C:\Program Files\Ingan121\DWMSwitch" (echo   C:\Program Files\Ingan121\DWMSwitch: Success) else echo   C:\Program Files\Ingan121\DWMSwitch: Fail

if not exist C:\Windows\System32\DWM\dwm.exe (
    echo.
    <nul set /p =Backing up the original DWM... 
    copy C:\Windows\System32\dwm.exe C:\Windows\System32\dwm.orig-%random%.exe >nul
    if not errorlevel 1 (echo Success) else echo Fail

    echo.
    <nul set /p =Moving the original DWM... 
    takeown /a /f C:\Windows\System32\dwm.exe >nul
    icacls C:\Windows\System32\dwm.exe /grant Administrators:F >nul
    move C:\Windows\System32\dwm.exe C:\Windows\System32\DWM\dwm.exe >nul
    if not errorlevel 1 (echo Success) else echo Fail
)

echo.
echo Copying files...
copy DWMServer.exe "C:\Program Files\Ingan121\DWMSwitch" /y >nul
if not errorlevel 1 (echo   DWMServer.exe: Success) else echo   DWMServer.exe: Fail

copy DWMSwitch\DWMSwitch.exe "C:\Program Files\Ingan121\DWMSwitch" /y >nul
if not errorlevel 1 (echo   DWMSwitch.exe: Success) else echo   DWMSwitch.exe: Fail

copy DWMSwitch\DWMSwitch.exe.config "C:\Program Files\Ingan121\DWMSwitch" /y >nul
if not errorlevel 1 (echo   DWMSwitch.exe.config: Success) else echo   DWMSwitch.exe.config: Fail

copy DWMCtl.exe C:\Windows\dwmctl.exe /y >nul
if not errorlevel 1 (echo   dwmctl.exe: Success) else echo   DWMCtl.exe: Fail

copy DWMLoaderStub.exe C:\Windows\System32\dwm.exe /y >nul
if not errorlevel 1 (echo   DWMLoaderStub: Success) else echo   DWMLoaderStub: Fail

echo.
choice /m "Do you want to automatically start DWMSwitch GUI on startup?"
if %errorlevel% == 1 (
    <nul set /p =Registering autostart... 
    reg add HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run /v DWMSwitch /t REG_SZ /d "C:\Program Files\Ingan121\DWMSwitch\DWMSwitch.exe" /f >nul
    if not errorlevel 1 (echo Success) else echo Fail
)

echo.
echo Done!
echo Please sign out and sign in again (or reboot) to complete the installation.
echo Press any key to close this window.
timeout 20 >nul