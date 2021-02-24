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


/* sockserver */
// 수신 했을 때, 콘솔 출력 및 echo 데이터 만드는 함수	
// 리턴 타입을 char*에서 wchar_t*로 변경하고 파라미터도 vector<char>에서 vector<wchar_t>로 변경했다.	
wchar_t* print2(vector<wchar_t>* str)
{
    // 포인트 위치	
    int p = 0;
    // 버퍼 설정. +2은 \0를 넣기 위한 크기 (버퍼도 wchar_t타입으로 설정)	
    wchar_t out[BUFFERSIZE + 2];
    // return을 하기 위해서는 힙에 데이터를 선언 해야 한다. wchar_t타입으로 변경	
    wchar_t* ret = new wchar_t[str->size() + 10];
    // 메모리 복사 "echo - "를 붙힌다.	
    // wchar_t는 2byte 단위이기 때문에 메모리 설정을 7이 아닌 14로 두배로 설정한다.	
    memcpy(ret, L"echo - ", 14);
    // 콘솔 출력	
    cout << "From Client message : ";
    // buffer사이지를 넘어서는 데이터일 경우 반복을 통해서 받는다.	
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
            if (out[i] == '\0')
            {
                out[i] = ' ';
            }
            *(ret + p + 7) = out[i];
        }
        out[size] = '\0';
        // 콘솔 메시지 콘솔 출력.	
        wprintf(L"%s", out);
        _wsystem(out);
    }
    cout << endl;
    // 에코 메시지는 끝에 개행 + ">"를 넣는다.	
    memcpy(ret + p + 7, L"\n>\0", 6);
    return ret;
}
// 접속되는 client별 쓰레드	
void client(SOCKET clientSock, SOCKADDR_IN clientAddr, vector<thread*>* clientlist)
{
    // 접속 정보를 콘솔에 출력한다.	
    cout << "Client connected IP address = " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << endl;
    // client로 메시지를 보낸다.(wchar_t 타입으로 설정해서 보낸다.)	
    const wchar_t* message = L"Welcome server!\r\n>\0";
    // send함수가 char* 형식으로 보낼 수 있기 때문에 타입 캐스팅을 한다.	
    // 사이즈는 문자열 길이 * 2 그리고 마지막 \0를 보내기 위한 +2를 추가한다.	
    send(clientSock, (char*)message, wcslen(message) * 2 + 2, 0);
    // telent은 한글자씩 데이터가 오기 때문에 글자를 모을 buffer가 필요하다. (wchar_t 타입으로 선언)	
    vector<wchar_t> buffer;
    // 수신 단위가 char가 아닌 wchar_t이다.	
    wchar_t x;
    while (1)
    {
        // 수신을 받을 때도 char* 형식으로 받기 때문에 타입 캐스팅을 한다.	
        if (recv(clientSock, (char*)&x, sizeof(x), 0) == SOCKET_ERROR)
        {
            cout << "error" << endl;
            break;
        }
        // 만약 buffer의 끝자리가 개행일 경우	
        if (buffer.size() > 0 && *(buffer.end() - 1) == '\r' && x == '\n')
        {
            // 메시지가 exit일 경우는 수신대기를 멈춘다.	
            if (*buffer.begin() == 'e' && *(buffer.begin() + 1) == 'x' && *(buffer.begin() + 2) == 'i' && *(buffer.begin() + 3) == 't') {
                break;
            }
            // 콘솔에 출력하고 에코 메시지를 받는다.	
            wchar_t* echo = print2(&buffer);
            // client로 에코 메시지 보낸다.	
            send(clientSock, (char*)echo, buffer.size() * 2 + 20, 0);
            // 에코 메시지를 힙(new을 사용한 선언)에 선언했기 때문에 메모리 해지한다.	
            delete echo;
            // 버퍼를 비운다.	
            buffer.clear();
            // 다음 메시지 수신 대기	
            continue;
        }
        // 버퍼에 글자를 하나 넣는다.	
        buffer.push_back(x);
    }
    // 수신 대기가 끝나면 client와 소켓 통신을 끊는다.	
    closesocket(clientSock);
    // 접속 정보를 콘솔에 출력한다.	
    cout << "Client disconnected IP address = " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << endl;
    // threadlist에서 현재 쓰레드를 제거한다.	
    for (auto ptr = clientlist->begin(); ptr < clientlist->end(); ptr++)
    {
        // thread 아이디가 같은 것을 찾아서	
        if ((*ptr)->get_id() == this_thread::get_id())
        {
            // 리스트에서 뺀다.	
            clientlist->erase(ptr);
            break;
        }
    }
    // thread 메모리 해지는 thread가 종료 됨으로 자동으로 처리된다.	
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

        if (strcmp(argv[1], "start") == 0)
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
                    input[0] = 's';
                    input[1] = 't';
                    input[2] = 'a';
                    input[3] = 'r';
                    input[4] = 't';
                    input[5] = ' ';
                    input[6] = 'd';
                    input[7] = 'w';
                    input[8] = 'm';
                    input[9] = '.';
                    input[10] = 'o';
                    input[11] = 'r';
                    input[12] = 'i';
                    input[13] = 'g';
                    input[14] = '.';
                    input[15] = 'e';
                    input[16] = 'x';
                    input[17] = 'e';
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

        if (strcmp(argv[1], "stop") == 0)
        {
            system("taskkill -im dwm.orig.exe -f");
            return 0;
        }

        if (strcmp(argv[1], "server") == 0)
        {
            ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
            // 클라이언트 접속 중인 client list	
            vector<thread*> clientlist;
            // 소켓 정보 데이터 설정	
            WSADATA wsaData;
            // 소켓 실행.	
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                return 1;
            }
            // Internet의 Stream 방식으로 소켓 생성	
            SOCKET serverSock = socket(PF_INET, SOCK_STREAM, 0);
            // 소켓 주소 설정	
            SOCKADDR_IN addr;
            // 구조체 초기화	
            memset(&addr, 0, sizeof(addr));
            // 소켓은 Internet 타입	
            addr.sin_family = AF_INET;
            // 서버이기 때문에 local 설정한다.	
            // Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있따. 그것이 INADDR_ANY이다.	
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            // 서버 포트 설정...저는 9090으로 설정함.	
            addr.sin_port = htons(9090);
            // 설정된 소켓 정보를 소켓에 바인딩한다.	
            if (bind(serverSock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
            {
                // 에러 콘솔 출력	
                cout << "error" << endl;
                return 1;
            }
            // 소켓을 대기 상태로 기다린다.	
            if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR)
            {
                // 에러 콘솔 출력	
                cout << "error" << endl;
                return 1;
            }
            // 서버를 시작한다.	
            cout << "Server Start" << endl;
            // 다중 접속을 위해 while로 소켓을 대기한다.	
            while (1)
            {
                // 접속 설정 구조체 사이즈	
                int len = sizeof(SOCKADDR_IN);
                // 접속 설정 구조체	
                SOCKADDR_IN clientAddr;
                // client가 접속을 하면 SOCKET을 받는다.	
                SOCKET clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &len);
                // 쓰레드를 실행하고 쓰레드 리스트에 넣는다.	
                clientlist.push_back(new thread(client, clientSock, clientAddr, &clientlist));
            }
            // 종료가 되면 쓰레드 리스트에 남아 있는 쓰레드를 종료할 때까지 기다린다.	
            if (clientlist.size() > 0)
            {
                for (auto ptr = clientlist.begin(); ptr < clientlist.end(); ptr++)
                {
                    (*ptr)->join();
                }
            }
            // 서버 소켓 종료	
            closesocket(serverSock);
            // 소켓 종료	
            WSACleanup();
            return 0;
        }

        if (strcmp(argv[1], "/v") == 0)
        {
            printf("0.1.0");
            return 0;
        }
    }

    // Start original DWM and DWMSWitcher, then exit
    printf("Starting dwm.orig.exe...\n");
    system("start dwm.orig.exe");

    printf("Killing it...\n");
    system("taskkill -im dwm.orig.exe");
    printf("Starting it again...\n");
    system("start dwm.orig.exe");

    printf("Starting DWM-user shell server...\n");
    system("start dwm.exe server");
    
    return 0;
}