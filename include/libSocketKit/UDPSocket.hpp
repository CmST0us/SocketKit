//
// Created by CmST0us on 2019/1/14.
//

#pragma once

#include "CommunicatorInterface.hpp"
#include "SocketKit.hpp"

namespace socketkit {

class UDPSocket final : public IRemoteCommunicator {

public:
    UDPSocket(unsigned short localPort = 0);
    virtual ~UDPSocket();

    virtual utils::Runloop *getRunloop() override;
    virtual void read(DataEventHandler handler) override;
    virtual void write(std::shared_ptr<utils::Data> data) override;
    virtual void closeWrite() override;
    virtual void close() override;
    virtual const CommunicatorStateMachine& stateMachine() const override;
    virtual DataType communicatorDataType() const override;

    virtual void connect(std::shared_ptr<Endpoint> endpoint) override;
    virtual const Endpoint *connectingEndpoint() const override;

    unsigned short localPort() const;
    const SocketFd getSocketFd() const;

    CommunicatorEventHandler mEventHandler;

private:
    SocketFd _socket{(SocketFd)-1};
    std::unique_ptr<utils::Runloop> _runloop;
    CommunicatorStateMachine _stateMachine;
    std::shared_ptr<Endpoint> _endpoint;
    unsigned short _localPort;

    void setupRunloop();
    void initSocket();
    void closeSocket();

};

};
