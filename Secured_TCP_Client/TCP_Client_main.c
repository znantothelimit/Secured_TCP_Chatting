/*
* TCP 보안 채팅 프로그램 client 소스파일
* 작성자: 정보통신공학과 20190895김찬영, 정보통신공학과 20190954 허진환
* 작성환경 : Visual Studio 2022
*/

#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <WinSock2.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>


//암호화, 복호화에 필요한 변수
extern BYTE ENC[MAXBYTE];
extern BYTE DEC[MAXBYTE];

//채팅에 필요한 변수
int msgcnt = -1;
int* msgcntp = &msgcnt;
int portnum = 9000;
char ip_addr[256] = "127.0.0.1";
char nickname[16] = "\0";;
BYTE input_msg[MAXBYTE];

//base64 인코딩, 디코딩에 필요한 함수 및 변수
unsigned char* base64_encode(const unsigned char* str, int length, int* ret_length);
unsigned char* base64_decode(const unsigned char* str, int length, int* ret_length);
int encoding_num = 0;
int decoding_num = 1;
static char base64_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};
static char base64_pad = '=';

void gotoxy(int x, int y) {
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


//접속할 서버 IP 및 PORTNUM 입력받는 함수
int get_server(char ip_addr[]) {
    int portnum2 = 0;
    char in = NULL;

    gotoxy(0, 0);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 3);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 1);
    printf("[default server] IP=127.0.0.1 PORTNUM=9000");
    gotoxy(0, 2);
    printf("수정하려면 [m], 그대로 접속하려면 [다른 key] 입력>> ");
    scanf("%c", &in);
    while (getchar() != '\n');//엔터키 입력 버퍼 제거
    if (in == 'm') {
        system("cls");
        gotoxy(0, 0);
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        gotoxy(0, 3);
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        gotoxy(0, 1);
        printf("접속할 서버의 IP 주소 입력>> ");
        gets(ip_addr);
        gotoxy(0, 2);
        printf("접속할 서버의 PORT NUM 입력>> ");
        scanf("%d", &portnum2);
        while (getchar() != '\n');//엔터키 입력 버퍼 제거
        return portnum2;
    }
    else {
        return 9000;
    }

}


//닉네임 입력받는 함수
void getnick() {

    gotoxy(0, 0);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 2);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 1);
    printf("대화명 입력(16byte)>>");
    gets_s(nickname, 16 * sizeof(char));
}

//채팅창 레이아웃 출력 함수
void chat_ui() {
    gotoxy(0, 0);
    printf("                                                           ");
    gotoxy(0, 0);
    printf("[TCP client]   [/quit]채팅 종료   [/cls]화면 초기화\n");
    gotoxy(0, 1);
    printf("━━[암호화O]━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 2);
    printf("                                                            ");
    gotoxy(0, 3);
    printf("                                                            ");
    gotoxy(0, 4);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    gotoxy(0, 5);
    printf("                                                            ");
    gotoxy(0, 2);
    printf("[%s] ", nickname);

}

//채팅창 화면 클리어
void chatclr() {
    system("cls");
    *msgcntp = -1;
    chat_ui();
}

//wsastartup, socket, connect함수 포함
int client_init(char* ip, int port)
{
    SOCKET server_socket;
    WSADATA wsadata;
    SOCKADDR_IN server_address = { 0 };

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)//윈속초기화
    {
        printf("WSAStartup 에러\n");
        return -1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)//서버소켓생성
    {
        puts("socket 에러.");
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    if ((connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))) < 0)//서버소켓에 연결시도
    {
        puts("connect 에러.");
        return -1;
    }

    return server_socket;
}


//채팅을 수행하는 함수
unsigned int WINAPI chat(void* params)
{
    SOCKET s = (SOCKET)params;
    BYTE recv_msg[MAXBYTE] = "\0";
    BYTE buf[MAXBYTE] = "\0";
    int len = 0;
    int index = 0;
    int plain_len = 0;//전송받은 데이터의 길이 확인하기 위해 선언
    int row = 0;//전송받은 데이터가 60자 기준으로 몇 줄인지 확인하기 위해서 선언
    WSANETWORKEVENTS ev;
    HANDLE event = WSACreateEvent();


    ////////////FD_READ, FD_CLOSE
    WSAEventSelect(s, event, FD_READ | FD_CLOSE);
    while (1)
    {

        index = WSAWaitForMultipleEvents(1, &event, FALSE, INFINITE, FALSE);
        if ((index != WSA_WAIT_FAILED) && (index != WSA_WAIT_TIMEOUT))
        {
            WSAEnumNetworkEvents(s, event, &ev);
            if (ev.lNetworkEvents == FD_READ)//read이벤트 감지
            {
                memset(recv_msg, '\0', MAXBYTE);

                int len = recv(s, recv_msg, MAXBYTE, 0);//메세지를 수신받는 과정
                if (len > 0) {
                    if ((*msgcntp) == 40 - 7) {     //클라이언트의 채팅창이 꽉 찼을 경우 채팅 내용을 초기화하고 출력해주기 위해 사용
                        chatclr();
                    }
                    chat_ui();//콘솔창에 채팅 ui를 다시 출력한다
                    gotoxy(0, (*msgcntp + 7));
                    strcpy(buf, base64_decode(recv_msg, strlen(recv_msg), &decoding_num));  //메세지 수신받았을 경우 Decoding 시행
                    data_decryption(buf, strlen(buf));                                      //Decoding 이후 Decryption하면 plain data 획득
                    strcpy(recv_msg, DEC);
                    plain_len = strlen(recv_msg);       //UI의 일관성을 맞추기 위해 plain data가 채팅창 가로 size인 60자 이상을 넘길 경우
                    row = (int)(plain_len / 60 + 1);    //이게 몇줄짜리 데이터인지 row에 저장
                    (*msgcntp) += row;
                    if (((*msgcntp) + row > 34)) {
                        chatclr();
                    }
                    time_t curr = time(NULL);
                    struct tm* d = localtime(&curr);

                    printf("[%02d:%02d]%s", d->tm_hour, d->tm_min, recv_msg);   //time함수를 사용해 서버로부터 메세지를 전송받았을 때의 시간 출력
                    row = 0;//row를 0으로 다시 초기화
                    chat_ui();
                }

            }
            else if (ev.lNetworkEvents == FD_CLOSE)//close이벤트 감지되면 클라이언트 소켓 끄고 서버 연결 종료
            {
                system("cls");
                printf("[TCP server] 서버와의 연결이 끊어짐\n");
                closesocket(s);
                break;
            }

        }
    }
    WSACleanup();
    _endthreadex(0);

    return 0;
}





int main()

{
    system("mode con:cols=60 lines=40");//채팅창 크기 조절, 가로*세로



    unsigned int tid;
    int sock;
    BYTE send_msg[MAXBYTE] = "\0";
    BYTE buf[MAXBYTE] = "\0";
    HANDLE mainthread;//주 스레드 생성

    portnum = get_server(ip_addr);//서버 정보를 입력받음
    system("cls");
    getnick();  //사용자 대화명 입력받음
    sock = client_init(ip_addr, portnum);   //소켓 생성
    if (sock < 0)
    {
        printf("sock_init 에러\n");
        exit(0);
    }

    mainthread = (HANDLE)_beginthreadex(NULL, 0, chat, (void*)sock, 0, &tid);
    if (mainthread)
    {
        chatclr();
        while (1)
        {
            memset(input_msg, '\0', MAXBYTE);
            memset(send_msg, '\0', MAXBYTE);
            memset(buf, '\0', MAXBYTE);


            chat_ui();
            gets_s(input_msg, MAXBYTE);

            if (!strcmp(input_msg, "/quit")) {      // quit명령어 입력시 채팅 종료
                break;
            }
            else if (!strcmp(input_msg, "/cls")) {  // cls명령어 입력시 채팅 내용이 표시되는 화면 초기화
                chatclr();
                continue;
            }
            else if (((*msgcntp) > 34)) {           // 채팅창 화면이 꽉 찼을 경우, 자동으로 초기화
                chatclr();
            }
            else if (strlen(input_msg) == 0) {      // 사용자가 입력한 내용이 없을 경우, 전송하지 않고 채팅 단계를 반복
                continue;
            }

            sprintf(send_msg, "[%s] %s", nickname, input_msg);  //보낼 메세지에 닉네임과 보낼 내용을 형식지정을 통해서 저장


            //전송을 위해 암호화 및 인코딩
            data_encryption(send_msg, strlen(send_msg));
            strcpy(send_msg, base64_encode(ENC, strlen(ENC), &encoding_num));




            //메세지 전송하는 단계
            send(sock, send_msg, MAXBYTE, 0);
            gotoxy(0, (*msgcntp + 7));

        }

        closesocket(sock);
        WSACleanup();
        CloseHandle(mainthread);
    }

    return 0;

}




//base64인코딩 및 디코딩 관련 함수
unsigned char* base64_encode(const unsigned char* str, int length, int* ret_length) {
    const unsigned char* current = str;
    int i = 0;
    unsigned char* result = (unsigned char*)malloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));
    while (length > 2) { /* keep going until we have less than 24 bits */
        result[i++] = base64_table[current[0] >> 2];
        result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        result[i++] = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        result[i++] = base64_table[current[2] & 0x3f];
        current += 3;
        length -= 3; /* we just handle 3 octets of data */
    }
    /* now deal with the tail end of things */
    if (length != 0) {
        result[i++] = base64_table[current[0] >> 2];
        if (length > 1) {
            result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            result[i++] = base64_table[(current[1] & 0x0f) << 2];
            result[i++] = base64_pad;
        }
        else {
            result[i++] = base64_table[(current[0] & 0x03) << 4];
            result[i++] = base64_pad;
            result[i++] = base64_pad;
        }
    }
    if (ret_length) {
        *ret_length = i;
    }
    result[i] = '\0';
    return result;
}

/* as above, but backwards. :) */
unsigned char* base64_decode(const unsigned char* str, int length, int* ret_length) {
    const unsigned char* current = str;
    int ch, i = 0, j = 0, k;
    /* this sucks for threaded environments */
    static short reverse_table[256];
    static int table_built;
    unsigned char* result;
    if (++table_built == 1) {
        char* chp;
        for (ch = 0; ch < 256; ch++) {
            chp = strchr(base64_table, ch);
            if (chp) {
                reverse_table[ch] = chp - base64_table;
            }
            else {
                reverse_table[ch] = -1;
            }
        }
    }
    result = (unsigned char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }
    /* run through the whole string, converting as we go */
    while ((ch = *current++) != '\0') {
        if (ch == base64_pad) break;
        /* When Base64 gets POSTed, all pluses are interpreted as spaces.
        This line changes them back.  It's not exactly the Base64 spec,
        but it is completely compatible with it (the spec says that
        spaces are invalid).  This will also save many people considerable
        headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
        */
        if (ch == ' ') ch = '+';
        ch = reverse_table[ch];
        if (ch < 0) continue;
        switch (i % 4) {
        case 0:
            result[j] = ch << 2;
            break;
        case 1:
            result[j++] |= ch >> 4;
            result[j] = (ch & 0x0f) << 4;
            break;
        case 2:
            result[j++] |= ch >> 2;
            result[j] = (ch & 0x03) << 6;
            break;
        case 3:
            result[j++] |= ch;
            break;
        }
        i++;
    }
    k = j;
    /* mop things up if we ended on a boundary */
    if (ch == base64_pad) {
        switch (i % 4) {
        case 0:
        case 1:
            free(result);
            return NULL;
        case 2:
            k++;
        case 3:
            result[k++] = 0;
        }
    }
    if (ret_length) {
        *ret_length = j;
    }
    result[k] = '\0';
    return result;
}