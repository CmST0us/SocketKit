//
//  UDPServer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "UDPServer.hpp"
#include <evutil.h>
#include <event.h>
#include <iostream>

using namespace ts;

static void eventCallback(evutil_socket_t socket, short event, void *ctx) {
    auto serverCtx = (UDPServer *)ctx;
    switch (event) {
        case EV_READ: {
            serverCtx->onReadable(socket);
        }
            break;
        default:
            break;
    }
}

UDPServer::UDPServer(SocketAddress& address) {
    this->_init();
    this->mSocketAddress = address;
}
UDPServer::UDPServer(short port) {
    this->_init();
    this->mSocketAddress.setPort(port);
    this->mSocketAddress.setIp("0.0.0.0");
}

UDPServer::~UDPServer() {
    if (this->mEventBase != nullptr) {
        event_base_free(this->mEventBase);
        this->mEventBase = nullptr;
    }
    close(this->mSocketFd);
}

void UDPServer::setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax) {
    this->mSyntax = syntax;
}

ProtocolSyntax * UDPServer::getProtocolSyntax() {
    return this->mSyntax.get();
}

void UDPServer::_init() {
    this->mEventBase = event_base_new();
}


void UDPServer::setup() {
    auto fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        //init socket fd error
        throw SocketException::socksFdInitError;
    }
    this->mSocketFd = fd;
    
    auto sockaddrIn = this->mSocketAddress.getSockaddrIn();
    auto ret = ::bind(this->mSocketFd, (struct sockaddr *)&sockaddrIn, sizeof(sockaddrIn));
    if (ret < 0) {
        throw SocketException::socksBindError;
    }
    
    auto ev = event_new(this->mEventBase, this->mSocketFd, EV_READ | EV_PERSIST, eventCallback, (void *)this);
    if (ev == nullptr) {
        throw SocketException::eventInitError;
    }
    if (event_add(ev, NULL) < 0){
        throw SocketException::eventAddError;
    }
}

void UDPServer::start() {
    event_base_dispatch(this->mEventBase);
}

void UDPServer::stop() {
    event_base_loopbreak(this->mEventBase);
}

void UDPServer::onReadable(socklen_t sockfd) {
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(addr);
    unsigned char buf[1500] = {0};
    auto recvLen = recvfrom(sockfd, buf, 1500, 0, (struct sockaddr *)&addr, &socklen);
    std::cout<<"recv from"<<inet_ntoa(addr.sin_addr)<<":"<<ntohs(addr.sin_port) <<": len"<<recvLen<<std::endl;
    sendto(sockfd, buf, recvLen, 0, (const struct sockaddr *)&addr, socklen);
}

