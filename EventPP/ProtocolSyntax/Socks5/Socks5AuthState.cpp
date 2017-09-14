//
//  Socks5AuthState.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/11.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "Socks5AuthState.hpp"
#include "SocketException.hpp"
#include "Socks5ProtocolSyntaxAdapter.hpp"
#include "Socks5LinkRequestState.hpp"
#include "TCPConnection.hpp"

using namespace ts;

void Socks5AuthState::handle(Socks5State::SOCKS5_STATE state) {
    switch (state) {
        case Socks5State::STATE_AUTH: {
            throw SocketException::socks5UnsupportStateError;
        }
            break;
        case Socks5State::STATE_TRANSMIT: {
            throw SocketException::socks5UnsupportStateError;
        }
            break;
        case Socks5State::STATE_LINK_REQUEST: {
            auto newState = std::shared_ptr<Socks5State>(new Socks5LinkRequestState(this->mSocks5Context, this->mClientKey));
            this->mSocks5Context->setState(this->mClientKey, newState);
        }
            break;
        default:
            break;
    }
}

void Socks5AuthState::feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {
    
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    
    if (key.compare(this->mClientKey) != 0) {
        return;
    }
    
    auto len = inputStream->length();
    if (len < 3) {
        return;
    }
    
    unsigned char buf[1024] = {0};
    inputStream->fetch(buf, 2);
    if (buf[0] != 0x05) {
        //version unsupport
    }
    
    int methodLen = buf[1];
    int reqLen = 2 + methodLen;
    if (len < reqLen) {
        return;
    }
    
    memset(buf, 0, 1024);
    inputStream->read(buf, reqLen);
    int supportAuthFlag = 0;
    for (int i = 0; i < buf[1]; ++i) {
        if (buf[2 + i] == 0) {
            supportAuthFlag = 1;
        }
    }
    
    if (!supportAuthFlag) {
        return;
    }
    
    unsigned char res[2] = {0x05, 0x00};
    outputStream->write(res, 2);
    this->mSocks5Context->requestClientState(this->mClientKey, Socks5State::STATE_LINK_REQUEST);
}

