//
//  TCPServer.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <functional>
#include <map> //or lamber method ?[TODO]
#include <vector>
#include <memory>
#include <mutex>

#include <event2/event.h>
#include <event2/listener.h>

#include "SocketAddress.hpp"
#include "TCPConnection.hpp"
#include "ProtocolSyntax.hpp"

namespace ts{
#warning [TODO] 端口冲突
class TCPServer{
private:
    SocketAddress mSocketAddress;
    std::shared_ptr<ProtocolSyntax> mSyntax;
    std::map<std::string, TCPConnection *> mClients;
    
    struct event_base* mEventBase = NULL;
    struct evconnlistener* mListener = NULL;
    
    std::mutex clientMapLock;
    
    void _init();
public:
    
    
    void setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax);
    
    TCPConnection * getConnectionWithKey(std::string s);
    void setConnectionWithKey(std::string s, TCPConnection *connection);
    void removeConnectionWithKey(std::string s);
    
    ProtocolSyntax* getProtocolSyntax();
    SocketAddress getSocketAddress();
    
    void onAcceptEvent(struct evconnlistener *listener,
                               evutil_socket_t fd,
                               struct sockaddr *address,
                               int socklen,
                               void *ctx);
    void onAcceptErrorEvent(struct evconnlistener *listener, void *ctx);
    
    TCPServer();
    TCPServer(SocketAddress& address);
    TCPServer(short port);
    TCPServer(short port, int backlog);
    TCPServer(SocketAddress& address, int backlog);
    ~TCPServer();
    
    bool setup();
    
    void start();
    void stop();
    
    
};
    
}
#endif /* TCPServer_hpp */
