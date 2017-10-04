//
//  SocketAddress.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "SocketAddress.hpp"
#include <sstream>
using namespace ts;

//Just Support IPv4
void SocketAddress::setIp(std::string ip) {
    this->mIp = ip;
}
void SocketAddress::setPort(int port) {
    this->mPort = port;
}
void SocketAddress::setPort(std::string portString) {
    int port = atoi(portString.c_str());
    this->setPort(port);
}
void SocketAddress::setErrorMessage(std::string errorMsg) {
    this->mErrorMessage = errorMsg;
}
std::string SocketAddress::getHostname() {
    return this->mHostname;
}

struct event_base* SocketAddress::getEventBase() {
    return this->mEventBase;
}

struct evdns_base* SocketAddress::getEventDnsBase() {
    return this->mEventDnsBase;
}

int SocketAddress::getPort() {
    return this->mPort;
}
std::string SocketAddress::getIpString() {
    return mIp;
}
struct sockaddr_in SocketAddress::getSockaddrIn() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->mPort);
    addr.sin_addr.s_addr = inet_addr(this->mIp.c_str());
    return addr;
}
std::string SocketAddress::getErrorMessage() {
    return this->mErrorMessage;
}

static void callback(int errcode, struct evutil_addrinfo *addr, void *ptr)
{
    SocketAddress *ctx = (SocketAddress *)ptr;
    if (errcode) {
        ctx->setIp("");
        ctx->setErrorMessage(evutil_gai_strerror(errcode));
    } else {
        struct evutil_addrinfo *ai;
        for (ai = addr; ai; ai = ai->ai_next) {
            char buf[128];
            const char *s = NULL;
            if (ai->ai_family == AF_INET) {
                struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
                s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
            }
            if (s) {
                ctx->setIp(std::string(s));
            }
        }
        evutil_freeaddrinfo(addr);
    }
    event_base_loopexit(ctx->getEventBase(), NULL);
}

bool SocketAddress::setupResolveHost(std::string hostname) {
    if (this->mEventBase == nullptr) {
        this->mEventBase = event_base_new();
        
    }
    if (this->mEventDnsBase == nullptr) {
        this->mEventDnsBase = evdns_base_new(this->mEventBase, EVDNS_BASE_INITIALIZE_NAMESERVERS);
        evdns_base_set_option(this->mEventDnsBase, "timeout", "0.2");
    }
    
    struct evutil_addrinfo hints;
    struct evdns_getaddrinfo_request *req;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
    /* Unless we specify a socktype, we'll get at least two entries for
     * each address: one for TCP and one for UDP. That's not what we
     * want. */
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
   this->mHostname = hostname;
    req = evdns_getaddrinfo(
                            this->mEventDnsBase, hostname.c_str(), NULL,
                            &hints, callback, this);
    if (req == nullptr) {
        this->setIp(hostname);
    }
    return true;
}
void SocketAddress::startResolveHost() {
    event_base_dispatch(this->mEventBase);
}
void SocketAddress::waitForResolveFinish() {
    while (event_base_got_exit(this->mEventBase) == 0) {
        
    }
}

std::string SocketAddress::ipPortPairString() {
    std::stringstream ss;
    ss<<this->mIp<<":"<<this->mPort;
    return ss.str();
}
SocketAddress::SocketAddress(std::string hostname, int port){
    this->mPort = port;
    this->mEventBase = event_base_new();
    this->mHostname = hostname;
    
    try {
        setupResolveHost(hostname);
    } catch (SocketException e) {
        throw e;
    }
    
    startResolveHost();
}

SocketAddress::SocketAddress(std::string hostname, std::string portString) {
    int port = atoi(portString.c_str());
    this->mPort = port;
    this->mEventBase = event_base_new();
    this->mHostname = hostname;
    
    try {
        setupResolveHost(hostname);
    } catch (SocketException e) {
        throw e;
    }
    
    startResolveHost();
}

SocketAddress::SocketAddress() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    this->mIp = "";
    this->mPort = 0;
    this->mHostname = "";
    this->mEventBase = nullptr;
    this->mEventDnsBase = nullptr;
}
SocketAddress::~SocketAddress() {
    if (this->mEventDnsBase != nullptr) {
        evdns_base_free(this->mEventDnsBase, 0);
        this->mEventDnsBase = nullptr;
    }
    
    if (this->mEventBase != nullptr) {
        event_base_loopexit(this->mEventBase, nullptr);
        event_base_free(this->mEventBase);
        this->mEventBase = nullptr;
    }
}
