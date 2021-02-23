#include <iostream>
#include <string>

// 소켓을 사용하기 위해서 라이브러리 참조해야 한다.	
#pragma comment(lib, "ws2_32")	
// inet_ntoa가 deprecated가 되었는데.. 사용하려면 아래 설정을 해야 한다.	
#pragma warning(disable:4996)	
#include <stdio.h>	
#include <iostream>	
#include <vector>	
#include <thread>	
// 소켓을 사용하기 위한 라이브러리	
#include <WinSock2.h>	
// 수신 버퍼 사이즈	
#define BUFFERSIZE 1024	
using namespace std;
// 콘솔에 메시지를 출력하는 함수	
void print(vector<wchar_t>* str)
{
    // 포인트 위치	
    int p = 0;
    // 버퍼 설정. +1은 \0를 넣기 위한 크기	
    wchar_t out[BUFFERSIZE + 2];
    // 콘솔 출력	
    cout << "From server message : ";

    for (int n = 0; n < (str->size() / BUFFERSIZE) + 1; n++)
    {
        // 버퍼 사이즈 설정	
        int size = str->size();
        // 수신 데이터가 버퍼 사이즈를 넘었을 경우.	
        if (size > BUFFERSIZE) {
            if (str->size() < (n + 1) * BUFFERSIZE)
            {
                size = str->size() % BUFFERSIZE;
            }
            else
            {
                size = BUFFERSIZE;
            }
        }
        // echo 메시지와 콘솔 메시지를 작성한다.	
        for (int i = 0; i < size; i++, p++)
        {
            out[i] = *(str->begin() + p);
        }
        // 콘솔 메시지 콘솔 출력.	
        wprintf(L"%s", out);
        //cout << out;	
    }
}

int main(int argc, char** argv)
{
    string switcherPath;
    if (argc > 1)
    {
        if (strcmp(argv[1], "socktest") == 0)
        {
            // 소켓 정보 데이터 설정	
            WSADATA wsaData;
            // 소켓 실행.	
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                return 1;
            }
            // Internet의 Stream 방식으로 소켓 생성 	
            SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
            // 소켓 주소 설정	
            SOCKADDR_IN addr;
            // 구조체 초기화	
            memset(&addr, 0, sizeof(addr));
            // 소켓은 Internet 타입	
            addr.sin_family = AF_INET;
            // 127.0.0.1(localhost)로 접속하기	
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            // 포트 9090으로 접속	
            addr.sin_port = htons(9090);
            // 접속	
            if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
            {
                // 에러 콘솔 출력	
                cout << "error" << endl;
                return 1;
            }
            // telent은 한글자씩 데이터가 오기 때문에 글자를 모을 buffer가 필요하다.(wchar_t 타입으로 선언)	
            vector<wchar_t> buffer;
            // 수신 데이터	
            wchar_t x;
            while (1)
            {
                // 데이터를 받는다. 에러가 발생하면 멈춘다.	
                // char* 형식으로 받기 때문에 타입 캐스팅을 한다.	
                if (recv(sock, (char*)&x, sizeof(x), 0) == SOCKET_ERROR)
                {
                    // 에러 콘솔 출력	
                    cout << "error" << endl;
                    break;
                }
                // 버퍼에 글자를 하나 넣는다.	
                buffer.push_back(x);
                // \r\n>\0가 나오면 콘솔에 출력하고 콘솔로 부터 메시지를 기다린다.	
                if (buffer.size() > 4 && *(buffer.end() - 4) == '\r' && *(buffer.end() - 3) == '\n' && *(buffer.end() - 2) == '>' && *(buffer.end() - 1) == '\0')
                {
                    // 메시지 출력	
                    print(&buffer);
                    // 버퍼 초기화	
                    buffer.clear();
                    // 콘솔로 부터 입력을 받는다.	
                    char input[BUFFERSIZE];
                    // 유저로 부터 입력 받기	
                    cin >> input;
                    // 입력받은 길이를 받는다.	
                    int size = strlen(input);
                    // 개행을 넣는다.	
                    wchar_t buffer[BUFFERSIZE * 2];
                    // char*에서 wchar_t*으로 변환하는 함수	
                    mbstowcs(&buffer[0], input, BUFFERSIZE * 2);
                    *(buffer + size) = '\r';
                    *(buffer + size + 1) = '\n';
                    *(buffer + size + 2) = '\0';
                    // 서버로 보내기	
                    // send함수가 char* 형식으로 보낼 수 있기 때문에 타입 캐스팅을 한다.	
                    send(sock, (char*)buffer, wcslen(buffer) * 2, 0);
                    // 메시지가 exit라면 종료	
                    if (*input == 'e' && *(input + 1) == 'x' && *(input + 2) == 'i' && *(input + 3) == 't')
                    {
                        break;
                    }
                    continue;
                }
            }
            // 서버 소켓 종료	
            closesocket(sock);
            // 소켓 종료	
            WSACleanup();
            return 0;
        }

        if (strcmp(argv[1], "switch") == 0)
        {
            // 소켓 정보 데이터 설정	
            WSADATA wsaData;
            // 소켓 실행.	
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                return 1;
            }
            // Internet의 Stream 방식으로 소켓 생성 	
            SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
            // 소켓 주소 설정	
            SOCKADDR_IN addr;
            // 구조체 초기화	
            memset(&addr, 0, sizeof(addr));
            // 소켓은 Internet 타입	
            addr.sin_family = AF_INET;
            // 127.0.0.1(localhost)로 접속하기	
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            // 포트 9090으로 접속	
            addr.sin_port = htons(9090);
            // 접속	
            if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
            {
                // 에러 콘솔 출력	
                cout << "error" << endl;
                return 1;
            }
            // telent은 한글자씩 데이터가 오기 때문에 글자를 모을 buffer가 필요하다.(wchar_t 타입으로 선언)	
            vector<wchar_t> buffer;
            // 수신 데이터	
            wchar_t x;
            for (int i = 0; i <= 18; i++)
            {
                // 데이터를 받는다. 에러가 발생하면 멈춘다.	
                // char* 형식으로 받기 때문에 타입 캐스팅을 한다.	
                if (recv(sock, (char*)&x, sizeof(x), 0) == SOCKET_ERROR)
                {
                    // 에러 콘솔 출력	
                    cout << "error" << endl;
                    break;
                }
                // 버퍼에 글자를 하나 넣는다.	
                buffer.push_back(x);
                // \r\n>\0가 나오면 콘솔에 출력하고 콘솔로 부터 메시지를 기다린다.	
                if (buffer.size() > 4 && *(buffer.end() - 4) == '\r' && *(buffer.end() - 3) == '\n' && *(buffer.end() - 2) == '>' && *(buffer.end() - 1) == '\0')
                {
                    // 메시지 출력	
                    //print(&buffer);
                    // 버퍼 초기화	
                    buffer.clear();
                    // 콘솔로 부터 입력을 받는다.	
                    char input[BUFFERSIZE];
                    // 유저로 부터 입력 받기	
                    input[0] = '.';
                    // 입력받은 길이를 받는다.	
                    int size = strlen(input);
                    // 개행을 넣는다.	
                    wchar_t buffer[BUFFERSIZE * 2];
                    // char*에서 wchar_t*으로 변환하는 함수	
                    mbstowcs(&buffer[0], input, BUFFERSIZE * 2);
                    *(buffer + size) = '\r';
                    *(buffer + size + 1) = '\n';
                    *(buffer + size + 2) = '\0';
                    // 서버로 보내기	
                    // send함수가 char* 형식으로 보낼 수 있기 때문에 타입 캐스팅을 한다.	
                    send(sock, (char*)buffer, wcslen(buffer) * 2, 0);
                    // 메시지가 exit라면 종료	
                    if (*input == 'e' && *(input + 1) == 'x' && *(input + 2) == 'i' && *(input + 3) == 't')
                    {
                        break;
                    }
                    continue;
                }
            }
            // 서버 소켓 종료	
            closesocket(sock);
            // 소켓 종료	
            WSACleanup();
            return 0;
        }

        /*
        if (strcmp(argv[1], "/username") == 0)
        {
            char const* val = getenv("username");
            printf((val == NULL) ? std::string().c_str() : std::string(val).c_str());
            if (strstr(val, "DWM"))
            {
                printf("e");
            }
            else
{
                printf("d");
            }
            return 0;
        }
        */

        if (strcmp(argv[1], "/v") == 0)
        {
            printf("0.1.0");
            return 0;
        }
        switcherPath = argv[1];
    }
    else {
        switcherPath = "\"C:\\Program Files\\Ingan121\\DWMSwitch\\DWMSwitch.exe\"";
    }

    // Start original DWM and DWMSWitcher, then exit
    printf("Starting dwm.orig.exe...\n");
    system("start C:\\Windows\\System32\\dwm.orig.exe");

    printf("Starting DWMSwitcher from ");
    printf(switcherPath.c_str());
    printf("...\n");
    switcherPath = "start \"\" " + switcherPath;
    system(switcherPath.c_str());
    
    return 0;
}