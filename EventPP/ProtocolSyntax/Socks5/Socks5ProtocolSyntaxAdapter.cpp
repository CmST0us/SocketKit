//
//  Socks5ProtocolSyntaxAdapter.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/9.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "Socks5ProtocolSyntaxAdapter.hpp"
#include "TCPConnection.hpp"
#include "Socks5AuthState.hpp"
#include "Socks5TransmitState.hpp"

using namespace ts;

Socks5ProtocolSyntaxAdapter::Socks5ProtocolSyntaxAdapter() {
    
}

Socks5ProtocolSyntaxAdapter::~Socks5ProtocolSyntaxAdapter() {
    
}

void Socks5ProtocolSyntaxAdapter::onSignalEvent(int fd, short what, void *arg) {
//    auto localToProxyConnection = (TCPConnection *)arg;
//    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
//    this->mClientState[key]->onSignalEvent(fd, what, arg);
}

void Socks5ProtocolSyntaxAdapter::onError(void *ctx) {
    
}

void Socks5ProtocolSyntaxAdapter::onEOFEvent(void *ctx) {
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    this->mClientState[key]->onEOFEvent(ctx);
}

void Socks5ProtocolSyntaxAdapter::onWriteable(void *ctx) {
    
}

void Socks5ProtocolSyntaxAdapter::willEndRead(void *ctx) {
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    this->mClientState[key]->onWillEndRead(ctx);
}

void Socks5ProtocolSyntaxAdapter::willEndWrite(void *ctx) {
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    this->mClientState[key]->onWillEndWrite(ctx);

}


void Socks5ProtocolSyntaxAdapter::feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    switch (type) {
        case ProtocolSyntax::Read: {
            this->mClientState[key]->feed(type, inputStream, outputStream, ctx);
        }
            break;
        case ProtocolSyntax::Accept: {
            this->mClientState[key] = std::shared_ptr<Socks5State>(new Socks5AuthState(this, key));
        }
            break;
        default:
            break;
    }
    
}

void Socks5ProtocolSyntaxAdapter::setState(std::string key, std::shared_ptr<Socks5State> state) {
    if (this->mClientState.count(key) > 0) {
        this->mClientState[key] = state;
    }
}
void Socks5ProtocolSyntaxAdapter::requestClientState(std::string key, Socks5State::SOCKS5_STATE s) {
    if (this->mClientState.count(key) > 0) {
        this->mClientState[key]->handle(s);
    }
}
