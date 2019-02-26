
#include "UsbmuxdConnector.hpp"

#define kUsbmuxdConnectorDefaultUsbmuxdPath "/var/run/usbmuxd"

using namespace socketkit;

UsbmuxdConnector::UsbmuxdConnector(UsbmuxdProtocol *protocol) : _stateMachine{CommunicatorType::Remote},
                                                                _protocol{protocol} {
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
            fd_set readSet;
            fd_set errorSet;
            FD_ZERO(&readSet);
            FD_ZERO(&errorSet);
            FD_SET(_socket, &readSet);
            FD_SET(_socket, &errorSet);

            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int ret = ::select(_socket + 1, &readSet, NULL, &errorSet, &tv);
            if (ret > 0) {
                // socket have event
                if (FD_ISSET(_socket, &readSet)) {
                    // read event
                    if (_stateMachine.state() == CommunicatorState::Established) {
                        runloop->stop();
                    } else if (_stateMachine.state() == CommunicatorState::Establishing) {
                        // Check Connect Result
                        UsbmuxdResultMessage msg = {0};
                        ::recv(_socket, &msg, sizeof(msg), 0);
                        _protocol->recvResultMessage(msg);
                    } else if (_stateMachine.state() == CommunicatorState::Init) {
                        // Wait Device Record
                        UsbmuxdDeviceRecordMessage record = {0};
                        ::recv(_socket, &record, sizeof(record), 0);
                        printf("Device ID: %d", record.record.deviceId);
                    } else {
                        // error
                    }
                } else if (FD_ISSET(_socket, &errorSet)) {
                    // error event

                } else {
                    // no event

                }
            } else if (ret < 0) {
                // error
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
        uint32_t deviceId = atol(endpoint->getEndpointDomain().c_str());
        uint16_t port = endpoint->getEndpointPort();
        _endpoint = endpoint;
        _protocol->makeConnectRequestWithHandler(deviceId, port, [this](UsbmuxdHeader req, UsbmuxdResultMessage res) {
            if (res.result == (uint32_t)UsbmuxdResult::OK) {
                _stateMachine.connected();
                if (mEventHandler != nullptr) {
                    mEventHandler(this, UsbmuxdConnectorEvent::Connected, _socket);
                }
            }
        });
    });
}

void UsbmuxdConnector::listenDevice() {
    UsbmuxdListenRequest request = _protocol->makeListenRequestWithHandler([this](UsbmuxdHeader req, UsbmuxdResultMessage res) {
        if (res.result != (uint32_t)UsbmuxdResult::OK) {
            _stateMachine.errored();
        }
    });
    getRunloop()->post([this, request]() {
        ::send(this->_socket, &request, sizeof(request), 0);
    });
}

uint32_t UsbmuxdConnector::numberOfAttachDevices() {
    return this->_devices.size();
}

std::vector<UsbmuxdDeviceRecord> UsbmuxdConnector::attachedDevices() const {
    return this->_devices;
}