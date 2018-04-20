﻿#include <socket.h>
#include <thread.h>

#define _STR_HOST "127.0.0.1:19527"

static void _socket_run(socket_t s) {
    int r, num;
    sockaddr_t addr;
    const char * cots = _STR_HOST;
    int sz = (int)strlen(cots);

    // 开始处理每个监听
    for (num = 12; num < 34; num += 5) {
        socket_t c = socket_accept(s, addr);
        if (INVALID_SOCKET == c) {
            CERR("socket_accept is error");
            break;
        }

        // 简单发送个信息过去
        r = socket_sendn(c, cots, sz);
        if (SOCKET_ERROR == r) {
            socket_close(c);
            CERR("socket_sendn err host = %s", cots);
            break;
        }

        socket_close(c);
    }

    socket_close(s);
}

// 创建一个服务器监听资源
static void _socket_server(void) {
    pthread_t tid;
    const char * host = _STR_HOST;
    socket_t s = socket_tcp(host);
    if (INVALID_SOCKET == s)
        RETNIL("socket_tcp err host = %s", host);

    if (pthread_run(&tid, _socket_run, s)) {
        socket_close(s);
        RETNIL("pthread_run is error");
    }
}

// 开始尝试链接
static int _socket_client(int idx) {
    int r;
    char buf[BUFSIZ];
    const char * host = _STR_HOST;
    socket_t c = socket_connects(host);
    if (INVALID_SOCKET == c)
        return EBase;

    // 开始接收数据
    r = socket_recv(c, buf, sizeof buf - 1);
    if (SOCKET_ERROR == r) {
        RETURN(EFd, "socket_recv err r = %d", r);
    }

    // 输出数据
    buf[r] = '\0';
    printf("%d -> %s\n", idx, buf);

    return SBase;
}

//
// socket test 功能
//
void socket_test(void) {
    int r, i;
    sockaddr_t addr;
    const char * host;

    r = socket_host(host = _STR_HOST, addr);
    if (r < SBase)
        RETNIL("socket_host is err %s", host);

    // 输出 addr
    printf("family = %d, sin_port = %d\n", 
        addr->sin_family, addr->sin_port);

    // 开启 server 
    _socket_server();

    // 开始 client 
    for (i = 1; ; ++i) {
        r = _socket_client(i);
        if (r < SBase)
            break;
    }
}
