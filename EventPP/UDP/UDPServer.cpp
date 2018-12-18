//
//  UDPServer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>
#include <sys/fcntl.h>
#include "UDPServer.hpp"

using namespace ts;

UDPServer::UDPServer() {
    this->mSocket = -1;
    this->mSocketAddress.mHostname = "0.0.0.0";
    this->mSocketAddress.mPort = 0;
    this->mSocketAddress.startResolveHost();
}

UDPServer::UDPServer(SocketAddress address) {
    this->mSocket = -1;
    this->mSocketAddress = address;
    this->mSocketAddress.startResolveHost();
}

UDPServer::UDPServer(short port) {
    this->mSocket = -1;
    this->mSocketAddress.mPort = port;
    this->mSocketAddress.mHostname = "0.0.0.0";
    this->mSocketAddress.startResolveHost();
}

UDPServer::~UDPServer() {
    this->mRecvThread.join();
}

bool UDPServer::createSocket() {
    SocketFd socket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (socket != -1) {
        if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
            return false;
        }
        
        int option = true;
        socklen_t optionLen = sizeof(option);
        
        struct linger l;
        l.l_linger = 0;
        l.l_onoff = 1;
        int intval = 1;
        
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optionLen);
        setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
        setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int));
        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &intval, sizeof(int));
        
        
        this->mSocket = socket;
        this->mStatus.isInit = true;
        return true;
    }
    return false;
}

bool UDPServer::bindSocket() {
    if (this->mStatus.isInit == false) {
        return false;
    }
    
    struct sockaddr_in sockAddrIn = this->mSocketAddress.getSockaddrIn();
    int ret = ::bind(this->mSocket, (struct sockaddr *)&sockAddrIn, sizeof(sockAddrIn));
    if (ret == 0) {
        return true;
    }
    return false;
}


bool UDPServer::closeSocket() {
    this->mStatus.isClosing = true;
    int ret = ::close(this->mSocket);
    if (ret == 0) {
        return true;
    }
    return false;
}

bool UDPServer::start() {
    int ret = this->createSocket();
    if (!ret) {
        throw SocketException::socksFdInitError;
    }
    ret = this->bindSocket();
    if (!ret) {
        throw SocketException::socksBindError;
    }

    this->mRecvThread = std::thread([this]() {
        this->recvHandle();
    });
    
    return true;
}

bool UDPServer::pause() {
    this->mStatus.isPause = true;
    return true;
}

bool UDPServer::resume() {
    this->mStatus.isPause = false;
    return true;
}

bool UDPServer::close() {
    this->mStatus.isClosing = true;
    int ret = ::close(this->mSocket);
    if (ret == 0) {
        this->mRecvThread.join();
        return true;
    }
    this->mStatus.isClosing = false;
    return false;
}

void UDPServer::recvHandle() {
    fd_set reads;
    int result;
    struct timeval timeout;
    uchar recvBuffer[UDP_BUFFER_SIZE];
    
    do {
        memset(recvBuffer, 0, UDP_BUFFER_SIZE);
        
        FD_ZERO(&reads);
        FD_SET(this->mSocket, &reads);
        int fd_max = this->mSocket + 1;
        
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        result = ::select(fd_max, &reads, 0, 0, &timeout);
        if (result == -1) {
            //server error
            std::cout<<"Server Error"<<std::endl;
            this->willStop = true;
        } else if (result == 0) {
            // timeout
        } else {
            if (FD_ISSET(this->mSocket, &reads)) {
                // accept
                struct sockaddr_in recvSocketAddrIn;
                socklen_t addrInLen = sizeof(recvSocketAddrIn);
                
                ssize_t recvLen = ::recvfrom(this->mSocket, recvBuffer, UDP_BUFFER_SIZE, 0, (struct sockaddr*)&recvSocketAddrIn, &addrInLen);
                if (recvLen == -1) {
                    continue;
                }
                SocketAddress recvAddress = SocketAddress();
                recvAddress.useSockAddrIn(recvSocketAddrIn);
                
                if (!this->mDelegate.expired()) {
                    this->mDelegate.lock()->serviceDidReadData(recvAddress, recvBuffer, (int)recvLen, nullptr);
                }
            }
        }
    } while (!this->willStop);
}

bool UDPServer::writeData(const uchar *data, int len) {
    return true;
}
