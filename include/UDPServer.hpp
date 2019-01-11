//
//  UDPServer.hpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef UDPServer_hpp
#define UDPServer_hpp

#include <memory>
#include <map>

#include "SocketKit.hpp"
#include "Communicator.hpp"
#include "SocketAddress.hpp"


#define UDP_BUFFER_SIZE 1500

namespace socketkit {
    class UDPServer : public Communicator {
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

