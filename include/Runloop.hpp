//
// Created by CmST0us on 2019/1/12.
//

#pragma once

#include "Runloop.hpp"

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
            _runloopThread.join();
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
        if (isRunning()) {
            _taskQueueLock.lock();
            _taskQueue.push_back(task);
            _taskQueueLock.unlock();
        }
    }

    void dispatch() {
        _taskQueueLock.lock();
        for (RunloopTaskHandler handler : _taskQueue) {
            handler();
        }
        _taskQueue.empty();
        _taskQueueLock.unlock();
    }


private:
    using AtomBool = std::atomic_bool;

    RunloopHandler _runloopHandler{[](Runloop *r){
        while(!r->isCanceled()) {
            r->dispatch();
        }
    }};

    std::thread _runloopThread;
    std::vector<RunloopTaskHandler> _taskQueue;

    std::mutex _taskQueueLock;
    AtomBool _canceled{false};
    AtomBool _running{false};

};
};
};
