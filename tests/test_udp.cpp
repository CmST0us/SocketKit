/* Don't actually copy this code: it is a poor way to implement an
 HTTP client.  Have a look at evhttp instead.
 */

#include <stdio.h>
#include <iostream>
#include <memory>
#include <unistd.h>

#include <libSocketKit/SocketAddress.hpp>
#include <libSocketKit/TCPServer.hpp>
#include <libSocketKit/UDPServer.hpp>
#include <libSocketKit/UDPConnection.hpp>
#include <libSocketKit/SocketAddress.hpp>

class Delegate: public ts::CommunicatorServiceDelegate {
    virtual void serviceDidReadData(ts::SocketAddress address, uchar *data, int len, std::shared_ptr<ts::CommunicatorService> service) {
        ::printf("[IN]Connect: %s\n", (const char *)data);
    };
};

int main(int argc, char **argv)
{
    auto delegate = std::make_shared<Delegate>();
    std::weak_ptr<Delegate> wp(delegate);
    
//    ts::TCPServer server(12000);
//    server.mDelegate = wp;
//    server.start();
    
    ts::UDPServer server(14560);
    server.mDelegate = wp;
    server.mClientAddress = ts::SocketAddress("127.0.0.1", 12001);
    server.start();
    
//    ts::UDPConnection connect("127.0.0.1", 12001);
//    connect.start();
    uchar d[] = "1232";
//    connect.writeData(d, 4);

    server.writeData(d, 4);
    return 0;
}
