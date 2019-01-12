//
// Created by CmST0us on 2019/1/12.
//

#include "Endpoint.hpp"
#include "SocketException.hpp"

using namespace socketkit;


Endpoint::Endpoint(std::string domain, short port) : _endpointDomain{domain},
                                                     _endpointPort{port} {
    try {
        std::string ip = Endpoint::getIpByDomain(_endpointDomain);
        _endpointIp = ip;
    } catch (SocketException e) {
        throw e;
    }

}

Endpoint::~Endpoint() {}

std::string Endpoint::getEndpointIp() const {
    return _endpointIp;
}

std::string Endpoint::getEndpointDomain() const {
    return _endpointDomain;
}

short Endpoint::getEndpointPort() const {
    return _endpointPort;
}

struct sockaddr_in Endpoint::getEndpointSockaddrIn() const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_endpointPort);
    addr.sin_addr.s_addr = inet_addr(_endpointIp.c_str());
    return addr;
}




