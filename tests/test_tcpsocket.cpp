
#include <thread>
#include <chrono>
#include <string>

#include "TCPSocket.hpp"


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
    MyClass c;
    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}