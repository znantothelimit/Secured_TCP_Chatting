/*
* TCP 爾寰 瓣た Щ煎斜極 client 模蝶だ橾
* 濛撩濠: 薑爾鱔褐奢з婁 20190895梯雙艙, 薑爾鱔褐奢з婁 20190954 ъ霞��
* 濛撩�秣� : Visual Studio 2022
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


//懍����, 犒���倍� в蹂и 滲熱
extern BYTE ENC[MAXBYTE];
extern BYTE DEC[MAXBYTE];

//瓣た縑 в蹂и 滲熱
int msgcnt = -1;
int* msgcntp = &msgcnt;
int portnum = 9000;
char ip_addr[256] = "127.0.0.1";
char nickname[16] = "\0";;
BYTE input_msg[MAXBYTE];

//base64 檣囀註, 蛤囀註縑 в蹂и л熱 塽 滲熱
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


//蕾樓й 憮幗 IP 塽 PORTNUM 殮溘嫡朝 л熱
int get_server(char ip_addr[]) {
    int portnum2 = 0;
    char in = NULL;

    gotoxy(0, 0);
    printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 3);
    printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 1);
    printf("[default server] IP=127.0.0.1 PORTNUM=9000");
    gotoxy(0, 2);
    printf("熱薑ж溥賊 [m], 斜渠煎 蕾樓ж溥賊 [棻艇 key] 殮溘>> ");
    scanf("%c", &in);
    while (getchar() != '\n');//縛攪酈 殮溘 幗ぷ 薯剪
    if (in == 'm') {
        system("cls");
        gotoxy(0, 0);
        printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
        gotoxy(0, 3);
        printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
        gotoxy(0, 1);
        printf("蕾樓й 憮幗曖 IP 輿模 殮溘>> ");
        gets(ip_addr);
        gotoxy(0, 2);
        printf("蕾樓й 憮幗曖 PORT NUM 殮溘>> ");
        scanf("%d", &portnum2);
        while (getchar() != '\n');//縛攪酈 殮溘 幗ぷ 薯剪
        return portnum2;
    }
    else {
        return 9000;
    }

}


//棣啻歜 殮溘嫡朝 л熱
void getnick() {

    gotoxy(0, 0);
    printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 2);
    printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 1);
    printf("渠�飛� 殮溘(16byte)>>");
    gets_s(nickname, 16 * sizeof(char));
}

//瓣た璽 溯檜嬴醒 轎溘 л熱
void chat_ui() {
    gotoxy(0, 0);
    printf("                                                           ");
    gotoxy(0, 0);
    printf("[TCP client]   [/quit]瓣た 謙猿   [/cls]�飛� 蟾晦�苒n");
    gotoxy(0, 1);
    printf("收收[懍���胝]收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 2);
    printf("                                                            ");
    gotoxy(0, 3);
    printf("                                                            ");
    gotoxy(0, 4);
    printf("收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收");
    gotoxy(0, 5);
    printf("                                                            ");
    gotoxy(0, 2);
    printf("[%s] ", nickname);

}

//瓣た璽 �飛� 贗葬橫
void chatclr() {
    system("cls");
    *msgcntp = -1;
    chat_ui();
}

//wsastartup, socket, connectл熱 んл
int client_init(char* ip, int port)
{
    SOCKET server_socket;
    WSADATA wsadata;
    SOCKADDR_IN server_address = { 0 };

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)//孺樓蟾晦��
    {
        printf("WSAStartup 縑楝\n");
        return -1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)//憮幗模鰍儅撩
    {
        puts("socket 縑楝.");
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    if ((connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))) < 0)//憮幗模鰍縑 翱唸衛紫
    {
        puts("connect 縑楝.");
        return -1;
    }

    return server_socket;
}


//瓣た擊 熱чж朝 л熱
unsigned int WINAPI chat(void* params)
{
    SOCKET s = (SOCKET)params;
    BYTE recv_msg[MAXBYTE] = "\0";
    BYTE buf[MAXBYTE] = "\0";
    int len = 0;
    int index = 0;
    int plain_len = 0;//瞪歎嫡擎 等檜攪曖 望檜 �挫恉炱� 嬪п 摹樹
    int row = 0;//瞪歎嫡擎 等檜攪陛 60濠 晦遽戲煎 賃 還檣雖 �挫恉炱� 嬪п憮 摹樹
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
            if (ev.lNetworkEvents == FD_READ)//read檜漸お 馬雖
            {
                memset(recv_msg, '\0', MAXBYTE);

                int len = recv(s, recv_msg, MAXBYTE, 0);//詭撮雖蒂 熱褐嫡朝 婁薑
                if (len > 0) {
                    if ((*msgcntp) == 40 - 7) {     //贗塭檜樹お曖 瓣た璽檜 粕 獺擊 唳辦 瓣た 頂辨擊 蟾晦�倆炾� 轎溘п輿晦 嬪п 餌辨
                        chatclr();
                    }
                    chat_ui();//夔樂璽縑 瓣た ui蒂 棻衛 轎溘и棻
                    gotoxy(0, (*msgcntp + 7));
                    strcpy(buf, base64_decode(recv_msg, strlen(recv_msg), &decoding_num));  //詭撮雖 熱褐嫡懊擊 唳辦 Decoding 衛ч
                    data_decryption(buf, strlen(buf));                                      //Decoding 檜�� Decryptionж賊 plain data �僱�
                    strcpy(recv_msg, DEC);
                    plain_len = strlen(recv_msg);       //UI曖 橾婦撩擊 蜃蹺晦 嬪п plain data陛 瓣た璽 陛煎 size檣 60濠 檜鼻擊 剩望 唳辦
                    row = (int)(plain_len / 60 + 1);    //檜啪 賃還瞼葬 等檜攪檣雖 row縑 盪濰
                    (*msgcntp) += row;
                    if (((*msgcntp) + row > 34)) {
                        chatclr();
                    }
                    time_t curr = time(NULL);
                    struct tm* d = localtime(&curr);

                    printf("[%02d:%02d]%s", d->tm_hour, d->tm_min, recv_msg);   //timeл熱蒂 餌辨п 憮幗煎睡攪 詭撮雖蒂 瞪歎嫡懊擊 陽曖 衛除 轎溘
                    row = 0;//row蒂 0戲煎 棻衛 蟾晦��
                    chat_ui();
                }

            }
            else if (ev.lNetworkEvents == FD_CLOSE)//close檜漸お 馬雖腎賊 贗塭檜樹お 模鰍 莖堅 憮幗 翱唸 謙猿
            {
                system("cls");
                printf("[TCP server] 憮幗諦曖 翱唸檜 莒橫颶\n");
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
    system("mode con:cols=60 lines=40");//瓣た璽 觼晦 褻瞰, 陛煎*撮煎



    unsigned int tid;
    int sock;
    BYTE send_msg[MAXBYTE] = "\0";
    BYTE buf[MAXBYTE] = "\0";
    HANDLE mainthread;//輿 蝶溯萄 儅撩

    portnum = get_server(ip_addr);//憮幗 薑爾蒂 殮溘嫡擠
    system("cls");
    getnick();  //餌辨濠 渠�飛� 殮溘嫡擠
    sock = client_init(ip_addr, portnum);   //模鰍 儅撩
    if (sock < 0)
    {
        printf("sock_init 縑楝\n");
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

            if (!strcmp(input_msg, "/quit")) {      // quit貲滄橫 殮溘衛 瓣た 謙猿
                break;
            }
            else if (!strcmp(input_msg, "/cls")) {  // cls貲滄橫 殮溘衛 瓣た 頂辨檜 ル衛腎朝 �飛� 蟾晦��
                chatclr();
                continue;
            }
            else if (((*msgcntp) > 34)) {           // 瓣た璽 �飛橉� 粕 獺擊 唳辦, 濠翕戲煎 蟾晦��
                chatclr();
            }
            else if (strlen(input_msg) == 0) {      // 餌辨濠陛 殮溘и 頂辨檜 橈擊 唳辦, 瞪歎ж雖 彊堅 瓣た 欽啗蒂 奩犒
                continue;
            }

            sprintf(send_msg, "[%s] %s", nickname, input_msg);  //爾鳥 詭撮雖縑 棣啻歜婁 爾鳥 頂辨擊 ⑽衝雖薑擊 鱔п憮 盪濰


            //瞪歎擊 嬪п 懍���� 塽 檣囀註
            data_encryption(send_msg, strlen(send_msg));
            strcpy(send_msg, base64_encode(ENC, strlen(ENC), &encoding_num));




            //詭撮雖 瞪歎ж朝 欽啗
            send(sock, send_msg, MAXBYTE, 0);
            gotoxy(0, (*msgcntp + 7));

        }

        closesocket(sock);
        WSACleanup();
        CloseHandle(mainthread);
    }

    return 0;

}




//base64檣囀註 塽 蛤囀註 婦溼 л熱
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