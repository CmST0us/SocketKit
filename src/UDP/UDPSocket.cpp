//
// Created by CmST0us on 2019/1/14.
//

#include "SocketException.hpp"
#include "UDPSocket.hpp"

using namespace socketkit;

UDPSocket::UDPSocket(short localPort) : _localPort{localPort},
                                        _stateMachine{CommunicatorType::Remote} {
    initSocket();
    setupRunloop();
}

UDPSocket::~UDPSocket() {

}

void UDPSocket::read(DataEventHandler handler) {
    getRunloop()->post([this, handler]() {
        uchar buf[1500] = {0};
        int size = 1500;
        auto data = std::make_shared<utils::Data>(size);
        struct sockaddr_in recvSocketAddrIn;
        socklen_t addrInLen = sizeof(recvSocketAddrIn);

        _stateMachine.readBegin();
#if _WIN32
        ssize_t recvLen = ::recvfrom(_socket, (char *)buf, 1500, 0, (struct sockaddr*)&recvSocketAddrIn, &addrInLen);
#else
        ssize_t recvLen = ::recvfrom(_socket, buf, 1500, 0, (struct sockaddr*)&recvSocketAddrIn, &addrInLen);
#endif
        size = recvLen;
        if (size > 0) {
            data->copy(buf, size);
            _stateMachine.readEnd();
            handler(data);
        } else {
            _stateMachine.errored();
            mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
        }
    });
}

void UDPSocket::write(std::shared_ptr<utils::Data> data) {
    getRunloop()->post([this, data]() {
        _stateMachine.writeBegin();
        sockaddr_in sockaddrIn = _endpoint->getEndpointSockaddrIn();
#if _WIN32
        ::sendto(_socket, (char *)data->getDataAddress(), data->getDataSize(), 0, (struct sockaddr *)&sockaddrIn, sizeof(sockaddrIn));
#else
        ::sendto(_socket, data->getDataAddress(), data->getDataSize(), 0, (struct sockaddr *)&sockaddrIn, sizeof(sockaddrIn));
#endif
        _stateMachine.writeEnd();
    });
}

void UDPSocket::closeWrite() {
    getRunloop()->post([this]() {
        _stateMachine.writeCloseBegin();
        _stateMachine.writeCloseEnd();
    });
}

const CommunicatorStateMachine& UDPSocket::stateMachine() const {
    return _stateMachine;
}

DataType UDPSocket::communicatorDataType() const {
    return DataType::Packet;
}

void UDPSocket::connect(std::shared_ptr<Endpoint> endpoint) {
    getRunloop()->post([this, endpoint]() {
        _stateMachine.connectBegin();
        _endpoint = endpoint;
        _stateMachine.connected();
    });
}

const Endpoint* UDPSocket::connectingEndpoint() const {
    return _endpoint.get();
}

const SocketFd UDPSocket::getSocketFd() const {
    return _socket;
}

short UDPSocket::localPort() const {
    return _localPort;
}

utils::Runloop* UDPSocket::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}
void UDPSocket::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while (!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Established) {
                runloop->dispatch();
                continue;
            }

            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(_socket, &readSet);
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int ret = ::select(_socket + 1, &readSet, NULL, NULL, &tv);
            if (ret > 0) {
                if (FD_ISSET(_socket, &readSet)) {
                    mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::HasBytesAvailable);
                } else {
                    mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::None);
                }
            } else if (ret == 0) {

            } else {
                _stateMachine.errored();
                mEventHandler((ICommunicator *)(IRemoteCommunicator *)this, CommunicatorEvent::ErrorOccurred);
            }

            runloop->dispatch();
        }
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void UDPSocket::initSocket() {
    if (_socket > 0) {
        closeSocket();
    }

    _socket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (_socket < 0) {
        _stateMachine.errored();
        throw SocketException::socketFdInitError;
    }

    utils::makeSocketNonblock(_socket);

    struct sockaddr_in bindAddr = {0};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(_localPort);
    bindAddr.sin_addr.s_addr =  INADDR_ANY;

    int ret = ::bind(_socket, (const struct sockaddr *)&bindAddr, sizeof(bindAddr));
    if (ret < 0) {
        _stateMachine.errored();
        throw SocketException::socketBindError;
    }
}

void UDPSocket::closeSocket() {
#if _WIN32
    ::closesocket(_socket);
#else
    ::close(_socket);
#endif
}
