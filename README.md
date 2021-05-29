# DWMSwitch
Easy DWM switcher without Winlogon suspension

## Installation
1. Build all the four projects with Visual Studio (2019 recommended).
2. Make a directory C:\Windows\System32\DWM and C:\Program Files\Ingan121\DWMSwitch. (These paths are currently hardcoded.)
3. Take ownership and grant permissions of C:\Windows\System32\dwm.exe. (Use ```takeown /a /f dwm.exe && icacls dwm.exe /grant Administrators:F``` in admin cmd.)
4. Move dwm.exe to the C:\Windows\System32\DWM directory.
5. Move the built DWMLoaderStub.exe to C:\Windows\System32\dwm.exe.
6. Move the built DWMCtl.exe to C:\Windows\System32\dwmctl.exe (or wherever in %path%.)
7. Move the built DWMServer.exe and DWMSwitch GUI files to C:\Program Files\Ingan121\DWMSwitch.
8. If you want, register the DWMSwitch GUI to autostart with any means.

I'm planning on releasing prebuilt binaries with a installer.

Note: the DWMSwitch GUI is currently outdated.
