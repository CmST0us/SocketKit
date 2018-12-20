//
//  UDPConnection.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "UDPConnection.hpp"

using namespace ts;

UDPConnection::UDPConnection() {
    this->mSocket = -1;
}

UDPConnection::UDPConnection(std::string hostname, int port) {
    this->mSocketAddress.mPort = port;
    this->mSocketAddress.mHostname = hostname;
    this->mSocketAddress.startResolveHost();
    this->mSocket = -1;
}

UDPConnection::UDPConnection(std::string hostname, std::string portString) {
    this->mSocketAddress.mHostname = hostname;
    this->mSocketAddress.mPort = atoi(portString.c_str());
    this->mSocketAddress.startResolveHost();
    this->mSocket = -1;
}

UDPConnection::UDPConnection(SocketAddress address) {
    this->mSocketAddress = address;
    this->mSocket = -1;
}

UDPConnection::~UDPConnection() {
    
}

void UDPConnection::useSocketFd(SocketFd fd) {
    this->mSocket = fd;
}

bool UDPConnection::writeData(const uchar *data, int len) {
    sockaddr_in sockaddrIn = this->mSocketAddress.getSockaddrIn();
    ssize_t sendLen = ::sendto(this->mSocket, data, len, 0, (struct sockaddr *)&sockaddrIn, sizeof(sockaddrIn));
    if (sendLen > 0) return true;
    return false;
}

bool UDPConnection::start() {
    this->mSocket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (this->mSocket == -1) {
        throw SocketException::socksFdInitError;
        return false;
    }
    return true;
}

bool UDPConnection::pause() {
    return true;
}

bool UDPConnection::resume() {
    return true;
}

bool UDPConnection::close() {
    shutdown(this->mSocket, SHUT_RDWR);
    ::close(this->mSocket);
    return true;
}
