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
    getRunloop()->post([this, data](){
        _stateMachine.writeBegin();
#if _WIN32
        ::send(_socket, (const char *)data->getDataAddress(), (int)data->getDataSize(), 0);
#else
        ::send(_socket, data->getDataAddress(), data->getDataSize(), 0);
#endif
        _stateMachine.writeEnd();
    });
}

void UsbmuxdSocket::closeWrite() {
    getRunloop()->post([this]() {
        _stateMachine.writeCloseBegin();
        ::shutdown(_socket, SHUT_WR);
        _stateMachine.writeCloseEnd();
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

UsbmuxdConnector* UsbmuxdSocket::getConnector() {
    return _connector.get();
}

void UsbmuxdSocket::setupRunloop() {

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




