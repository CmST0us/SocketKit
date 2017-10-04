//
//  TCPServer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <thread>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <evutil.h>
#include <event2/thread.h>

#include "TCPConnection.hpp"
#include "TCPServer.hpp"
#include "EvBufferAdapter.hpp"

using namespace ts;

//callback
static void acceptCallback(struct evconnlistener *listener,
               evutil_socket_t fd, struct sockaddr *address, int socklen,
               void *ctx) {
    TCPServer *server = (decltype(server))ctx;
    server->onAcceptEvent(listener, fd, address, socklen, ctx);
}

static void acceptErrorCallback(struct evconnlistener *listener, void *ctx) {
    TCPServer *server = (decltype(server))ctx;
    server->onAcceptErrorEvent(listener, ctx);
}

static void readCallback(struct bufferevent *bev, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    ctx->onReadableEvent(bev, ptr);
}

static void writeCallback(struct bufferevent *bev, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    ctx->onWriteableEvent(bev, ptr);
}

static void eventCallback(struct bufferevent *bev, short events, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    auto connection = ctx;
    auto server = (TCPServer *)ctx->getContext();
    if (events & BEV_EVENT_ERROR) {
        if (events & BEV_EVENT_READING) {
            connection->onReadError();// 对端强制结束！
        } else if (events & BEV_EVENT_WRITING) {
            connection->onWriteError();// 对端结束后写入，发生SIGPIPE!// 管道破裂
        } else {
            if (connection != nullptr && server != nullptr) {
                server->removeConnectionWithKey(connection->getSocketAddress().ipPortPairString());
            }
        }
    } else if (events & BEV_EVENT_EOF) {
        connection->onEOFEvent();
    }
    
}

void TCPServer::_init() {
#ifdef __WIN32
    evthread_use_windows_threads();
#else
    evthread_use_pthreads();
#endif
    this->mEventBase = event_base_new();
    
}

TCPServer::TCPServer() {
    this->_init();
    this->mSocketAddress.setIp("0.0.0.0");
    this->mSocketAddress.setPort(0);
}

TCPServer::TCPServer(SocketAddress& address) {
    this->_init();
    this->mSocketAddress = address; //[TODO]测试对象拷贝
}

TCPServer::TCPServer(short port) {
    this->_init();
    this->mSocketAddress.setPort(port);
    this->mSocketAddress.setIp("0.0.0.0");
}

TCPServer::TCPServer(short port, int backlog) {
    this->_init();
    this->mSocketAddress.setPort(port);
}

TCPServer::TCPServer(SocketAddress& address, int backlog) {
    this->_init();
    this->mSocketAddress = address;
}

TCPServer::~TCPServer() {
    if (this->mListener != nullptr) {
        evconnlistener_free(this->mListener);
        this->mListener = nullptr;
    }
    
    if (this->mEventBase != nullptr) {
        event_base_loopbreak(this->mEventBase);
        event_base_free(this->mEventBase);
        this->mEventBase = nullptr;
    }
}
void TCPServer::setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax) {
    this->mSyntax = syntax;
}

TCPConnection * TCPServer::getConnectionWithKey(std::string s) {
    clientMapLock.lock();
    TCPConnection *c;
    if (this->mClients.count(s) != 0) {
        c = this->mClients[s];
    } else {
        c = nullptr;
    }
    clientMapLock.unlock();
    return c;
}

void TCPServer::setConnectionWithKey(std::string s, TCPConnection *connection) {
    clientMapLock.lock();
    
    this->mClients[s] = connection;
    
    clientMapLock.unlock();
}

void TCPServer::removeConnectionWithKey(std::string s) {
    clientMapLock.lock();
    
    auto &&c = this->mClients[s];
    delete c;
    c = nullptr;
//    this->mClients.erase(s);
    
    clientMapLock.unlock();
}

ProtocolSyntax* TCPServer::getProtocolSyntax() {
    return this->mSyntax.get();
}

SocketAddress TCPServer::getSocketAddress() {
    return this->mSocketAddress;
}
bool TCPServer::setup() {
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(addr);
    addr = this->mSocketAddress.getSockaddrIn();
    this->mListener = evconnlistener_new_bind(this->mEventBase, acceptCallback, this,
                                       LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE|LEV_OPT_THREADSAFE, -1,
                                       (struct sockaddr*)&addr, sizeof(addr));
    int fd = evconnlistener_get_fd(this->mListener);
    getsockname(fd, (struct sockaddr *)&addr, &socklen);
    
    this->mSocketAddress.setIp(std::string(inet_ntoa(addr.sin_addr)));
    this->mSocketAddress.setPort(ntohs(addr.sin_port));
    if (!this->mListener) {
        this->mListener = nullptr;
        throw SocketException::connectListenerCreateError;
        return false;
    }
    evconnlistener_set_error_cb(this->mListener, acceptErrorCallback);
    return true;
}

void TCPServer::start() {
    event_base_dispatch(this->mEventBase);
}

void TCPServer::stop() {
    evconnlistener_disable(this->mListener);
}

void TCPServer::onAcceptEvent(struct evconnlistener *listener, int fd, struct sockaddr *address, int socklen, void *ctx) {
    auto eventBase = evconnlistener_get_base(listener);
    auto bev = bufferevent_socket_new(eventBase, fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
    struct sockaddr_in addrIn = *(sockaddr_in *)address;
    
    auto connection = new TCPConnection();
    bufferevent_setcb(bev, readCallback, writeCallback, eventCallback, connection);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_SIGNAL | EV_CLOSED);
    
    connection->mEventBase = nullptr;
    connection->mBufferEvent = bev;
    connection->mSocketAddress.setIp(std::string(inet_ntoa(addrIn.sin_addr)));
    connection->mSocketAddress.setPort(ntohs(addrIn.sin_port));
    
    auto inputBufferAdapter = std::unique_ptr<Buffer>(new EvBufferAdapter(bufferevent_get_input(bev)));
    auto outputBufferAdapter = std::unique_ptr<Buffer>(new EvBufferAdapter(bufferevent_get_output(bev)));
    
    auto inputStream = std::unique_ptr<InputStream>(new InputStream(inputBufferAdapter));
    auto outputStream = std::unique_ptr<OutputStream>(new OutputStream(outputBufferAdapter));
    
    connection->mInputStream = std::move(inputStream);
    connection->mOutputStream = std::move(outputStream);
    connection->mContext = this;
    connection->setProtocolSyntax(this->mSyntax);
    
    auto key = connection->mSocketAddress.ipPortPairString();
    this->setConnectionWithKey(key, connection);
    
    connection->getProtocolSyntax()->feed(ts::ProtocolSyntax::EventType::Accept, connection->getInputStream(), connection->getOutputStream(), connection);
}

void TCPServer::onAcceptErrorEvent(struct evconnlistener *listener, void *ctx) {
    
}
