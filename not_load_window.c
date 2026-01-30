#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
int main(int argc, char *argv[]) {
    WSADATA w;
    SOCKET s;
    struct sockaddr_in a;
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
        return 1;
    }
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(8080);
    a.sin_addr.s_addr = inet_addr(argv[1]);
    if (a.sin_addr.s_addr == INADDR_NONE) {
        closesocket(s);
        WSACleanup();
        return 1;
    }
    if (connect(s, (struct sockaddr *)&a, sizeof(a)) == SOCKET_ERROR) {
	closesocket(s);
        WSACleanup();
        return 1;
    }
    if (send(s, "\n", 1, 0) == SOCKET_ERROR) {
	closesocket(s);
        WSACleanup();
        return 1;
    }  
    shutdown(s, SD_SEND);  
    closesocket(s);
    WSACleanup();
    return 0;
}
