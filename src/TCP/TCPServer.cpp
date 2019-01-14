//
//  TCPServer.cpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>

#include "SocketKit.hpp"
#include "TCPServer.hpp"

using namespace socketkit;

TCPServer::TCPServer() {
    this->mSocket = -1;
    this->mSocketAddress.mHostname = "0.0.0.0";
    this->mSocketAddress.mPort = 0;
    this->mSocketAddress.startResolveHost();
}

TCPServer::TCPServer(SocketAddress address) {
    this->mSocket = -1;
    this->mSocketAddress = address;
    this->mSocketAddress.startResolveHost();
}

TCPServer::TCPServer(short port) {
    this->mSocket = -1;
    this->mSocketAddress.mPort = port;
    this->mSocketAddress.mHostname = "0.0.0.0";
    this->mSocketAddress.startResolveHost();
}

TCPServer::~TCPServer() {
    this->mAccpetThread.join();
}

bool TCPServer::createSocket() {
    SocketFd socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket != -1) {
#if _WIN32
        unsigned long ul = 1;
        int ret = ioctlsocket(socket, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式
        if (ret != 0) {
            return false;
        }
#else
        if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
            return false;
        }
#endif

        
        int option = true;
        socklen_t optionLen = sizeof(option);
        
        struct linger l;
        l.l_linger = 0;
        l.l_onoff = 1;
        int intval = 1;
#if _WIN32
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, optionLen);
        setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(struct linger));
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&intval, sizeof(int));
        setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&intval, sizeof(int));
#else
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optionLen);
        setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int));
        setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &intval, sizeof(int));
        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &intval, sizeof(int));
#endif
        
        this->mSocket = socket;
        this->mStatus.isInit = true;
        return true;
    }
    return false;
}

bool TCPServer::bindSocket() {
    struct sockaddr_in socketAddrIn = this->mSocketAddress.getSockaddrIn();
    int ret = ::bind(this->mSocket, (struct sockaddr *)&socketAddrIn, sizeof(socketAddrIn));
    if (ret == 0) {
        return true;
    }
    return false;
}

bool TCPServer::listenSocket() {
    int ret = ::listen(this->mSocket, 20);
    if (ret == 0) {
        return true;
    }
    return false;
}

bool TCPServer::writeData(const uchar *data, int len) {
    return true;
}

bool TCPServer::start() {
    int ret = this->createSocket();
    if (!ret) {
        throw SocketException::socksFdInitError;
    }
    ret = this->bindSocket();
    if (!ret) {
        throw SocketException::socksBindError;
    }
    ret = this->listenSocket();
    if (!ret) {
        throw SocketException::socksListenError;
    }
    
    this->mAccpetThread = std::thread([this]() {
        this->accpetHandle();
    });
    return true;
}

bool TCPServer::pause() {
    return true;
}

bool TCPServer::resume() {
    return true;
}

bool TCPServer::close() {
#if _WIN32
    int ret = ::closesocket(this->mSocket);
#else
    int ret = ::close(this->mSocket);
#endif
    this->mAccpetThread.join();
    return true;
}

void TCPServer::accpetHandle() {
    fd_set reads;
    int result;
    struct timeval timeout;
    
    do {
        FD_ZERO(&reads);
        FD_SET(this->mSocket, &reads);
        SocketFd fd_max = this->mSocket + 1;
        
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        result = ::select((int)fd_max, &reads, 0, 0, &timeout);
        if (result == -1) {
            //server error
            std::cout<<"Server Error"<<std::endl;
            this->willStop = true;
        } else if (result == 0) {
            // timeout
        } else {
            if (FD_ISSET(this->mSocket, &reads)) {
                // accept
                struct sockaddr_in acceptSocketAddrIn;
                socklen_t addrInLen = sizeof(acceptSocketAddrIn);
                SocketFd acceptSocket = ::accept(this->mSocket, (struct sockaddr *)&acceptSocketAddrIn, &addrInLen);
                auto client = std::make_shared<TCPConnection>();
                client->useSocketFd(acceptSocket);
                client->mSocketAddress.useSockAddrIn(acceptSocketAddrIn);
                const char *ipString = client->mSocketAddress.getIpPortPairString().c_str();
                if (!this->mDelegate.expired()) {
                    this->mDelegate.lock()->serviceDidReadData(client->mSocketAddress, (uchar *)ipString, (int)strlen(ipString), client);
                }
            }
        }
    } while (!this->willStop);
}

