//
//  TCPConnection.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>

#include <event2/event.h>
#include <evutil.h>
#include <event2/thread.h>

#include "TCPConnection.hpp"
#include "SocketAddress.hpp"
#include "SocketException.hpp"
#include "EvBufferAdapter.hpp"
using namespace ts;
static void readCallback(struct bufferevent *bev, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    ctx->onReadableEvent(bev, nullptr);
}

static void writeCallback(struct bufferevent *bev, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    ctx->onWriteableEvent(bev, nullptr);
}

static void buffereventCallback(struct bufferevent *bev, short events, void *ptr) {
    auto ctx = (TCPConnection *)ptr;
    if (events & BEV_EVENT_CONNECTED) {
        ctx->onConnectedEvent();
    } else if (events & BEV_EVENT_ERROR) {
        if (events & BEV_EVENT_WRITING) {
            ctx->onWriteError();
        } else if (events & BEV_EVENT_READING) {
            ctx->onReadError();
        } else {
            ctx->onErrorEvent();
        }
    } else if (events & BEV_EVENT_EOF) {
        ctx->onEOFEvent();
    }
}

static void signalCallback(int fd, short what, void *arg) {
    auto connection = (TCPConnection *)arg;
    connection->onSignalEvent(fd, what, arg);
}

void TCPConnection::_init() {
#ifdef __WIN32
    evthread_use_windows_threads();
#else
    evthread_use_pthreads();
#endif
}

TCPConnection::TCPConnection() {
    this->_init();
}

TCPConnection::TCPConnection(std::string hostname, short port) {
    this->_init();
    this->mSocketAddress.setPort(port);
    this->mSocketAddress.setupResolveHost(hostname);
    this->mSocketAddress.startResolveHost();
}

TCPConnection::TCPConnection(std::string hostname, std::string portString) {
    this->_init();
    this->mSocketAddress.setPort(portString);
    this->mSocketAddress.setupResolveHost(hostname);
    this->mSocketAddress.startResolveHost();
}

TCPConnection::~TCPConnection() {
//evbuffer被bufferevent引用，由bufferevent释放evbuffer
    if (this->mBufferEvent != nullptr) {
        bufferevent_free(this->mBufferEvent);
        this->mBufferEvent = nullptr;
    }
    if (this->mEventBase != nullptr) {
        event_base_loopexit(this->mEventBase, nullptr);
        event_base_free(this->mEventBase);
        this->mEventBase = nullptr;
    }
}
void TCPConnection::setProtocolSyntax(std::shared_ptr<ProtocolSyntax> syntax) {
    this->mSyntax = syntax;
}
void * TCPConnection::getContext() const{
    return this->mContext;
}

void TCPConnection::setContext(void * ctx) {
    this->mContext = ctx;
}

SocketAddress TCPConnection::getSocketAddress() {
    return this->mSocketAddress;
}

InputStream* TCPConnection::getInputStream() {
    return this->mInputStream.get();
}

OutputStream* TCPConnection::getOutputStream() {
    return this->mOutputStream.get();
}

ProtocolSyntax* TCPConnection::getProtocolSyntax() {
    return this->mSyntax.get();
}

void TCPConnection::connect(std::string hostname, std::string portString) {
    this->connect(hostname, atoi(portString.c_str()));
}

void TCPConnection::connect(std::string hostname, short port) {
    this->mSocketAddress.setPort(port);
    this->mSocketAddress.setupResolveHost(hostname);
    this->mSocketAddress.startResolveHost();
    
    this->connect();
}

void TCPConnection::connect() {
    this->mEventBase = event_base_new();
    auto event = evsignal_new(this->mEventBase, SIGPIPE, signalCallback, this);
    event_add(event, nullptr);
    
    this->mBufferEvent = bufferevent_socket_new(this->mEventBase, -1, BEV_OPT_THREADSAFE|BEV_OPT_CLOSE_ON_FREE);
    
    auto inputBufferAdapter = std::unique_ptr<Buffer>(new EvBufferAdapter(bufferevent_get_input(this->mBufferEvent)));
    auto outputBufferAdapter = std::unique_ptr<Buffer>(new EvBufferAdapter(bufferevent_get_output(this->mBufferEvent)));
    
    auto inputStream = std::unique_ptr<InputStream>(new InputStream(inputBufferAdapter));
    auto outputStream = std::unique_ptr<OutputStream>(new OutputStream(outputBufferAdapter));
    
    this->mInputStream = std::move(inputStream);
    this->mOutputStream = std::move(outputStream);
    
    if (!this->mBufferEvent) {
        throw SocketException::connectError;
        return;
    }
    bufferevent_setcb(this->mBufferEvent, readCallback, writeCallback, buffereventCallback, this);
    bufferevent_enable(this->mBufferEvent, EV_WRITE | EV_READ | EV_CLOSED | EV_SIGNAL);
    
    auto addr = this->mSocketAddress.getSockaddrIn();
    if (-1 == bufferevent_socket_connect(this->mBufferEvent, (struct sockaddr*)&addr, sizeof(addr))) {
        throw SocketException::connectError;
        return;
    }
}
void TCPConnection::start() {
    if (this->mEventBase != nullptr) {
        event_base_dispatch(this->mEventBase);
    }
}

void TCPConnection::stop() {
    if (this->mEventBase != nullptr) {
        event_base_loopbreak(this->mEventBase);
    }
}

void TCPConnection::close() {
    if (this->mBufferEvent != nullptr) {
        evutil_closesocket(bufferevent_getfd(this->mBufferEvent));
    }
}

void TCPConnection::shutdown(int how) {
    if (this->mBufferEvent != nullptr) {
        int fd = bufferevent_getfd(this->mBufferEvent);
        ::shutdown(fd, how);
    }
}

void TCPConnection::onSignalEvent(int fd, short what, void *arg) {
    this->mSyntax.get()->onSignalEvent(fd, what, arg);
}

void TCPConnection::onEOFEvent() {
    this->mSyntax.get()->onEOFEvent(this);
}

void TCPConnection::onErrorEvent() {
    this->mSyntax.get()->onError(this);
}

void TCPConnection::onConnectedEvent() {
    this->mSyntax.get()->feed(ts::ProtocolSyntax::Connect, this->getInputStream(), this->getOutputStream(), this);
}
void TCPConnection::onWriteError() {
    this->mSyntax.get()->willEndRead(this);
}

void TCPConnection::onReadError() {
    this->mSyntax.get()->willEndWrite(this);
}

void TCPConnection::onWriteableEvent(struct bufferevent *bev, void *ptr) {
    this->mSyntax.get()->onWriteable(this);
}

void TCPConnection::onReadableEvent(struct bufferevent *bev, void *ptr) {
    this->mSyntax.get()->feed(ts::ProtocolSyntax::Read, this->getInputStream(), this->getOutputStream(), this);
}


