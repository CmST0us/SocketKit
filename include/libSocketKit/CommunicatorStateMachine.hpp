//
// Created by CmST0us on 2019/1/12.
//

#pragma once

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

namespace socketkit {

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
                _state == CommunicatorState::Established ||
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
        _state = CommunicatorState::Established;
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

}