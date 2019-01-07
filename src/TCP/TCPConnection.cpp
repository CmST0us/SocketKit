//
//  TCPConnection.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>

#include "SocketKit.hpp"
#include "TCPConnection.hpp"
#include "SocketException.hpp"

using namespace socketkit;

TCPConnection::TCPConnection() {
    this->mSocket = -1;
}

TCPConnection::TCPConnection(std::string hostname, int port) {
    this->mSocketAddress.mPort = port;
    this->mSocketAddress.mHostname = hostname;
    this->mSocketAddress.startResolveHost();
    this->mSocket = -1;
}

TCPConnection::TCPConnection(std::string hostname, std::string portString) {
    this->mSocketAddress.mHostname = hostname;
    this->mSocketAddress.mPort = atoi(portString.c_str());
    this->mSocketAddress.startResolveHost();
    this->mSocket = -1;
}

TCPConnection::TCPConnection(SocketAddress address) {
    
}

TCPConnection::~TCPConnection() {
    
}

void TCPConnection::useSocketFd(SocketFd fd) {
    this->mSocket = fd;
}

bool TCPConnection::writeData(const uchar *data, int len) {
#if _WIN32
    ssize_t s = send(this->mSocket, (const char *)data, len, 0);
#else
    ssize_t s = send(this->mSocket, data, len, 0);
#endif
    if (s > 0) return true;
    return false;
}

bool TCPConnection::start() {
    return true;
}

bool TCPConnection::pause() {
    return true;
}

bool TCPConnection::resume() {
    return true;
}

bool TCPConnection::close() {
    shutdown(this->mSocket, SHUT_RDWR);
#if _WIN32
    ::closesocket(this->mSocket);
#else
    ::close(this->mSocket);
#endif
    return true;
}
