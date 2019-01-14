//
// Created by CmST0us on 2019/1/14.
//

#pragma once

#include "SocketKit.hpp"
#include "CommunicatorInterface.hpp"

namespace socketkit {

enum class TCPConnectorEvent {
    Connected,

    Timeout,
    Errored
};

class TCPConnector final : public utils::IAsync {

public:
    TCPConnector();
    virtual ~TCPConnector();
    virtual utils::Runloop *getRunloop() override;

    void connect(std::shared_ptr<Endpoint> endpoint);

public:
    int mTimeoutSecond{5};  //单次连接超时时间
    int mRetrySecond{2};    //失败重连时间
    int mRetryTimes{3};     //重试次数

    using TCPConnectorEventHandler = std::function<void(TCPConnector *, TCPConnectorEvent, SocketFd socket)>;
    TCPConnectorEventHandler mEventHandler;

private:
    SocketFd _socket{-1};
    std::unique_ptr<utils::Runloop> _runloop;
    CommunicatorStateMachine _stateMachine;
    std::shared_ptr<Endpoint> _endpoint{nullptr};

    void setupRunloop();
    void initSocket();
    void closeSocket();
};

};