//
//  UDPConnection.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef UDPConnection_hpp
#define UDPConnection_hpp

#include <memory>

#include <event2/event.h>
#include <event2/bufferevent.h>

#include "SocketAddress.hpp"
#include "Stream.hpp"
#include "ProtocolSyntax.hpp"

namespace ts {
    class UDPServer;
    class UDPConnection{
        friend class UDPServer;
    private:
        void *mContext;
        
        SocketAddress mSocketAddress;
        struct event_base* mEventBase = nullptr;
        
        std::shared_ptr<ProtocolSyntax> mSyntax;
        
        void _init();
        
    public:
        void setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax);
        void *getContext() const;
        void setContext(void * ctx);
        
        SocketAddress getSocketAddress();
        ProtocolSyntax* getProtocolSyntax();
        
        UDPConnection();
        UDPConnection(std::string hostname, short port);
        UDPConnection(std::string hostname, std::string portString);
        ~UDPConnection();
        
        void connect();
        void connect(std::string hostname, short port);
        void connect(std::string hostname, std::string portString);
        void close();
        void start();
        void stop();
        
        void onSignalEvent(int fd, short what, void *arg);
        void onConnectedEvent();
        void onReadableEvent(struct bufferevent *bev, void *ptr);
        void onWriteableEvent(struct bufferevent *bev, void *ptr);
        void onErrorEvent();
        void onWriteError();
        void onReadError();
        void onEOFEvent();
    };
}

#endif /* UDPConnection_hpp */
