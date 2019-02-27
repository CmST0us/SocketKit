#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <iostream>

#if _WIN32
#include <WinSock2.h>
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef SOCKET SocketFd;
typedef int ssize_t;
typedef int socklen_t;
#define SHUT_RDWR SD_BOTH
#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND

#else
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/un.h>

typedef int SocketFd;
#endif

#ifdef MSG_NOSIGNAL
#define SO_NOSIGPIPE MSG_NOSIGNAL
#endif

#ifdef DEBUG
#define SOCKETKIT_ASSET(x) assert(x)
#else
#define SOCKETKIT_ASSET(x)
#endif

namespace socketkit
{

bool initialize();
bool release();

namespace utils {
void makeSocketNonblock(SocketFd socket);
void makeSocketBlock(SocketFd socket);
};

};
