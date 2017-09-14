//
//  Socks5LinkRequestState.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/13.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <sstream>
#include <iostream>

#include "Socks5LinkRequestState.hpp"
#include "SocketException.hpp"
#include "Socks5ProtocolSyntaxAdapter.hpp"
#include "Socks5TransmitState.hpp"
#include "TCPConnection.hpp"

using namespace ts;

void Socks5LinkRequestState::handle(Socks5State::SOCKS5_STATE state) {
    switch (state) {
        case Socks5State::STATE_AUTH: {
            throw SocketException::socks5UnsupportStateError;
        }
            break;
        case Socks5State::STATE_TRANSMIT: {
            auto newState = std::shared_ptr<Socks5State>(new Socks5TransmitState(this->mSocks5Context, this->mClientKey));
            auto transmitPtr = (Socks5TransmitState *)newState.get();
            transmitPtr->setRemoteHost(this->mRemoteHost);
            transmitPtr->setRemotePort(this->mRemotePort);
            transmitPtr->createRemoteConnection(this->mLocalConnection);
            
            this->mSocks5Context->setState(this->mClientKey, newState);
        }
            break;
        case Socks5State::STATE_LINK_REQUEST: {
            throw SocketException::socks5UnsupportStateError;
        }
            break;
        default:
            break;
    }
}

void Socks5LinkRequestState::feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {
    
    auto localToProxyConnection = (TCPConnection *)ctx;
    auto key = localToProxyConnection->getSocketAddress().ipPortPairString();
    this->mLocalConnection = localToProxyConnection;
    
    if (key.compare(this->mClientKey) != 0) {
        return;
    }
    int len = inputStream->length();
    if (len < 5) {
        return;
    }
    
    unsigned char buf[1024] = {0};
    memset(buf, 0, 1024);
    inputStream->fetch(buf, 5);
    int aType = buf[3];
    int reqLen = 0;
    if (aType == 0x01) {
        reqLen = 10;
    } else if (aType == 0x03) {
        reqLen = 5 + buf[4] + 2;
    } else if (aType == 0x04) {
        reqLen = 22;
    }
    if (len < reqLen) {
        return;
    }
    
    inputStream->read(buf, reqLen);
    
    int sockCommand = buf[1];
    aType = buf[3];
    uint32_t destAddr = 0;
    uint16_t destPort = 0;
    std::string destAddrString;
    std::string destPortString;
    if (aType == 0x01) {
        destAddr = *((uint32_t *)(buf + 4));
        destPort = *((uint16_t *)(buf + 4 + 8));
        destPort = ntohs(destPort);
        
        struct in_addr inAddr;
        inAddr.s_addr = destAddr;
        destAddrString = inet_ntoa(inAddr);
        std::stringstream sss;
        sss<<destPort;
        destPortString = sss.str();
        
    } else if (aType == 0x03) {
        int destLen = buf[4];
        std::stringstream ss;
        std::stringstream sss;
        for (int p = 0; p < destLen; ++p) {
            ss<<buf[5 + p];
        }
        
        destAddrString = ss.str();
        uint8_t h = *((uint8_t *)(buf + 5 + destLen));
        uint8_t l = *((uint8_t *)(buf + 5 + destLen + 1));
        
        destPort = h;
        destPort = (destPort << 8) | l;
        
        sss<<destPort;
        destPortString = sss.str();
    } else if (aType == 0x04) {
        //unsupport
        return;
    }
    this->mRemoteHost = destAddrString;
    this->mRemotePort = destPortString;
    
    int bindPort = 12000;
    short nBindPort = htons(bindPort);
    std::string bindIp = "0.0.0.0";
    
    memset(buf, 0, 1024);
    buf[0] = 0x05;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x03;
    buf[4] = bindIp.length();
    strcpy((char *)buf + 5, bindIp.c_str());
    memcpy(buf + 5 + bindIp.length(), &nBindPort, 2);
    int sndLen = 5 + bindIp.length() + 2;
    outputStream->write(buf, sndLen);
    
    this->mSocks5Context->requestClientState(key, Socks5State::STATE_TRANSMIT);
    
}

