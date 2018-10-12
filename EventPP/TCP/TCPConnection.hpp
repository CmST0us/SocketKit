//
//  TCPConnection.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef TCPConnection_hpp
#define TCPConnection_hpp

#include <memory>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <evutil.h>

#include "SocketAddress.hpp"
#include "ProtocolSyntax.hpp"

namespace ts {
class TCPServer;
class TCPConnection{
    friend class TCPServer;
private:
    void *mContext;
    
    SocketAddress mSocketAddress;
    struct event_base* mEventBase = nullptr;
    struct bufferevent* mBufferEvent = nullptr;
    
    evutil_socket_t mSocketFd = -1;
    
    std::unique_ptr<InputStream> mInputStream;
    std::unique_ptr<OutputStream> mOutputStream;
    std::shared_ptr<ProtocolSyntax> mSyntax;
    
    void _init();
    
public:
    void setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax);
    void *getContext() const;
    void setContext(void * ctx);
    
    SocketAddress getSocketAddress();
    InputStream* getInputStream();
    OutputStream* getOutputStream();
    ProtocolSyntax* getProtocolSyntax();
    void setTimeout(struct timeval *readTimeout, struct timeval *writeTimeout);
    
    TCPConnection();
    TCPConnection(std::string hostname, short port);
    TCPConnection(std::string hostname, std::string portString);
    ~TCPConnection();
    
    void connect();
    void connect(std::string hostname, short port);
    void connect(std::string hostname, std::string portString);
    void close();
    void shutdown(int how);
    void disable(int how);
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
#endif /* TCPConnection_hpp */
