
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
    std::unique_ptr<utils::Runloop> runloop = std::unique_ptr<utils::Runloop>(new utils::Runloop());
    utils::Runloop *runloopPtr = runloop.get();
    runloop->run();

    // Connect Target
    std::shared_ptr<TCPSocket> tcp = std::make_shared<TCPSocket>();

    auto rh = [](ICommunicator *communicator, std::shared_ptr<utils::Data> data) {
        communicator->write(data);
    };

    auto eh = [rh, runloopPtr, &tcp](ICommunicator *communicator, CommunicatorEvent event) {
        if (event == CommunicatorEvent::OpenCompleted) {
            char str[] = "TCPSocket Test Echo Server";
            std::shared_ptr<utils::Data> strData = std::make_shared<utils::Data>(strlen(str));
            strData->copy((const void *)str, strData->getDataSize());

            communicator->write(strData);
        } else if (event == CommunicatorEvent::HasBytesAvailable) {
            communicator->read(rh);
        } else if (event == CommunicatorEvent::EndEncountered) {
            communicator->getRunloop()->post([communicator]() {
                communicator->closeWrite();
            });
        } else {
            // 参考错误释放处理方法
            communicator->getRunloop()->stop();
            tcp.reset();
            //runloopPtr->post([&tcp]() {
            //    tcp.reset();
            //});
        }
    };

    tcp->mEventHandler = eh;
     tcp->getRunloop()->run();
    
    std::shared_ptr<Endpoint> ep = std::make_shared<Endpoint>("127.0.0.1", TEST_TCP_REMOTE_PORT);
    tcp->connect(ep);

    // Local Server
    std::shared_ptr<TCPSocket> clientSocket;
    std::shared_ptr<TCPAcceptor> acceptor = std::make_shared<TCPAcceptor>();

    auto ap = [&clientSocket, eh](TCPAcceptor *acceptor, std::shared_ptr<TCPSocket> client) {
        client->getRunloop()->run();

        client->open();
        client->continueFinished();
        client->mEventHandler = eh;

        char str[] = "TCPSocket Test Echo Server";
        std::shared_ptr<utils::Data> strData = std::make_shared<utils::Data>(strlen(str));
        strData->copy((const void *)str, strData->getDataSize());

        client->write(strData);

        clientSocket = client;
    };

    auto aep = [ap](TCPAcceptor *acceptor, TCPAcceptorEvent event) {
        if (event == TCPAcceptorEvent::CanAccept) {
            acceptor->accept(ap);
        }
    };
    acceptor->mEventHandler = aep;
    acceptor->getRunloop()->run();

    std::shared_ptr<Endpoint> listenEp = std::make_shared<Endpoint>("127.0.0.1", TEST_TCP_LOCAL_PORT);
    acceptor->bind(listenEp);
    acceptor->listen(5);
    acceptor->mEventHandler = aep;

    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}