//
//  TCPConnection.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>

#include "TCPConnection.hpp"
#include "SocketException.hpp"

using namespace ts;

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
    return true;
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
    return true;
}
