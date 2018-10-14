//
//  TCPServer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>

#include "TCPServer.hpp"
#include "TCPConnection.hpp"

using namespace ts;

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
        int option = true;
        socklen_t optionLen = sizeof(option);
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optionLen);
        this->mSocket = socket;
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
    ::close(this->mSocket);
    this->mAccpetThread.join();
    return true;
}

void TCPServer::accpetHandle2() {
    std::cout<<"Accept"<<std::endl;
    
    fd_set reads;
    int result;
    struct timeval timeout;
    
    FD_ZERO(&reads);
    FD_SET(this->mSocket, &reads);
    int fd_max = this->mSocket + 1;
    while (1) {
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        result = ::select(fd_max, &reads, 0, 0, &timeout);
        if (result == -1) {
            //server error
            std::cout<<"Server Error"<<std::endl;
            break;
        } else if (result == 0) {
            // timeout
            std::cout<<"timeout retry"<<std::endl;
        } else {
            for (int i = 0; i < fd_max; ++i) {
                if (i == this->mSocket) {
                    // accept
                    struct sockaddr_in acceptSocketAddrIn = {0};
                    socklen_t addrInLen;
                    int acceptSocket = ::accept(this->mSocket, (struct sockaddr *)&acceptSocketAddrIn, &addrInLen);
                    auto client = std::make_shared<TCPConnection>();
                    client->useSocketFd(acceptSocket);
                    client->mSocketAddress.useSockAddrIn(acceptSocketAddrIn);
                    const char *ipString = client->mSocketAddress.getIpString().c_str();
                    if (!this->mDelegate.expired()) {
                        this->mDelegate.lock()->serviceDidReadData((uchar *)ipString, (int)strlen(ipString), client);
                    }
                }
            }
        }
    }
}

void TCPServer::accpetHandle() {
    while (1) {
        // accept
        struct sockaddr_in acceptSocketAddrIn;
        socklen_t addrInLen = sizeof(acceptSocketAddrIn);
        int acceptSocket = ::accept(this->mSocket, (struct sockaddr *)&acceptSocketAddrIn, &addrInLen);
        if (acceptSocket == -1) {
            break;
        }
        auto client = std::make_shared<TCPConnection>();
        client->useSocketFd(acceptSocket);
        client->mSocketAddress.useSockAddrIn(acceptSocketAddrIn);
        const char *ipString = client->mSocketAddress.getIpPortPairString().c_str();
        if (!this->mDelegate.expired()) {
            this->mDelegate.lock()->serviceDidReadData((uchar *)ipString, (int)strlen(ipString), client);
        }
    }
}

