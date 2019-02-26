#include <memory>
#include <map>
#include <sstream>
#include <string>

#include "UsbmuxdSocket.hpp"

using namespace socketkit;
int main(int argc, char *argv[]) {
    std::shared_ptr<socketkit::UsbmuxdSocket> socket = std::make_shared<socketkit::UsbmuxdSocket>();
    UsbmuxdSocket *socketPtr = socket.get();
//    socket->getConnector()->listenDevice();
    socket->getConnector()->mEventHandler = [socketPtr](UsbmuxdConnector *connector, UsbmuxdConnectorEvent event, SocketFd socket) {
        if (event == UsbmuxdConnectorEvent::Connected) {
            printf("Connect\n");
        } else if (event == UsbmuxdConnectorEvent::DeviceAttached) {
            printf("Device Attach\n");
            std::map<uint32_t, UsbmuxdDeviceRecord>::iterator it;
            std::map<uint32_t, UsbmuxdDeviceRecord> attachDevices = connector->attachedDevices();
            it = attachDevices.begin();
            if (it != attachDevices.end()) {
                uint32_t deviceID = it->first;
                std::stringstream sstr;
                sstr << deviceID;

                std::shared_ptr<Endpoint> endpoint = std::make_shared<Endpoint>(sstr.str(), 17123, false);
                socketPtr->connect(endpoint);
            }
        } else if (event == UsbmuxdConnectorEvent::DeviceDetached) {
            printf("Device Detached\n");
        } else if (event == UsbmuxdConnectorEvent::Errored) {
            printf("Error \n");
        }
    };

    std::shared_ptr<Endpoint> endpoint = std::make_shared<Endpoint>("11", 17123, false);
    socket->connect(endpoint);

    std::this_thread::sleep_for(std::chrono::seconds(100000));
    return 0;
}