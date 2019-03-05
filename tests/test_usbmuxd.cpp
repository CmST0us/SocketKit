#include <memory>
#include <map>
#include <sstream>
#include <string>

#include "UsbmuxdSocket.hpp"
#include "UsbmuxdDeviceListener.hpp"

using namespace socketkit;
int main(int argc, char *argv[]) {
    std::shared_ptr<UsbmuxdDeviceListener> listener = std::make_shared<UsbmuxdDeviceListener>();
    std::shared_ptr<UsbmuxdSocket> socket = nullptr;
    auto readHandler = [](ICommunicator *comm, std::shared_ptr<utils::Data> data) {
        comm->write(data);
    };

    auto eventHandle = [readHandler, listener](ICommunicator *comm, CommunicatorEvent event) {
        if (event == CommunicatorEvent::OpenCompleted) {
            char *helloStr = (char *)malloc(10);
            std::shared_ptr<utils::Data> data = std::make_shared<utils::Data>(helloStr, 10);
            strcpy(helloStr, "Hello\n");
            comm->write(data);
        } else if (event == CommunicatorEvent::HasBytesAvailable) {
            comm->read(readHandler);
        } else {
            comm->getRunloop()->stop();
        }
    };
    listener->mDeviceListenerHandler = [eventHandle, &socket](bool isAttach, UsbmuxdDeviceRecord record) {
        if (isAttach) {
            std::stringstream sstr;
            sstr << record.deviceId;
            std::shared_ptr<Endpoint> ep = std::make_shared<Endpoint>(sstr.str(), 17123, false);
            socket = std::make_shared<socketkit::UsbmuxdSocket>();
            socket->mEventHandler = eventHandle;
            socket->getRunloop()->run();
            socket->connect(ep);
        } else {

        }
    };

    listener->getRunloop()->run();
    listener->startListenDevice();
    std::this_thread::sleep_for(std::chrono::seconds(100000));
    return 0;
}