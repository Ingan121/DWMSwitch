@echo off
title DWMSwitch Uninstaller

REM Copy itself to temp directory first to keep the uninstaller running even after the DWMSwitch directory is deleted.
if "%~dp0" == "C:\Program Files\Ingan121\DWMSwitch\" (
    copy %0 %tmp% >nul
    %tmp%\%~nx0
    exit /b
) else (goto NotInDSwDir)

:NotInDSwDir

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
    cd /d "%~dp0"
:--------------------------------------

choice /m "Are you sure you want to uninstall DWMSwitch?"
if errorlevel 2 exit /b

echo.
echo Killing DWMSwitch processes...
taskkill -im DWMServer.exe -f >nul 2>nul
if not errorlevel 1 (echo   DWMServer.exe: Success) else echo   DWMServer.exe: Not running

taskkill -im DWMSwitch.exe -f >nul 2>nul
if not errorlevel 1 (echo   DWMSwitch.exe: Success) else echo   DWMSwitch.exe: Not running

taskkill -im dwmctl.exe -f >nul 2>nul
if not errorlevel 1 (echo   dwmctl.exe: Success) else echo   dwmctl.exe: Not running

echo.
<nul set /p =Restoring the original DWM... 
move /y C:\Windows\System32\DWM\dwm.exe C:\Windows\System32\dwm.exe >nul
if not errorlevel 1 (echo Success) else echo Fail

echo.
echo Removing files...
rmdir /s /q "C:\Program Files\Ingan121\DWMSwitch" >nul 2>nul
if not exist "C:\Program Files\Ingan121\DWMSwitch" (echo   C:\Program Files\Ingan121\DWMSwitch: Success) else echo   C:\Program Files\Ingan121\DWMSwitch: Fail

del C:\Windows\dwmctl.exe >nul 2>nul
if not exist C:\Windows\dwmctl.exe (echo   C:\Windows\dwmctl.exe: Success) else echo   C:\Windows\dwmctl.exe: Fail

rmdir C:\Windows\System32\DWM\ >nul 2>nul
if not exist C:\Windows\System32\DWM (echo   C:\Windows\System32\DWM: Success) else echo   C:\Windows\System32\DWM: Fail

echo.
<nul set /p =Removing autostart... 
reg delete HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run /v DWMSwitch /f >nul 2>nul
if not errorlevel 1 (echo Success) else echo Not registered for autostart

echo.
echo Done!
echo Please sign out and sign in again (or reboot) to complete the installation.

echo.
choice /m "Sign out now? "
if %errorlevel% == 1 (
    logoff
    exit
) else (
    echo Press any key to close this window.
    timeout 20 >nul
)