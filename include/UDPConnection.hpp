//
//  UDPConnection.hpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef UDPConnection_hpp
#define UDPConnection_hpp

#include <memory>

#include "SocketKit.hpp"
#include "SocketAddress.hpp"
#include "Communicator.hpp"

namespace socketkit {
    class UDPConnection : public Communicator {
    private:
        
        SocketFd mSocket;
        
    public:
        UDPConnection();
        UDPConnection(SocketAddress address);
        UDPConnection(std::string hostname, int port);
        UDPConnection(std::string hostname, std::string portString);
        ~UDPConnection();
        
        SocketAddress mSocketAddress;
        
        void useSocketFd(SocketFd fd);
        
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
}

#endif /* UDPConnection_hpp */
