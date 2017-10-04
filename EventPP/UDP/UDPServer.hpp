//
//  UDPServer.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef UDPServer_hpp
#define UDPServer_hpp

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <memory>
#include <evutil.h>
#include <map>

#include "ProtocolSyntax.hpp"
#include "SocketAddress.hpp"
#include "SocketException.hpp"
#include "UDPConnection.hpp"

namespace ts {
class UDPServer {
private:
    std::shared_ptr<ProtocolSyntax> mSyntax;
//    std::map<std::string, UDPConnection *> mClients;
    struct event_base* mEventBase = nullptr;
    SocketAddress mSocketAddress;
    evutil_socket_t mSocketFd;
    
    
    void _init();
public:
    UDPServer(SocketAddress& address);
    UDPServer(short port);
    ~UDPServer();
    
    void setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax);
    
    ProtocolSyntax * getProtocolSyntax();
    
    void setup();
    void start();
    void stop();
    
    void onReadable(ev_socklen_t sockfd);
};
    
}
#endif /* UDPServer_hpp */

/*
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
 #endif

 */

