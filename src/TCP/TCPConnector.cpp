
#include "TCPConnector.hpp"

using namespace socketkit;

TCPConnector::TCPConnector() : _stateMachine{CommunicatorType::Remote} {
    setupRunloop();
}

TCPConnector::~TCPConnector() {
    // DO NOT CLOSE SOCKET!
}

utils::Runloop* TCPConnector::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void TCPConnector::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while (!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Establishing) {
                runloop->dispatch();
                continue;
            }

            sockaddr_in addr = _endpoint->getEndpointSockaddrIn();
            bool isConnect = false;
            bool isError = false;
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
                                isError = false;
                                break;
                            }
                        }
                    } else if (selectRet == 0){
                        // timeout
                        isConnect = false;
                        isError = false;
                        continue;
                    } else {
                        isConnect = false;
                        isError = true;
                    }
                } else if (ret == 0) {
                    isConnect = true;
                    isError = false;
                    break;
                } else {
                    isConnect = false;
                    isError = true;
                }
                std::this_thread::sleep_for(std::chrono::seconds(mRetrySecond));
            }

            if (isConnect && !isError) {
                _stateMachine.connected();
                mEventHandler(this, TCPConnectorEvent::Connected, _socket);
            } else if (!isConnect && !isError) {
                _stateMachine.errored();
                mEventHandler(this, TCPConnectorEvent::Timeout, -1);
            } else {
                _stateMachine.errored();
                mEventHandler(this, TCPConnectorEvent::Errored, -1);
            }

            runloop->dispatch();
        }
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void TCPConnector::initSocket() {
    if (_socket > 0) {
        closeSocket();
    }

    _socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket < 0) {
        _stateMachine.errored();
        throw SocketException::socketFdInitError;
        return;
    }

    utils::makeSocketNonblock(_socket);
}

void TCPConnector::closeSocket() {
#if _WIN32
        ::closesocket(_socket);
#else
        ::close(_socket);
#endif
}

void TCPConnector::connect(std::shared_ptr<Endpoint> endpoint) {
    getRunloop()->post([this, endpoint]() {
        _stateMachine.connectBegin();
        _endpoint = endpoint;
    });
}

