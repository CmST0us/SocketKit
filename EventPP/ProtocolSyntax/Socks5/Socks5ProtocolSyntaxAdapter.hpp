//
//  Socks5ProtocolSyntaxAdapter.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/9.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Socks5ProtocolSyntaxAdapter_hpp
#define Socks5ProtocolSyntaxAdapter_hpp

#include <map>
#include <memory>

#include "ProtocolSyntax.hpp"
#include "Socks5State.hpp"

namespace ts {
class Socks5State;
class Socks5ProtocolSyntaxAdapter: public ProtocolSyntax {
    
private:
    std::map<std::string, std::shared_ptr<Socks5State> > mClientState;

public:
    Socks5ProtocolSyntaxAdapter();
    ~Socks5ProtocolSyntaxAdapter();
    
    virtual void onEOFEvent(void *ctx) override;
    virtual void willEndRead(void *ctx) override;
    virtual void willEndWrite(void *ctx) override;
    virtual void onError(void *ctx) override;
    virtual void onWriteable(void *ctx) override;
    virtual void onSignalEvent(int fd, short what, void *arg) override;
    
    virtual void feed(EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
    
    void setState(std::string key, std::shared_ptr<Socks5State> state);
    void requestClientState(std::string key, Socks5State::SOCKS5_STATE s);
};
}

#endif /* Socks5ProtocolSyntaxAdapter_hpp */
