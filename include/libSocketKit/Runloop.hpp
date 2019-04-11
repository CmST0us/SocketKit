//
// Created by CmST0us on 2019/1/12.
//

#pragma once

#include <queue>
#include <thread>
#include <future>
#include <condition_variable>

#include "SocketKit.hpp"
#include "NoCopyable.hpp"

namespace socketkit {
namespace utils {

class Runloop final : NoCopyable {
public:
    using RunloopHandler = std::function<void(Runloop *)>;
    using RunloopTaskHandler = std::function<void(void)>;

    Runloop() = default;
    Runloop(RunloopHandler handler) : _runloopHandler{handler} {
        _innerSocket = ::socket(PF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in innerAddr;
        innerAddr.sin_family = AF_INET;
        innerAddr.sin_port = 0;
        innerAddr.sin_addr.s_addr = INADDR_LOOPBACK;
        int ret = ::bind(_innerSocket, (const sockaddr *)&innerAddr, sizeof(innerAddr));
        if (ret != 0) {
            printf("error");
        }
        _innerPort = ntohs(innerAddr.sin_port);
    }

    ~Runloop() {
        stop();
    }

    bool isRunning() const {
        return _running;
    }

    bool isCanceled() const {
        return _canceled;
    }

    void stop() {
        _canceled = true;
    }

    void run() {
        if (isRunning()) return;

        _canceled = false;

        _runloopThreadResult = std::async(std::launch::async, [this]() {
            _running = true;
            _runloopHandler(this);
            _running = false;
            return true;
        });

    }

    void post(RunloopTaskHandler task) {
        _taskQueuePushLock.lock();
        _taskQueue.push(task);
        _notEmpty.notify_all();
        notifyEvent();
        _taskQueuePushLock.unlock();
    }

    void dispatch(bool wait = false) {
        if (_taskQueue.empty()) {
            // 等待数据
            if (wait) {
                std::unique_lock<std::mutex> locker(_taskQueuePushLock);
                _notEmpty.wait(locker, [this](){
                    // !!! 如果一个Runloop启动，并开始等待操作，此时会阻塞线程，
                    // 在一个操作到来之前不会结束，故这里需要添加_cancel判断是否应该继续等待
                    return !_taskQueue.empty() || _canceled;
                });
            }
            if (_canceled) {
                return;;
            }
        } else {
            while (!_taskQueue.empty()) {
                RunloopTaskHandler task = _taskQueue.front();
                task();
                _taskQueue.pop();
            }
        }
    }


private:
    using AtomBool = std::atomic_bool;

    RunloopHandler _runloopHandler{[](Runloop *r){
        while(!r->isCanceled()) {
            r->dispatch(true);
        }
        return true;
    }};

    std::future<bool> _runloopThreadResult;
    std::queue<RunloopTaskHandler> _taskQueue;

    std::mutex _taskQueuePushLock;
    std::condition_variable _notEmpty;
    volatile AtomBool _canceled{false};
    AtomBool _running{false};

    SocketFd _innerSocket{ -1 };
    short _innerPort{ 0 };
    void notifyEvent() {
        struct sockaddr_in target;
        target.sin_addr.s_addr = INADDR_LOOPBACK;
        target.sin_family = AF_INET;
        target.sin_port = htons(_innerPort);
        ::sendto(_innerSocket, "", 0, 0, (const sockaddr*)&target, sizeof(target));
    }
};
};
};
