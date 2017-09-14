//
//  Socks5TransmitState.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/11.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Socks5TransmitState_hpp
#define Socks5TransmitState_hpp

#include "Socks5State.hpp"
#include "TCPConnection.hpp"

namespace ts {
    
class Socks5TransmitState: public Socks5State {
private:
    std::string mRemoteHost;
    std::string mRemotePort;
    
    TCPConnection *mRemoteConnection = nullptr;
    TCPConnection *mLocalConnection = nullptr;
public:
    void setRemoteHost(std::string host) {
        this->mRemoteHost = host;
    }
    
    void setRemotePort(std::string port) {
        this->mRemotePort = port;
    }
    
    
    Socks5TransmitState(Socks5ProtocolSyntaxAdapter *ctx, std::string key) {
        this->mSocks5Context = ctx;
        this->mClientKey = key;
    }
    
    ~Socks5TransmitState() {
        
    }
    void createRemoteConnection(TCPConnection *connection);
    
    virtual void handle(Socks5State::SOCKS5_STATE state) override;
    virtual void onSignalEvent(int fd, short what, void *arg) override;
    virtual void onEOFEvent(void *ctx) override;
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
};
    
}

#endif /* Socks5TransmitState_hpp */
