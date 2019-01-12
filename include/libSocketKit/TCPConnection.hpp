//
//  TCPConnection.hpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef TCPConnection_hpp
#define TCPConnection_hpp

#include <memory>

#include "SocketAddress.hpp"
#include "Communicator.hpp"

namespace socketkit {
    class TCPConnection : public Communicator {
    private:
        
        SocketFd mSocket;
        
    public:
        TCPConnection();
        TCPConnection(SocketAddress address);
        TCPConnection(std::string hostname, int port);
        TCPConnection(std::string hostname, std::string portString);
        ~TCPConnection();
        
        SocketAddress mSocketAddress;
        
        void useSocketFd(SocketFd fd);
        
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
}
#endif /* TCPConnection_hpp */
