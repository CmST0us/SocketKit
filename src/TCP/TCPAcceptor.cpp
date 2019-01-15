//
// Created by CmST0us on 2019/1/14.
//


#include "SocketException.hpp"
#include "TCPAcceptor.hpp"

using namespace socketkit;

TCPAcceptor::TCPAcceptor() : _stateMachine{CommunicatorType::Local} {
    initSocket();
    setupRunloop();
};

TCPAcceptor::~TCPAcceptor() {
    getRunloop()->stop();
    closeSocket();
}

utils::Runloop * TCPAcceptor::getRunloop() {
    utils::Runloop *runloop = _runloop.get();
    return runloop;
}

void TCPAcceptor::bind(std::shared_ptr<Endpoint> endpoint) {
    getRunloop()->post([this, endpoint]() {
        _endpoint = endpoint;
        Endpoint *ep = _endpoint.get();
        struct sockaddr_in addr = ep->getEndpointSockaddrIn();

        int ret = ::bind(_acceptorSocket, (const sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            _stateMachine.errored();
            throw SocketException::socketBindError;
        }
    });
}

void TCPAcceptor::listen(int backlog) {
    getRunloop()->post([this, backlog] {
        _stateMachine.connectBegin();
        int ret = ::listen(_acceptorSocket, backlog);
        if (ret < 0) {
            _stateMachine.errored();
            throw SocketException::socketListenError;
        }

        _stateMachine.connected();
    });
}
void TCPAcceptor::accept(TCPAcceptSocketHandler acceptorHandler) {
    getRunloop()->post([this, acceptorHandler]() {
        struct sockaddr_in acceptSocketAddrIn;
        socklen_t addrInLen = sizeof(acceptSocketAddrIn);

        SocketFd acceptSocket = ::accept(_acceptorSocket, (struct sockaddr *)&acceptSocketAddrIn, &addrInLen);
        std::shared_ptr<Endpoint> ep = Endpoint::create(acceptSocketAddrIn);
        std::shared_ptr<TCPSocket> s = std::make_shared<TCPSocket>(acceptSocket, ep);
        acceptorHandler(this, s);
    });
}

void TCPAcceptor::setupRunloop() {
    auto workRunloop = [this](utils::Runloop *runloop) {
        while (!runloop->isCanceled()) {
            if (_stateMachine.state() != CommunicatorState::Established) {
                runloop->dispatch(true);
                continue;
            }
            fd_set reads;
            int result;
            struct timeval timeout;
            FD_ZERO(&reads);
            FD_SET(_acceptorSocket, &reads);

            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            result = ::select((int)_acceptorSocket + 1, &reads, 0, 0, &timeout);
            if (result == -1) {
                //server error
                _stateMachine.errored();
                mEventHandler(this, TCPAcceptorEvent::Error);
            } else if (result == 0) {
                // timeout
            } else {
                if (FD_ISSET(_acceptorSocket, &reads)) {
                    // accept
                    mEventHandler(this, TCPAcceptorEvent::CanAccept);
                }
            }

            runloop->dispatch();
        }
    };

    _runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop(workRunloop));
}

void TCPAcceptor::initSocket() {
    if (_acceptorSocket > 0) {
        closeSocket();
    }

    _acceptorSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_acceptorSocket < 0) {
        _stateMachine.errored();
        throw SocketException::socketFdInitError;
    }

    utils::makeSocketNonblock(_acceptorSocket);

    int option = true;
    socklen_t optionLen = sizeof(option);

    struct linger l;
    l.l_linger = 0;
    l.l_onoff = 1;
    int intval = 1;

#if _WIN32
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, optionLen);
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(struct linger));
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&intval, sizeof(int));
    ::setsockopt(_acceptorSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&intval, sizeof(int));
#else
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optionLen);
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int));
    ::setsockopt(_acceptorSocket, IPPROTO_TCP, TCP_NODELAY, &intval, sizeof(int));
    ::setsockopt(_acceptorSocket, SOL_SOCKET, SO_NOSIGPIPE, &intval, sizeof(int));
#endif

}

void TCPAcceptor::closeSocket() {
#if _WIN32
        ::closesocket(_acceptorSocket);
#else
        ::close(_acceptorSocket);
#endif
}