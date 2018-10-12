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
#endif

