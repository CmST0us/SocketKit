//
//  CommunicatorInterface.hpp
//  SocketKit
//
//  Created by CmST0us on 2018/10/12.
//  Copyright © 2018年 CmST0us. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <memory>

#include "Endpoint.hpp"
#include "SocketKit.hpp"
#include "AsyncInterface.hpp"
#include "CommunicatorStateMachine.hpp"
#include "Data.hpp"

typedef unsigned char uchar;

namespace socketkit
{

enum class CommunicatorEvent {
    None,
    OpenCompleted,
    ErrorOccurred,
    EndEncountered,
    HasBytesAvailable,
    HasSpaceAvailable,
};

enum class DataType {
    Stream,
    Packet
};

// [TODO] 写函数需要异步传递数据，需要一个std::shared_ptr<utils::Buffer>
class ICommunicator: public utils::IAsync {
public:
    using DataEventHandler = std::function<void(std::shared_ptr<utils::Data> data)>;
    using CommunicatorEventHandler = std::function<void(ICommunicator *, CommunicatorEvent event)>;

    virtual ~ICommunicator() = default;
    virtual void read(DataEventHandler handler) = 0;
    virtual void write(std::shared_ptr<utils::Data> data) = 0;
    virtual void closeWrite() = 0;

    virtual const CommunicatorStateMachine& stateMachine() const = 0;
    virtual DataType communicatorDataType() const = 0;
};

class IRemoteCommunicator : public ICommunicator {
public:
    virtual ~IRemoteCommunicator() = default;

    virtual void connect(std::shared_ptr<Endpoint>) = 0;
    virtual const Endpoint* connectingEndpoint() const = 0;
};

class ILocalCommunicator : public ICommunicator {
public:
    virtual ~ILocalCommunicator() = default;

    virtual void open() = 0;
    virtual void continueFinished() = 0;
};

};

