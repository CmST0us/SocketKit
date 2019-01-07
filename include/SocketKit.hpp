#ifndef _SOCKET_KIT_HEADER_
#define _SOCKET_KIT_HEADER_

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
#endif // _WIN32

#if __linux__
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
    bool initialize() {
#if _WIN32
        WSADATA wsaData;
        int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (ret != 0) {
            // error
            perror("WSAStartup Error");
            return false;
        }
#endif
        return true;
    };

    bool relesase() {
#if _WIN32
        int ret = WSACleanup();
        if (ret != 0) {
            perror("WSACleanup Error");
            return false;
        }
#endif
        return true;
    };

}
#endif // !_SOCKET_KIT_HEADER_