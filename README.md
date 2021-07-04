# DWMSwitch
Easy DWM switcher without Winlogon suspension

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
