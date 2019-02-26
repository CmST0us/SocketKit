#include "UsbmuxdSocket.hpp"

#include <memory>
using namespace socketkit;
int main(int argc, char *argv[]) {
    std::shared_ptr<socketkit::UsbmuxdSocket> socket = std::make_shared<socketkit::UsbmuxdSocket>();
    socket->getConnector()->listenDevice();
    socket->getConnector()->mEventHandler = [](UsbmuxdConnector *connector, UsbmuxdConnectorEvent event, SocketFd socket) {
        if (event == UsbmuxdConnectorEvent::Connected) {
            printf("Connect\n");
        }
    };
    std::this_thread::sleep_for(std::chrono::seconds(100000));
    return 0;
}