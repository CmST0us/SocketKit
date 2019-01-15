//
// Created by CmST0us on 2019/1/12.
//

#pragma once

#include <queue>
#include <thread>
#include <condition_variable>

#include "NoCopyable.hpp"

namespace socketkit {
namespace utils {

class Runloop final : NoCopyable {
public:
    using RunloopHandler = std::function<void(Runloop *)>;
    using RunloopTaskHandler = std::function<void(void)>;

    Runloop() = default;
    Runloop(RunloopHandler handler) : _runloopHandler{handler} {

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
#if DEBUG
        std::cout<<"Runloop thread "<<_runloopThread.get_id()<<"will exit"<<std::endl;
#endif
    }

    void run() {
        if (isRunning()) return;

        _canceled = false;

        _runloopThread = std::thread([this](){
            _running = true;
            _runloopHandler(this);
            _running = false;
        });
        _runloopThread.detach();
    }

    void post(RunloopTaskHandler task) {
        _taskQueuePushLock.lock();
        _taskQueue.push(task);
        _notEmpty.notify_all();
        _taskQueuePushLock.unlock();
    }

    void dispatch(bool wait = false) {
        if (_taskQueue.empty()) {
            // 等待数据
            if (wait) {
                std::unique_lock<std::mutex> locker(_taskQueuePushLock);
                _notEmpty.wait(locker, [this](){
                    return !_taskQueue.empty();
                });
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
    }};

    std::thread _runloopThread;
    std::queue<RunloopTaskHandler> _taskQueue;

    std::mutex _taskQueuePushLock;
    std::condition_variable _notEmpty;
    volatile AtomBool _canceled{false};
    AtomBool _running{false};

};
};
};
