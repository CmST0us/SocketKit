//
// Created by CmST0us on 2019/1/14.
//

#include <memory>
#include "UDPSocket.hpp"

#define TEST_UDP_LOCAL_PORT 12001
#define TEST_UDP_REMOTE_PORT 12002
using namespace socketkit;

int main(int argc, char *argv[]) {
    socketkit::initialize();

    std::unique_ptr<UDPSocket> udp = std::unique_ptr<UDPSocket>(new UDPSocket(TEST_UDP_LOCAL_PORT));
    UDPSocket *pUdp = udp.get();
    auto rh = [pUdp](std::shared_ptr<utils::Data> data) {
        pUdp->write(data);
    };

    auto eh = [rh](ICommunicator *communicator, CommunicatorEvent event) {
        if (event == CommunicatorEvent::HasBytesAvailable) {
            communicator->read(rh);
        }
    };

    udp->mEventHandler = eh;
    udp->getRunloop()->run();

    std::shared_ptr<Endpoint> ep = std::make_shared<Endpoint>("localhost", 12002);
    udp->connect(ep);
    
    std::shared_ptr<utils::Data> helloData = std::make_shared<utils::Data>(5);
    char str[] = "hello";
    helloData->copy((const void *)str, 5);

    udp->write(helloData);

    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}