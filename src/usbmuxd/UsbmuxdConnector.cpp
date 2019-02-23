
#include "UsbmuxdConnector.hpp"

#define kUsbmuxdConnectorDefaultUsbmuxdPath "/var/run/usbmuxd"

using namespace socketkit;

UsbmuxdConnector::UsbmuxdConnector() : _stateMachine{CommunicatorType::Remote} {
    initSocket();
    setupRunloop();
}

UsbmuxdConnector::~UsbmuxdConnector() {
    // DO NOT CLOSE SOCKET!
}

utils::Runloop* UsbmuxdConnector::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void UsbmuxdConnector::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while (!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Establishing) {
                runloop->dispatch(true);
                continue;
            }

            runloop->dispatch();
        }
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void UsbmuxdConnector::initSocket() {
    if (_socket > 0) {
        closeSocket();
    }

    _socket = ::socket(PF_UNIX, SOCK_STREAM, 0);
    if (_socket < 0) {
        _stateMachine.errored();
        throw SocketException::socketFdInitError;
        return;
    }

    int intval = 1;
#if _WIN32

#else
    ::setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &intval, sizeof(int));
#endif
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, kUsbmuxdConnectorDefaultUsbmuxdPath);
    socklen_t socklen = sizeof(addr);

    if (::connect(_socket, (struct sockaddr*)&addr, socklen) < 0) {
        _stateMachine.errored();
        throw SocketException::usbmuxdFdCannotConnect;
    }

    utils::makeSocketNonblock(_socket);
}

void UsbmuxdConnector::closeSocket() {
#if _WIN32
    ::closesocket(_socket);
#else
    ::close(_socket);
#endif
}

void UsbmuxdConnector::connect(std::shared_ptr<Endpoint> endpoint) {
    getRunloop()->post([this, endpoint]() {
        _stateMachine.connectBegin();
        _endpoint = endpoint;
    });
}

