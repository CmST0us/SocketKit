//
// Created by CmST0us on 2019/1/14.
//

#pragma once

#include <memory>

#include "TCPConnector.hpp"
#include "CommunicatorInterface.hpp"
#include "SocketKit.hpp"

namespace socketkit {

class TCPSocket final : public IRemoteCommunicator,
                        public ILocalCommunicator {

public:
    virtual utils::Runloop* getRunloop() override;
    virtual void read(DataEventHandler handler) override;
    virtual void write(std::shared_ptr<utils::Data> data) override;
    virtual void closeWrite() override;
    virtual const CommunicatorStateMachine& stateMachine() const override;
    virtual DataType communicatorDataType() const override;
    void closeSocket();

    // Remote
    TCPSocket();
    virtual void connect(std::shared_ptr<Endpoint> endpoint) override;
    virtual const Endpoint *connectingEndpoint() const override;

    // Local
    TCPSocket(SocketFd socket, std::shared_ptr<Endpoint>);

    virtual void open() override;
    virtual void continueFinished() override;

public:
    CommunicatorEventHandler mEventHandler;
private:
    SocketFd _socket{-1};
    std::unique_ptr<TCPConnector> _connector{nullptr};

    std::shared_ptr<Endpoint> _endpoint;
    CommunicatorStateMachine _stateMachine;
    std::unique_ptr<utils::Runloop> _runloop;

    void setupRunloop();
};

};