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

//Just Support IPv4
void SocketAddress::setIp(std::string ip) {
    this->mIp = ip;
}
void SocketAddress::setPort(int port) {
    this->mPort = port;
}
void SocketAddress::setPort(std::string portString) {
    int port = atoi(portString.c_str());
    this->setPort(port);
}
void SocketAddress::setErrorMessage(std::string errorMsg) {
    this->mErrorMessage = errorMsg;
}
std::string SocketAddress::getHostname() {
    return this->mHostname;
}

int SocketAddress::getPort() {
    return this->mPort;
}

std::string SocketAddress::getIpString() {
    return mIp;
}

struct sockaddr_in SocketAddress::getSockaddrIn() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->mPort);
    addr.sin_addr.s_addr = inet_addr(this->mIp.c_str());
    return addr;
}

std::string SocketAddress::getErrorMessage() {
    return this->mErrorMessage;
}


bool SocketAddress::setupResolveHost(std::string hostname) {
    
    return true;
}
void SocketAddress::startResolveHost() {
    
}

void SocketAddress::waitForResolveFinish() {
    
}

std::string SocketAddress::ipPortPairString() {
    std::stringstream ss;
    ss<<this->mIp<<":"<<this->mPort;
    return ss.str();
}

SocketAddress::SocketAddress(std::string hostname, int port){
    this->mPort = port;
    this->mHostname = hostname;
    
    try {
        setupResolveHost(hostname);
    } catch (SocketException e) {
        throw e;
    }
    
    startResolveHost();
}

SocketAddress::SocketAddress(std::string hostname, std::string portString) {
    int port = atoi(portString.c_str());
    this->mPort = port;
    this->mHostname = hostname;
    
    try {
        setupResolveHost(hostname);
    } catch (SocketException e) {
        throw e;
    }
    
    startResolveHost();
}

SocketAddress::SocketAddress() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    this->mIp = "";
    this->mPort = 0;
    this->mHostname = "";
}
SocketAddress::~SocketAddress() {
    
}
