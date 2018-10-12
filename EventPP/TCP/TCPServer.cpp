//
//  TCPServer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <thread>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <evutil.h>
#include <event2/thread.h>

#include "TCPConnection.hpp"
#include "TCPServer.hpp"

using namespace ts;

TCPServer::TCPServer() {
    this->mSocket = -1;
    this->mSocketAddress.mHostname = "0.0.0.0";
    this->mSocketAddress.mPort = 0;
}

TCPServer::TCPServer(SocketAddress address) {
    this->mSocket = -1;
    this->mSocketAddress = address;
}

TCPServer::TCPServer(short port) {
    this->mSocket = -1;
    this->mSocketAddress.mPort = port;
    this->mSocketAddress.mHostname = "0.0.0.0";
}

TCPServer::~TCPServer() {
    
}

bool TCPServer::writeData(const uchar *data, int len) {
    return true;
}

bool TCPServer::start() {
    
    return true;
}

bool TCPServer::pause() {
    return true;
}

bool TCPServer::resume() {
    return true;
}

bool TCPServer::close() {
    return true;
}



