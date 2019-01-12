//
//  TCPConnection.hpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "NoCopyable.hpp"
#include "Endpoint.hpp"
#include "CommunicatorInterface.hpp"

namespace socketkit {

class TCPConnection final : public utils::NoCopyable,
                            public IRemoteCommunicator {

public:
    TCPConnection();
    virtual ~TCPConnection();

    virtual utils::Runloop* getRunloop() override;
    virtual void read(DataEventHandler handler) override;
    virtual void write(uchar *buffer, int &size) override;
    virtual void closeWrite() override;
    virtual const CommunicatorStateMachine& stateMachine() const override;
    virtual DataType communicatorDataType() const override;

    virtual void connect(std::shared_ptr<Endpoint> endpoint) override;
    virtual const Endpoint* connectingEndpoint() const override;

    void closeSocket();

public:
    int mTimeoutSecond{5};  //单次连接超时时间
    int mRetrySecond{2};    //失败重连时间
    int mRetryTimes{3};     //重试次数

    CommunicatorEventHandler mEventHandler;

private:
    SocketFd _socket{-1};
    std::shared_ptr<Endpoint> _endpoint;
    CommunicatorStateMachine _stateMachine;
    std::unique_ptr<utils::Runloop> _runloop;

    void setupRunloop();
    void initSocket();
    void runloop(utils::Runloop *runloop);


};

}

