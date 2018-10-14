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
#include <netdb.h>

#include "SocketException.hpp"
namespace ts {
    
    struct SocketAddress {
    private:
        std::string mIp;
        
    public:
        SocketAddress(std::string hostname, int port);
        SocketAddress(std::string hostname, std::string portString);
        SocketAddress(std::string hostname);
        SocketAddress();
        
        ~SocketAddress();
        
        int mPort;
        std::string mHostname;
        
        struct sockaddr_in getSockaddrIn() const;
        std::string getIpPortPairString() const;
        std::string getIpString() const;
        
        void useSockAddrIn(struct sockaddr_in addr_in);
        void startResolveHost();
        
    // class func
    public:
        static std::string getHostByName(std::string hostname) {
            struct hostent *host = ::gethostbyname(hostname.c_str());
            if (host == NULL) {
                throw SocketException::hostCannotResolve;
            }
            if (host->h_addrtype == AF_INET) {
                char *ipStr = ::inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
                size_t ipStrLen = strlen(ipStr);
                return std::string(ipStr, ipStrLen);
            } else {
                throw SocketException::hostCannotResolve;
            }
        };
    };
    
}
#endif /* SocketAddress_hpp */
