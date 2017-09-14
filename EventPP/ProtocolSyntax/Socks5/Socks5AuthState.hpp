//
//  Socks5AuthState.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/11.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Socks5AuthState_hpp
#define Socks5AuthState_hpp

#include "Socks5State.hpp"

namespace ts {
    
class Socks5AuthState: public Socks5State {
private:
    
public:
    Socks5AuthState(Socks5ProtocolSyntaxAdapter *ctx, std::string key) {
        this->mSocks5Context = ctx;
        this->mClientKey = key;
    }
    
    ~Socks5AuthState() {
        
    }
    
    virtual void handle(Socks5State::SOCKS5_STATE state) override;
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
};
    
}

#endif /* Socks5AuthState_hpp */
