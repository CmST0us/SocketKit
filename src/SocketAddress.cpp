//
//  SocketAddress.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "SocketAddress.hpp"
#include <sstream>
using namespace ts;

struct sockaddr_in SocketAddress::getSockaddrIn() const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->mPort);
    addr.sin_addr.s_addr = inet_addr(this->mIp.c_str());
    return addr;
}

void SocketAddress::startResolveHost() {
    try {
        //[TODO] Random Port
        std::string ip = SocketAddress::getHostByName(this->mHostname);
        this->mIp = ip;
    } catch (SocketException e) {
        throw e;
    }
}

std::string SocketAddress::getIpPortPairString() const {
    std::stringstream ss;
    ss<<this->mIp<<":"<<this->mPort;
    return ss.str();
}

std::string SocketAddress::getIpString() const {
    return this->mIp;
}

void SocketAddress::useSockAddrIn(struct sockaddr_in addr_in) {
    if (addr_in.sin_family == AF_INET) {
        int port = ntohs(addr_in.sin_port);
        this->mPort = port;
        char *ipAddrStr = inet_ntoa(addr_in.sin_addr);
        this->mIp = std::string(ipAddrStr, strlen(ipAddrStr));
        this->mHostname = mIp;
    }
}

SocketAddress::SocketAddress(std::string hostname, int port) {
    this->mPort = port;
    this->mHostname = hostname;
    this->mIp = "";
    
    try {
        startResolveHost();
    } catch (SocketException e) {
        throw e;
    }
}

SocketAddress::SocketAddress(std::string hostname, std::string portString) {
    int port = atoi(portString.c_str());
    this->mIp = "";
    this->mPort = port;
    this->mHostname = hostname;
    
    try {
        startResolveHost();
    } catch (SocketException e) {
        throw e;
    }
}

SocketAddress::SocketAddress(std::string hostname) {
    this->mIp = "";
    this->mPort = 0;
    this->mHostname = hostname;
    try {
        startResolveHost();
    } catch (SocketException e) {
        throw e;
    }
}

SocketAddress::SocketAddress() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    this->mIp = "";
    this->mPort = -1;
    this->mHostname = "";
}
SocketAddress::~SocketAddress() {
    
}
