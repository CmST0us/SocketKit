//
//  Socks5LinkRequestState.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/13.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Socks5LinkRequestState_hpp
#define Socks5LinkRequestState_hpp

#include <string>

#include "Socks5State.hpp"

namespace ts {

class Socks5LinkRequestState: public Socks5State {
private:
    std::string mRemoteHost;
    std::string mRemotePort;
    
    TCPConnection *mLocalConnection;
public:
    Socks5LinkRequestState(Socks5ProtocolSyntaxAdapter *ctx, std::string key) {
        this->mSocks5Context = ctx;
        this->mClientKey = key;
    }
    
    ~Socks5LinkRequestState() {
        
    }
    
    virtual void handle(Socks5State::SOCKS5_STATE state) override;
//    virtual void onEOFEvent(void *ctx) override;
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
};
    
}

#endif /* Socks5LinkRequestState_hpp */
