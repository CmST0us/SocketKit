//
//  SocketAddress.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef SocketAddress_hpp
#define SocketAddress_hpp
#include <string>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SocketException.hpp"
namespace ts {
    
class SocketAddress {
private:
    
    int mPort;
    std::string mIp;
    std::string mHostname;
    
    std::string mErrorMessage;
public:
    //Just Support IPv4
    void setIp(std::string ip);
    void setPort(std::string portString);
    void setPort(int port);
    void setErrorMessage(std::string errorMsg);
    
    std::string getHostname();
    struct event_base* getEventBase();
    struct evdns_base* getEventDnsBase();
    std::string getErrorMessage();
    
    int getPort();
    std::string getIpString();
    struct sockaddr_in getSockaddrIn();
    
    bool setupResolveHost(std::string hostname);
    void startResolveHost();
    void waitForResolveFinish();
    
    std::string ipPortPairString();
    
    SocketAddress(std::string hostname, int port);
    SocketAddress(std::string hostname, std::string portString);
    SocketAddress();
    
    ~SocketAddress();
};
    
}
#endif /* SocketAddress_hpp */
