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
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../Communicator.hpp"

#include "SocketAddress.hpp"

namespace ts{
    class TCPServer : public CommunicatorService {
    private:
        SocketFd mSocket;
        std::thread mAccpetThread;
        bool willStop = false;
        
        bool createSocket();
        bool bindSocket();
        bool listenSocket();
        bool closeSocket();
        void accpetHandle();
        
    public:
        TCPServer();
        TCPServer(SocketAddress address);
        TCPServer(short port);
        
        ~TCPServer();
        
        SocketAddress mSocketAddress;
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
    
}
#endif /* TCPServer_hpp */
