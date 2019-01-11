//
//  Communicator.hpp
//  SocketKit
//
//  Created by eric.wu on 2018/10/12.
//  Copyright © 2018年 CmST0us. All rights reserved.
//

#ifndef Communicator_hpp
#define Communicator_hpp

#include <stdio.h>
#include <memory>

#include "SocketAddress.hpp"
#include "SocketKit.hpp"

/*
 * Communicator State Machine
 * Reference from libnekit(https://github.com/zhuhaow/libnekit)
                  ┌────────────┐
                  │            │
       ┌──────────│    Init    │
       │          │            │
       │          └────────────┘
       │                 │   ConnectBegin (state machine)
       │                 │   Connect (remote)
       │                 │   Open (local)
       │                 ▼
       │          ┌────────────┐
       │          │            │
       ├──────────│Establishing│
       │          │            │
       │          └────────────┘
       │                 │
       │                 │  Connected (state machine)
       │                 │  Connected (remote)
       │                 │  Continue Finished (local)
       │                 │
       │                 ▼
       │          ┌────────────┐
       │          │            │  ReadBegin (state machine)
       ├──────────│Established │  WriteBegin (state machine)
       │          │            │  Read
       │          └────────────┘  Write
       │                 │
       │                 │  ReadEnd (state machine)
       │                 │  WriteCloseBegin (state machine)
       │                 │  Read EOF
       │                 │  CloseWrite
       │                 │
       │                 │
       │                 ▼
       │          ┌────────────┐
       │          │            │
       ├──────────│  Closing   │
       │          │            │
       │          └────────────┘
       │                 │
       │                 │  Closed (state machine)
       │                 │    Read
       │                 │   closed
       │                 │     &&
       │                 │   Write
       │                 │   closed
       │                 │
       ▼                 ▼
┌────────────┐    ┌────────────┐
│            │    │            │
│  Errored   │    │   Closed   │
│            │    │            │
└────────────┘    └────────────┘

 */

typedef unsigned char uchar;

namespace socketkit
{

    class Communicator;
    class CommunicatorService;

    // State
    enum class CommunicatorState {
        Init,
        Establishing,
        Established,
        Closing,
        Closed,
    };

    enum class CommunicatorType {
        Local,
        Remote
    };

    enum class DataType {
        Stream,
        Packet
    };


    class CommunicatorStateMachine final {
    public:
        CommunicatorStateMachine(CommunicatorType type) : _type(type) {

        };

        CommunicatorState state() const {
            return _state;
        };

        bool isReadable() const {
            return !_reading && // 不处于读取状态 并且 (本地正在建立 或者 状态建立完成 或者 关闭时,读未关闭)
                    ((_type == CommunicatorType::Local && _state == CommunicatorState::Establishing) ||
                    state_ == CommunicatorState::Established ||
                    (_state == CommunicatorState::Closing && !_readClosed));
        }

        bool isWritable() const {
            return !_writing &&
                    ((_type == CommunicatorType::Local && _state == CommunicatorState::Establishing) ||
                    _state == CommunicatorState::Established ||
                    (_state == CommunicatorState::Closing && !_writeClosed && !_writeClosing));
        }

        bool isWriteClosable() const {
            return isWritable();
        }

        bool isReading() const {
            return _reading;
        };

        bool isWriting() const {
            return _writing;
        }

        bool isReadClosed() const {
            return _readClosed;
        }

        bool isWriteClosed() const {
            return _writeClosed;
        }

        bool isWriteClosing() const {
            return _writeClosing;
        }

        void connectBegin() {
            SOCKETKIT_ASSET(_state == CommunicatorState::Init);
            if (_state != CommunicatorState::Init) return;

            _state = CommunicatorState::Establishing;
        }

        void connected() {
            SOCKETKIT_ASSET(_state == CommunicatorState::Establishing ||
                                    _state == CommunicatorState::Init);

            if (!((_state == CommunicatorState::Establishing) ||
                    (_state == CommunicatorState::Init))) return;

        }

        void readBegin() {
            SOCKETKIT_ASSET(isReadable());
            if (!isReadable()) return;;

            _reading = true;
        }

        void readEnd() {
            SOCKETKIT_ASSET(_reading);
            if (!_reading) return;

            _reading = false;
        }

        void readClosed() {
            SOCKETKIT_ASSET(_state == CommunicatorState::Established ||
                            _state == CommunicatorState::Closing);

            if (!(_state == CommunicatorState::Established ||
                  _state == CommunicatorState::Closing))
                return;;

            if (_state == CommunicatorState::Established) {
                _readClosed = true;
                _reading = false;
                _state = CommunicatorState::Closing;
            } else {
                _readClosed = true;
                _reading = false;
                if (_writeClosed) {
                    _state = CommunicatorState::Closed;
                }
            }
        }

        void writeBegin() {
            SOCKETKIT_ASSET(isWritable());
            if (!isWritable()) return;

            _writing = true;
        }

        void writeEnd () {
            SOCKETKIT_ASSET(_writing);
            if (!_writing) return;

            _writing = false;
        }

        void writeCloseBegin() {
            SOCKETKIT_ASSET(_state == CommunicatorState::Established ||
                            _state == CommunicatorState::Closing);
            if (!(_state == CommunicatorState::Established ||
                  _state == CommunicatorState::Closing))
                return;

            _state = CommunicatorState::Closing;
            _writeClosing = true;
            _writing = false;
        }

        void writeCloseEnd() {
            SOCKETKIT_ASSET(_writeClosing);
            if (!_writeClosing) return;

            _writeClosing = false;
            _writeClosed = true;

            if (_readClosed) {
                _state = CommunicatorState::Closed;
            }
        }

        void closed() {
            _reading = false;
            _writing = false;

            _writeClosed = true;
            _readClosed = true;

            _writeClosing = false;
            _state = CommunicatorState::Closed;

        }

        void errored() {
            this->closed();
        }

    private:
        CommunicatorType _type;
        CommunicatorState _state {CommunicatorState::Init};

        bool _reading{false}, _writing{false}, _readClosed{false},
                _writeClosed{false}, _writeClosing{false};
    };

    // Interface
    class Runloop final {

    };

    class IAsync {
        virtual ~IAsync() = default;
        virtual Runloop *getRunloop() = 0;
    };

    class ICommunicator: public IAsync {
    public:
        using DataEventHandler = std::function<void(uchar *buffer, int *size)>;

        ~ICommunicator() = default;
        virtual void read(DataEventHandler) = 0;
        virtual void write(uchar *buffer, int *size);
        virtual void closeWrite() = 0;

        virtual const CommunicatorState& stateMachine() const = 0;
        virtual DataType communicatorDataType() const = 0;
    };

    class IRemoteCommunicator : public ICommunicator {
    public:
        virtual ~IRemoteCommunicator() = default;

        virtual void connect(SocketAddress) = 0;
        virtual const SocketAddress& connectingEndpoint() const = 0;
    };

    class ILocalCommunicator : public ICommunicator {
    public:
        virtual ~ILocalCommunicator() = default;

        virtual void open() = 0;
        virtual void continueFinished() = 0;
    };
    
};
#endif /* Communicator_hpp */
