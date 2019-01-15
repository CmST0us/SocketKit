

#include "TCPSocket.hpp"

using namespace socketkit;

TCPSocket::TCPSocket() : _stateMachine{CommunicatorType::Remote} {
    setupRunloop();
}

TCPSocket::TCPSocket(SocketFd socket, std::shared_ptr<Endpoint> ep) : _socket{socket},
                                                                      _endpoint{ep},
                                                                      _stateMachine{CommunicatorType::Local} {
    setupRunloop();
}

utils::Runloop* TCPSocket::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void TCPSocket::read(DataEventHandler handler) {
    getRunloop()->post([this, handler](){
        uchar buffer[10240] = {0};
        int size = 10240;
        auto data = std::make_shared<utils::Data>(size);
        _stateMachine.readBegin();
#if _WIN32
        int readLen = (int)::recv(_socket, (char *)buffer, size, 0);
#else
        int readLen = (int)::recv(_socket, buffer, size, 0);
#endif
        size = readLen;
        if (readLen > 0) {
            data->copy(buffer, size);
            _stateMachine.readEnd();
            handler(data);
        } else {
            _stateMachine.errored();
            mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
        }
    });
}

void TCPSocket::write(std::shared_ptr<utils::Data> data) {
    getRunloop()->post([this, data](){
        _stateMachine.writeBegin();
#if _WIN32
        ::send(_socket, (char *)data->getDataAddress, data->getDataSize(), 0);
#else
        ::send(_socket, data->getDataAddress(), data->getDataSize(), 0);
#endif
        _stateMachine.writeEnd();
    });
}

void TCPSocket::closeWrite() {
    getRunloop()->post([this]() {
        _stateMachine.writeCloseBegin();
        shutdown(_socket, SHUT_WR);
        _stateMachine.writeCloseEnd();
    });
}

const CommunicatorStateMachine& TCPSocket::stateMachine() const {
    return _stateMachine;
}

DataType TCPSocket::communicatorDataType() const {
    return DataType::Stream;
}

void TCPSocket::connect(std::shared_ptr<Endpoint> endpoint) {
    _connector = std::unique_ptr<TCPConnector>(new TCPConnector());
    _endpoint = endpoint;
    TCPConnector *connector = _connector.get();
    connector->mEventHandler = [this](TCPConnector *connector, TCPConnectorEvent event, SocketFd socket) {
        switch (event) {
            case TCPConnectorEvent::Connected :
                _socket = socket;
                _stateMachine.connected();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::OpenCompleted);
                connector->getRunloop()->post([connector]() {
                    connector->getRunloop()->stop();
                });
                break;
            default:
                _stateMachine.errored();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
                break;
        }
    };

    _stateMachine.connectBegin();
    connector->getRunloop()->run();

    connector->connect(endpoint);
}

const Endpoint* TCPSocket::connectingEndpoint() const {
    return _endpoint.get();
}

const SocketFd TCPSocket::getSocketFd() const {
    return _socket;
}

void TCPSocket::open() {
    getRunloop()->post([this]() {
        _stateMachine.connectBegin();
    });

}

void TCPSocket::continueFinished() {
    getRunloop()->post([this]() {
        _stateMachine.connected();
    });
}

void TCPSocket::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while(!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Established) {
                if (_stateMachine.state() == CommunicatorState::Closed) {
                    runloop->stop();
                }
                runloop->dispatch(true);
                continue;
            }

            fd_set readSet;
            fd_set writeSet;
            fd_set errorSet;
            FD_ZERO(&readSet);
//            FD_ZERO(&writeSet);
            FD_ZERO(&errorSet);
            FD_SET(_socket, &readSet);
//            FD_SET(_socket, &writeSet);
            FD_SET(_socket, &errorSet);

            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int ret = ::select(_socket + 1, &readSet, NULL, &errorSet, &tv);
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
                        mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::EndEncountered);
                    } else if (recvLen < 0) {
                        _stateMachine.errored();
                        mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
                    } else {
                        mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::HasBytesAvailable);
                    }
                } else if (FD_ISSET(_socket, &writeSet)) {
                    // can write
                    mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::HasSpaceAvailable);
                } else if (FD_ISSET(_socket, &errorSet)) {
                    // error
                    _stateMachine.errored();
                    closeSocket();
                    mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
                } else {
                    // no event
                    mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::None);
                }
            } else if (ret < 0) {
                // error happen
                _stateMachine.errored();
                closeSocket();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
            }

            runloop->dispatch();
        }
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));

}

void TCPSocket::closeSocket() {
#if _WIN32
        ::closesocket(_socket);
#else
        ::close(_socket);
#endif
}
