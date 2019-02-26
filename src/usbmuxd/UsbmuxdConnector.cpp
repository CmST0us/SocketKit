
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
                        // connect finish switch socket owner to UsbmuxdSocket
                        runloop->stop();
                    } else if (_stateMachine.state() == CommunicatorState::Init ||
                               _stateMachine.state() == CommunicatorState::Establishing) {
                        // Wait Device Record
                        UsbmuxdHeader header = {0};
                        ::recv(_socket, &header, sizeof(header), MSG_PEEK);
                        uint32_t messageLen = header.length;
                        uint8_t *data = (uint8_t *)malloc(messageLen);
                        memset(data, 0, messageLen);
                        ::recv(_socket, data, messageLen, 0);
                        _protocol->parsePlistPayloadMessage(header, data + sizeof(header), messageLen - sizeof(header));
                        free(data);
                    } else {
                        // error
                        runloop->stop();
                    }
                } else if (FD_ISSET(_socket, &errorSet)) {
                    // error event
                    runloop->stop();
                } else {
                    // no event

                }
            } else if (ret < 0) {
                // error
                runloop->stop();
            } else {
                if (_stateMachine.state() == CommunicatorState::Closed) {
                    runloop->stop();
                }
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
    _stateMachine.connectBegin();
    uint32_t deviceId = (uint32_t)atoi(endpoint->getEndpointDomain().c_str());
    uint16_t port = endpoint->getEndpointPort();
    _endpoint = endpoint;
    std::shared_ptr<UsbmuxdPayloadData> data = _protocol->makeConnectRequestWithHandler(deviceId, port, [this](UsbmuxdHeader req, UsbmuxdResultMessage res) {
        if (res.result == (uint32_t)UsbmuxdResult::OK) {
            printf("Connect OK\n");
            _stateMachine.connected();
            if (mEventHandler != nullptr) {
                mEventHandler(this, UsbmuxdConnectorEvent::Connected, _socket);
            }
        } else {
            printf("Connect Error\n");
            _stateMachine.errored();
        }
    });
    getRunloop()->post([this, data]() {
        ::send(_socket, data->getDataAddress(), data->dataLength(), 0);
    });
}

void UsbmuxdConnector::listenDevice() {
    _protocol->mDeviceRecordHandler = [this](bool isAttach, UsbmuxdDeviceRecord record) {
        if (isAttach) {
            _devices[record.deviceId] = record;
            if (mEventHandler) {
                mEventHandler(this, UsbmuxdConnectorEvent::DeviceAttached, _socket);
            }
        } else {
            if (_devices.count(record.deviceId) > 0) {
                _devices.erase(record.deviceId);
            }
            if (mEventHandler) {
                mEventHandler(this, UsbmuxdConnectorEvent::DeviceDetached, _socket);
            }
        }
    };

    std::shared_ptr<UsbmuxdPayloadData> data = _protocol->makeListenRequestWithHandler([this](UsbmuxdHeader req, UsbmuxdResultMessage res) {
        if (res.result != (uint32_t)UsbmuxdResult::OK) {
            _stateMachine.errored();
            if (mEventHandler) {
                mEventHandler(this, UsbmuxdConnectorEvent::Errored, _socket);
            }
        } else {
            printf("Listen Request OK\n");
        }
    });

    getRunloop()->post([this, data]() {
        ::send(this->_socket, data->getDataAddress(), data->dataLength(), 0);
    });
}

uint32_t UsbmuxdConnector::numberOfAttachDevices() {
    return this->_devices.size();
}

std::map<uint32_t, UsbmuxdDeviceRecord> UsbmuxdConnector::attachedDevices() const {
    return this->_devices;
}