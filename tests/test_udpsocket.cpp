//
// Created by CmST0us on 2019/1/14.
//

#include "UDPSocket.hpp"

using namespace socketkit;

int main(int argc, char *argv[]) {
    UDPSocket *socket = new UDPSocket(12009);
    auto eh = [](ICommunicator *c, CommunicatorEvent e) {
        if (e == CommunicatorEvent::HasBytesAvailable) {
            c->read([c](uchar *b, int s) {
                int size = s;
                c->write(b, size);
            });
        }
    };
    socket->mEventHandler = eh;
    socket->getRunloop()->run();

    auto ep = std::make_shared<Endpoint>("127.0.0.1", 12002);
    socket->connect(ep);
    std::string hello = "hello";
    int s = hello.length();
    socket->write((uchar *)hello.c_str(), s);
    std::this_thread::sleep_for(std::chrono::seconds(100000));
    delete socket;
}