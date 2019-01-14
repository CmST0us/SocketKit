

#include <iostream>
#include <functional>
#include <thread>
#include <vector>

/*
 *
 * class NoCopyable的基本思想是把构造函数和析构函数设置protected权限，
 * 这样子类可以调用，但是外面的类不能调用，那么当子类需要定义构造函数的时
 * 候不至于通不过编译。但是最关键的是NoCopyable把复制构造函数和复制赋值
 * 函数做成了private，这就意味着除非子类定义自己的copy构造和赋值函数，否
 * 则在子类没有定义的情况下，外面的调用者是不能够通过赋值和copy构造等手段
 * 来产生一个新的子类对象的。
 */
class NoCopyable {
protected:
    NoCopyable() {};
    ~NoCopyable() {};
private:
    NoCopyable(const NoCopyable&);
    const NoCopyable& operator=(const NoCopyable&);
};



class Runloop final : NoCopyable {
public:
    using RunloopHandler = std::function<void(Runloop *)>;
    using RunloopTaskHandler = std::function<void(void)>;

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

    RunloopHandler _runloopHandler{[](Runloop *){}};
    std::thread _runloopThread;
    std::vector<RunloopTaskHandler> _taskQueue;

    std::mutex _taskQueueLock;
    AtomBool _canceled{false};
    AtomBool _running{false};

};


void runloop_main(Runloop* runloop) {
    using namespace std;
    while (!runloop->isCanceled()) {
        cout<<"Hello World"<<endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        runloop->dispatch();
    }
}

int main(int argc, char *argv[]) {
    {
        using namespace std;
        std::shared_ptr<Runloop> rp = std::make_shared<Runloop>(runloop_main);

        rp->run();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        rp->post([](){
            cout<<"Post Task"<<endl;
        });
        rp->stop();
    }

    return 0;
}