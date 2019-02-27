//
// Created by CmST0us on 2019/2/26.
//

#pragma once

#include "SocketKit.hpp"
#include "AsyncInterface.hpp"
#include "CommunicatorInterface.hpp"
#include "UsbmuxdProtocol.hpp"

namespace socketkit {

class UsbmuxdDeviceListener final: public utils::IAsync {
public:
    UsbmuxdDeviceListener();
    virtual ~UsbmuxdDeviceListener();
    virtual utils::Runloop *getRunloop() override;

    using UsbmuxdDeviceListenerHandler = std::function<void(bool isAttach, UsbmuxdDeviceRecord record)>;
    UsbmuxdDeviceListenerHandler mDeviceListenerHandler = {nullptr};

    void startListenDevice();
    uint32_t numberOfAttachDevices();
    std::map<uint32_t, UsbmuxdDeviceRecord> attachedDevices() const;

private:
    SocketFd _socket{(SocketFd)-1};
    std::unique_ptr<utils::Runloop> _runloop;
    CommunicatorStateMachine _stateMachine;

    std::unique_ptr<UsbmuxdProtocol> _protocol;
    std::map<uint32_t, UsbmuxdDeviceRecord> _devices;

    void initSocket();
    void initProtocol();
    void setupRunloop();
    void closeSocket();
};

}
