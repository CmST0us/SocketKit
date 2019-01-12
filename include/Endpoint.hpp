//
// Created by CmST0us on 2019/1/12.
//

#pragma once

#include <string>
#include <iostream>

#include "SocketException.hpp"
#include "SocketKit.hpp"
#include "NoCopyable.hpp"

namespace socketkit {

class Endpoint final : public utils::NoCopyable {

public:

    Endpoint(std::string domain, short port);
    ~Endpoint();

    std::string getEndpointIp() const;
    std::string getEndpointDomain() const;
    short getEndpointPort() const;
    struct sockaddr_in getEndpointSockaddrIn() const;

    static std::shared_ptr<Endpoint> create(struct sockaddr_in addr_in) {
        if (addr_in.sin_family == AF_INET) {
            try {
                short port = ntohs(addr_in.sin_port);
                char *ipAddrStr = inet_ntoa(addr_in.sin_addr);
                std::string ipString = std::string(ipAddrStr, strlen(ipAddrStr));

                auto ptr = std::make_shared<Endpoint>(ipString, port);
                return ptr;
            } catch (SocketException e) {
                return nullptr;
            }
        }
        return nullptr;
    }

    static std::string getIpByDomain(std::string domain) {
        struct hostent *host = ::gethostbyname(domain.c_str());
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
    }

private:
    std::string _endpointIp{"127.0.0.1"};
    std::string _endpointDomain{"localhost"};
    short  _endpointPort{0};
};


}


