#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#if _WIN32
#include <WinSock2.h>
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef SOCKET SocketFd;
typedef int ssize_t;
typedef int socklen_t;
#define SHUT_RDWR SD_BOTH

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>
#include <sys/select.h>

typedef int SocketFd;
#endif

namespace socketkit
{
    bool initialize();
    bool release();

}
