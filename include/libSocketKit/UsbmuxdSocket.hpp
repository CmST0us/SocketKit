//
// Created by CmST0us on 2019/2/25.
//

#pragma once

#include "SocketKit.hpp"
#include "CommunicatorInterface.hpp"
#include "UsbmuxdConnector.hpp"

namespace socketkit {

class UsbmuxdSocket final : public IRemoteCommunicator {

public:
    UsbmuxdSocket();
    virtual ~UsbmuxdSocket();

    virtual utils::Runloop *getRunloop() override;
    virtual void read(DataEventHandler handler) override;
    virtual void write(std::shared_ptr<utils::Data> data) override;
    virtual void closeWrite() override;
    virtual const CommunicatorStateMachine& stateMachine() const override;
    virtual DataType communicatorDataType() const override;

    virtual void connect(std::shared_ptr<Endpoint> endpoint) override;
    virtual const Endpoint *connectingEndpoint() const override;

    unsigned short localPort() const;
    const SocketFd getSocketFd() const;

    CommunicatorEventHandler mEventHandler;

private:
    SocketFd _socket{(SocketFd)-1};
    std::unique_ptr<UsbmuxdConnector> _connector{nullptr};
    std::unique_ptr<UsbmuxdProtocol> _protocol{nullptr};
    std::unique_ptr<utils::Runloop> _runloop;
    CommunicatorStateMachine _stateMachine;
    std::shared_ptr<Endpoint> _endpoint;

    void setupRunloop();
    void setupProtocol();
    void setupConnector();
    void closeSocket();

};

}