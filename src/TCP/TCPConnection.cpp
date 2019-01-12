//
//  TCPConnection.cpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include <iostream>

#include "Runloop.hpp"
#include "SocketKit.hpp"
#include "TCPConnection.hpp"
#include "SocketException.hpp"

using namespace socketkit;

TCPConnection::TCPConnection() : _endpoint{nullptr},
                                 _stateMachine{CommunicatorType::Remote} {
    setupRunloop();
}

TCPConnection::~TCPConnection() {

}

void TCPConnection::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while(!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Established) {
                continue;
            }

            fd_set readSet;
            fd_set writeSet;
            fd_set errorSet;
            FD_ZERO(&readSet);
            FD_ZERO(&writeSet);
            FD_ZERO(&errorSet);
            FD_SET(_socket, &readSet);
            FD_SET(_socket, &writeSet);
            FD_SET(_socket, &errorSet);

            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int ret = ::select(_socket + 1, &readSet, &writeSet, &errorSet, &tv);
            if (ret > 0) {
                // socket have event
                if (FD_ISSET(_socket, &readSet)) {
                    // can read;
                    // peek 1 byte to check if socket closed;
                    uchar peekByte = {0};
#if _WIN32
                    int recvLen = ::recv(_socket, (char *)&peekByte, 1, MSG_PEEK);
#else
                    int recvLen = ::recv(_socket, &peekByte, 1, MSG_PEEK);
#endif
                    if (recvLen == 0) {
                        // endpoint write closed;
                        // close read
                        _stateMachine.readClosed();
                        mEventHandler(CommunicatorEvent::EndEncountered);
                    } else if (recvLen < 0) {
                        _stateMachine.errored();
                        mEventHandler(CommunicatorEvent::ErrorOccurred);
                    } else {
                        mEventHandler(CommunicatorEvent::HasBytesAvailable);
                    }
                } else if (FD_ISSET(_socket, &writeSet)) {
                    // can write
                    mEventHandler(CommunicatorEvent::HasSpaceAvailable);
                } else if (FD_ISSET(_socket, &errorSet)) {
                    // error
                    _stateMachine.errored();
                    closeSocket();
                    mEventHandler(CommunicatorEvent::ErrorOccurred);
                } else {
                    // no event
                    mEventHandler(CommunicatorEvent::None);
                }
            } else if (ret < 0) {
                // error happen
                _stateMachine.errored();
                closeSocket();
                mEventHandler(CommunicatorEvent::ErrorOccurred);
            }

            runloop->dispatch();
        }
    };


    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void TCPConnection::initSocket() {
    if (_socket > 0) {
        closeSocket();
    }

    _socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket < 0) {
        _stateMachine.errored();
        return;
    }

    utils::makeSocketNonblock(_socket);
}

utils::Runloop* TCPConnection::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void TCPConnection::closeSocket() {
#if _WIN32
    ::closesocket(_socket);
#else
    ::close(_socket);
#endif
}

void TCPConnection::read(DataEventHandler handler) {
    getRunloop()->post([this, handler](){
        uchar buffer[10240] = {0};
        int size = 10240;
        _stateMachine.readBegin();
#if _WIN32
        int readLen = (int)::recv(_socket, (char *)buffer, size, 0);
#else
        int readLen = (int)::recv(_socket, buffer, size, 0);
#endif
        size = readLen;
        _stateMachine.readEnd();
        handler(buffer, size);
    });
}

void TCPConnection::write(uchar *buffer, int &size) {
    _stateMachine.writeBegin();
    getRunloop()->post([this, buffer, &size](){
#if _WIN32
        int writeLen = (int)::send(_socket, (char *)buffer, size, 0);
#else
        int writeLen = (int)::send(_socket, buffer, size, 0);
#endif
        size = writeLen;
        _stateMachine.writeEnd();
    });
}

void TCPConnection::closeWrite() {
    _stateMachine.writeCloseBegin();
    shutdown(_socket, SHUT_WR);
    _stateMachine.writeCloseEnd();
}

void TCPConnection::connect(std::shared_ptr<Endpoint> endpoint) {
    _stateMachine.connectBegin();

    _endpoint = endpoint;
    sockaddr_in addr = _endpoint->getEndpointSockaddrIn();
    bool isConnect = false;
    // connecting
    for (int retryTimes = 0; retryTimes < mRetryTimes; retryTimes++) {
        initSocket();
        int ret = ::connect(_socket, (const sockaddr *)&addr, sizeof(addr));
        if (ret < 0 && errno == EINPROGRESS) {
            fd_set writeSet;
            fd_set readSet;
            FD_ZERO(&writeSet);
            FD_ZERO(&readSet);
            FD_SET(_socket, &writeSet);
            FD_SET(_socket, &readSet);
            struct timeval tv;
            tv.tv_sec = mTimeoutSecond;
            tv.tv_usec = 0;

            int selectRet = ::select(_socket + 1, &readSet, &writeSet, NULL, &tv);
            if (selectRet > 0) {
                // connect succeed
                if (FD_ISSET(_socket, &writeSet)) {
                    if (!FD_ISSET(_socket, &readSet)) {
                        isConnect = true;
                        break;
                    }
                }
            } else {
                continue;
            }
        } else if (ret == 0) {
            isConnect = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(mRetrySecond));
    }

    if (isConnect) {
        _stateMachine.connected();
        mEventHandler(CommunicatorEvent::OpenCompleted);
    } else {
        _stateMachine.errored();
        mEventHandler(CommunicatorEvent::ErrorOccurred);
    }
}

const CommunicatorStateMachine & TCPConnection::stateMachine() const {
    return _stateMachine;
}

DataType TCPConnection::communicatorDataType() const {
    return DataType::Stream;
}

const Endpoint* TCPConnection::connectingEndpoint() const {
    return _endpoint.get();
}