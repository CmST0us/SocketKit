//
// Created by CmST0us on 2019/1/12.
//

#pragma once

#include <queue>
#include <thread>
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
        if (isRunning()) {
            stop();
            if (_runloopThread.joinable()) {
                _runloopThread.join();
            }
        }
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
    }

    void post(RunloopTaskHandler task) {
        _taskQueuePushLock.lock();
        _taskQueue.push(task);
        _taskQueuePushLock.unlock();
    }

    void dispatch() {
        while (!_taskQueue.empty()) {
            RunloopTaskHandler task = _taskQueue.front();
            task();
            _taskQueue.pop();
        }
    }


private:
    using AtomBool = std::atomic_bool;

    RunloopHandler _runloopHandler{[](Runloop *r){
        while(!r->isCanceled()) {
            r->dispatch();
        }
    }};

    std::thread _runloopThread;
    std::queue<RunloopTaskHandler> _taskQueue;

    std::mutex _taskQueuePushLock;
    volatile AtomBool _canceled{false};
    AtomBool _running{false};

};
};
};
