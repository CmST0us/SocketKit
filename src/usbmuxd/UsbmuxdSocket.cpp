//
// Created by CmST0us on 2019/2/25.
//

#include "UsbmuxdSocket.hpp"

using namespace socketkit;

UsbmuxdSocket::UsbmuxdSocket() : _stateMachine{CommunicatorType::Remote} {
    setupProtocol();
    setupConnector();
    setupRunloop();
}

UsbmuxdSocket::~UsbmuxdSocket() {

}

utils::Runloop* UsbmuxdSocket::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void UsbmuxdSocket::read(DataEventHandler handler) {
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
        if (readLen > 0) {
            auto data = std::make_shared<utils::Data>(size);
            data->copy(buffer, size);
            _stateMachine.readEnd();
            handler((ICommunicator *)(IRemoteCommunicator *)this, (data));
        } else {
            _stateMachine.errored();
            mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
        }
    });
}

void UsbmuxdSocket::write(std::shared_ptr<utils::Data> data) {
    _stateMachine.writeBegin();
#if _WIN32
    ::send(_socket, (const char *)data->getDataAddress(), (int)data->getDataSize(), 0);
#else
    ::send(_socket, data->getDataAddress(), data->getDataSize(), 0);
#endif
    _stateMachine.writeEnd();
}

void UsbmuxdSocket::closeWrite() {
    getRunloop()->post([this]() {
        _stateMachine.writeCloseBegin();
        ::shutdown(_socket, SHUT_WR);
        _stateMachine.writeCloseEnd();
    });
}

void UsbmuxdSocket::close() {
    getRunloop()->post([this]() {
        _stateMachine.closed();
        this->closeSocket();
    });
}

const CommunicatorStateMachine& UsbmuxdSocket::stateMachine() const {
    return _stateMachine;
}

DataType UsbmuxdSocket::communicatorDataType() const {
    return DataType::Stream;
}

void UsbmuxdSocket::connect(std::shared_ptr<Endpoint> endpoint) {
    _endpoint = endpoint;
    _stateMachine.connectBegin();
    UsbmuxdConnector *connector = _connector.get();
    connector->connect(endpoint);
}

const Endpoint* UsbmuxdSocket::connectingEndpoint() const {
    return _endpoint.get();
}

const SocketFd UsbmuxdSocket::getSocketFd() const {
    return _socket;
}

void UsbmuxdSocket::setupRunloop() {
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

        closeSocket();
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void UsbmuxdSocket::setupConnector() {
    _connector = std::unique_ptr<UsbmuxdConnector>(new UsbmuxdConnector(_protocol.get()));
    UsbmuxdConnector *connector = _connector.get();
    connector->mEventHandler = [this](UsbmuxdConnector *connector, UsbmuxdConnectorEvent event, SocketFd socket) {
        switch (event) {
            case UsbmuxdConnectorEvent::Connected :
                _socket = socket;
                _stateMachine.connected();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::OpenCompleted);
                // post 一个空任务唤醒runloop，防止socket传递之后没有任何操作就关闭，然后导致EOF不回调
                _runloop->post([]() {

                });
                connector->getRunloop()->stop();
                break;
            default:
                _stateMachine.errored();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
                // post 一个空任务唤醒runloop，防止socket传递之后没有任何操作就关闭，然后导致EOF不回调
                _runloop->post([]() {

                });
                connector->getRunloop()->stop();
                break;
        }
    };
    connector->getRunloop()->run();
}

void UsbmuxdSocket::setupProtocol() {
    _protocol = std::unique_ptr<UsbmuxdProtocol>(new UsbmuxdProtocol());
}

void UsbmuxdSocket::closeSocket() {
    utils::makeSocketBlock(_socket);
#if _WIN32
    ::closesocket(_socket);
#else
    ::close(_socket);
#endif
}




