//
//  Socks5State.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/11.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Socks5State_hpp
#define Socks5State_hpp
#include <string>
#include "ProtocolSyntax.hpp"

namespace ts {
class Socks5ProtocolSyntaxAdapter;
class TCPConnection;
    
class Socks5State {
public:
    enum SOCKS5_STATE {
        STATE_AUTH,
        STATE_LINK_REQUEST,
        STATE_TRANSMIT,
    };
    
protected:
    Socks5ProtocolSyntaxAdapter *mSocks5Context;
//    TCPConnection *mConnectionContext;
    std::string mClientKey;
public:
//    TCPConnection *getConnectionContext() {
//        return this->mConnectionContext;
//    }
//    
//    void setConnectionContext(TCPConnection *ctx) {
//        this->mConnectionContext = ctx;
//    }
//    
    Socks5State() {
        
    };
    
    Socks5State(Socks5ProtocolSyntaxAdapter *socks5, std::string key) {
        this->mSocks5Context = socks5;
        this->mClientKey = key;
    };
    
    virtual ~Socks5State() {};
    
    virtual void handle(Socks5State::SOCKS5_STATE state) {};
//状态机消息传递
    virtual void onSignalEvent(int fd, short what, void *arg) {};
    virtual void onEOFEvent(void *ctx) {};
    virtual void onWillEndWrite(void *ctx) {};
    virtual void onWillEndRead(void *ctx) {};
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {};
};
}
#endif /* Socks5State_hpp */
