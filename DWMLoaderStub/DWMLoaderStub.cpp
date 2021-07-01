#include <iostream>

int main()
{
    // Start original DWM and DWMServer, then exit
    system("start C:\\Windows\\System32\\DWM\\dwm.exe");
    printf("Started DWM\n");
    
    system("start \"\" \"C:\\Program Files\\Ingan121\\DWMSwitch\\DWMServer.exe\"");
    printf("Started DWMServer\n");

    return 0;
}