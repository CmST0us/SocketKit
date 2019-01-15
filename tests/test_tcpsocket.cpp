
#include <thread>
#include <chrono>
#include <string>
#include <memory>

#include "TCPSocket.hpp"
#include "TCPAcceptor.hpp"

class MyAcceptorClass final : socketkit::utils::NoCopyable {
private:
    std::shared_ptr<socketkit::TCPAcceptor> _acceptor;
    std::shared_ptr<socketkit::TCPSocket> _socket;
public:
    MyAcceptorClass() {
        _acceptor = std::make_shared<socketkit::TCPAcceptor>();
        auto rh = [this](uchar *b, int s) {
            if (s > 0) {
                int ss = s;
                _socket->write(b, ss);
            }
        };

        auto eh = [this, rh](socketkit::ICommunicator *c, socketkit::CommunicatorEvent e) {
            if (e == socketkit::CommunicatorEvent::HasBytesAvailable) {
                c->read(rh);
            }
        };

        auto ae = [this, eh](socketkit::TCPAcceptor *a, std::shared_ptr<socketkit::TCPSocket> s) {
            _socket = s;
            _socket->getRunloop()->run();
            _socket->mEventHandler = eh;
            _socket->open();
            _socket->continueFinished();

            std::string str = "hello";
            int l = str.length();
            _socket->write((uchar *)str.c_str(), l);
        };

        auto e = [ae] (socketkit::TCPAcceptor *a, socketkit::TCPAcceptorEvent event) {
            if (event == socketkit::TCPAcceptorEvent::CanAccept) {
                printf("accept");
                a->accept(ae);
            }
        };

        _acceptor->mEventHandler = e;
        _acceptor->getRunloop()->run();

        auto endpoint = std::make_shared<socketkit::Endpoint>("127.0.0.1", 12002);
        _acceptor->bind(endpoint);
        _acceptor->listen(5);
    }
};


class MyClass final : socketkit::utils::NoCopyable {
private:
    std::shared_ptr<socketkit::TCPSocket> _connection;
    uchar buffer[4096] = {0};
public:
    void eventCallback(socketkit::ICommunicator *communicator, socketkit::CommunicatorEvent event) {
        switch (event) {
            case socketkit::CommunicatorEvent::HasBytesAvailable : {
                printf("can read\n");
                int s = 4096;
                _connection->read([this](uchar *data, int size){
                    if (size > 0) {
                        _connection->write(data, size);
                    }
                });

                break;
            }
            case socketkit::CommunicatorEvent::EndEncountered : {
                printf("eof\n");
                break;
            }
            case socketkit::CommunicatorEvent::OpenCompleted : {
                printf("connect\n");
                break;
            }
            case socketkit::CommunicatorEvent::ErrorOccurred : {
                printf("error\n");
                break;
            }
            default:

                break;
        }
    }

    MyClass() {
        _connection= std::make_shared<socketkit::TCPSocket>();
        auto endpoint = std::make_shared<socketkit::Endpoint>("127.0.0.1", 12002);
        auto callback = std::bind(&MyClass::eventCallback, this, std::placeholders::_1, std::placeholders::_2);
        _connection->mEventHandler = callback;
        _connection->getRunloop()->run();
        _connection->connect(endpoint);
    }

};


int main(int argc, char* argv[]) {
    MyAcceptorClass *c = new MyAcceptorClass();

//    std::this_thread::sleep_for(std::chrono::seconds(1));

//    MyClass *t = new MyClass();

    std::this_thread::sleep_for(std::chrono::seconds(100000));
    delete c;
//    delete t;

    return 0;
}