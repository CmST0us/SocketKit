//
// Created by CmST0us on 2019/2/23.
//

#pragma once

#include "SocketKit.hpp"
#include "CommunicatorInterface.hpp"
#include "UsbmuxdProtocol.hpp"

namespace socketkit {

enum class UsbmuxdConnectorEvent {
    Connected,
    DeviceAttached,
    DeviceDetached,
    Timeout,
    Errored
};

class UsbmuxdConnector final : public utils::IAsync {

public:
    UsbmuxdConnector(UsbmuxdProtocol *protocol);
    virtual ~UsbmuxdConnector();
    virtual utils::Runloop *getRunloop() override;

    void connect(std::shared_ptr<Endpoint> endpoint);

public:
    int mTimeoutSecond{5};  //单次连接超时时间
    int mRetrySecond{2};    //失败重连时间
    int mRetryTimes{3};     //重试次数

    using UsbmuxdConnectorEventHandler = std::function<void(UsbmuxdConnector *, UsbmuxdConnectorEvent , SocketFd socket)>;
    UsbmuxdConnectorEventHandler mEventHandler{nullptr};

private:
    SocketFd _socket{(SocketFd)-1};
    std::unique_ptr<utils::Runloop> _runloop;
    CommunicatorStateMachine _stateMachine;
    std::shared_ptr<Endpoint> _endpoint{nullptr};
    UsbmuxdProtocol *_protocol;

    void initSocket();
    void initProtocol();
    void setupRunloop();
    void closeSocket();
};

}