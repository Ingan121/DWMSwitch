# DWMSwitch
Easy DWM switcher without Winlogon suspension

**Project no longer maintained as I'm not using Windows 10 anymore. If you're using Windows 11, you can disable DWM without breaking the mouse functionality by renaming dwminit.dll in system32. (Also rename Windows.UI.Logon.dll to get the logon screen working.) Since it's a dll loaded by Winlogon at logon, you must reboot/relog in order to disable/enable DWM, so this project is no longer applicable. I couldn't find any alternatives at the time of writing, sorry.**

 
**Warning: DO NOT use this in Windows 11. Killing DWM in there will make the mouse completely unusable. Currently no fix is known.**
If you disabled DWM in Windows 11, just press Win+R, type `dwmctl start`, then press enter.

## Manual installation
1. Build all the four projects with Visual Studio (2019 recommended), or download the prebuilt binaries from the releases.
2. Make a directory C:\Windows\System32\DWM and C:\Program Files\Ingan121\DWMSwitch. (These paths are currently hardcoded.)
3. Take ownership and grant permissions of C:\Windows\System32\dwm.exe. (Use ```takeown /a /f dwm.exe && icacls dwm.exe /grant Administrators:F``` in admin cmd.)
4. Move dwm.exe to the C:\Windows\System32\DWM directory.
5. Move the built DWMLoaderStub.exe to C:\Windows\System32\dwm.exe.
6. Move the built DWMCtl.exe to C:\Windows\System32\dwmctl.exe (or wherever in %path%.)
7. Move the built DWMServer.exe and DWMSwitch GUI files to C:\Program Files\Ingan121\DWMSwitch.
8. If you want, register the DWMSwitch GUI to autostart with any means.
