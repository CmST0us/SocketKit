//
// Created by CmST0us on 2019/1/14.
//

#pragma once

#include <memory>
#include <functional>

#include "CommunicatorStateMachine.hpp"
#include "AsyncInterface.hpp"
#include "SocketKit.hpp"
#include "TCPSocket.hpp"

namespace socketkit {

enum class TCPAcceptorEvent {
    Accepted,

    Error
};

class TCPAcceptor final : public utils::IAsync {
public:
    TCPAcceptor();
    virtual ~TCPAcceptor();
    virtual utils::Runloop *getRunloop();

    using TCPAcceptorEventHandler = std::function<void(TCPAcceptor *, TCPAcceptorEvent)>;
    using TCPAcceptSocketHandler = std::function<void(TCPAcceptor *, std::shared_ptr<TCPSocket>)>;
    TCPAcceptorEventHandler mEventHandler;

    void bind(std::shared_ptr<Endpoint> endpoint);
    void listen(int backlog);
    void accept(TCPAcceptSocketHandler);

private:
    SocketFd _acceptorSocket{-1};
    CommunicatorStateMachine _stateMachine;
    std::shared_ptr<Endpoint> _endpoint{nullptr};
    std::unique_ptr<utils::Runloop> _runloop{nullptr};

    void setupRunloop();
    void initSocket();
    void closeSocket();
};

};
