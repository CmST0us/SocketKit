
#include <thread>
#include <chrono>
#include <string>
#include <memory>

#include "TCPSocket.hpp"
#include "TCPAcceptor.hpp"

#define TEST_TCP_LOCAL_PORT 12003
#define TEST_TCP_REMOTE_PORT 12004

using namespace socketkit;
int main(int argc, char* argv[]) {
    initialize();

    // Connect Target
    std::shared_ptr<TCPSocket> tcp = std::make_shared<TCPSocket>();

    auto rh = [](ICommunicator *communicator, std::shared_ptr<utils::Data> data) {
        communicator->write(data);
    };

    auto eh = [rh](ICommunicator *communicator, CommunicatorEvent event) {
        if (event == CommunicatorEvent::OpenCompleted) {
            char str[] = "TCPSocket Test Echo Server";
            std::shared_ptr<utils::Data> strData = std::make_shared<utils::Data>(strlen(str));
            strData->copy((const void *)str, 5);

            communicator->write(strData);
        } else if (event == CommunicatorEvent::HasBytesAvailable) {
            communicator->read(rh);
        } else if (event == CommunicatorEvent::EndEncountered) {
            communicator->getRunloop()->post([communicator]() {
                communicator->closeWrite();
            });
        } else {
            communicator->getRunloop()->stop();
        }
    };

    tcp->mEventHandler = eh;
    tcp->getRunloop()->run();
    
    std::shared_ptr<Endpoint> ep = std::make_shared<Endpoint>("127.0.0.1", TEST_TCP_REMOTE_PORT);
    tcp->connect(ep);

    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}