/* Don't actually copy this code: it is a poor way to implement an
 HTTP client.  Have a look at evhttp instead.
 */

#include <stdio.h>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "../EventPP/SocketAddress.hpp"
#include "../EventPP/TCP/TCPServer.hpp"

class Delegate: public ts::CommunicatorServiceDelegate {
    virtual void serviceDidReadData(uchar *data, int len, std::shared_ptr<ts::CommunicatorService> service) {
        ::printf("[IN]Connect: %s\n", (const char *)data);
    };
};

int main(int argc, char **argv)
{
    auto delegate = std::make_shared<Delegate>();
    std::weak_ptr<Delegate> wp(delegate);
    
    ts::TCPServer server(12000);
    server.mDelegate = wp;
    server.start();
    return 0;
    
}
