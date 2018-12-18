//
//  UDPServer.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef UDPServer_hpp
#define UDPServer_hpp

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <memory>
#include <evutil.h>
#include <map>

#include "../Communicator.hpp"
#include "SocketAddress.hpp"


#define UDP_BUFFER_SIZE 1500

namespace ts {
    class UDPServer : public CommunicatorService {
    private:
        SocketFd mSocket;
        std::thread mRecvThread;
        bool willStop = false;
        
        bool createSocket();
        bool bindSocket();
        bool closeSocket();
        void recvHandle();
        
    public:
        UDPServer();
        UDPServer(SocketAddress address);
        UDPServer(short port);
        ~UDPServer();
    
        SocketAddress mSocketAddress;
        SocketAddress mClientAddress;
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
}
#endif

