//
//  TCPServer.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <mutex>

#include "../Communicator.hpp"

#include "SocketAddress.hpp"
#include "TCPConnection.hpp"
#include "ProtocolSyntax.hpp"

typedef int SocketFd;

namespace ts{
    class TCPServer : public CommunicatorService {
    private:
        SocketFd mSocket;
        
    public:
        TCPServer();
        TCPServer(SocketAddress address);
        TCPServer(short port);
        
        virtual ~TCPServer();
        
        SocketAddress mSocketAddress;
        
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
    
}
#endif /* TCPServer_hpp */
