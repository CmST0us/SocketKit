//
//  Communicator.hpp
//  EventPP
//
//  Created by eric.wu on 2018/10/12.
//  Copyright © 2018年 CmST0us. All rights reserved.
//

#ifndef Communicator_hpp
#define Communicator_hpp

#include <stdio.h>
#include <memory>
#include "SocketAddress.hpp"

typedef unsigned char uchar;
typedef int SocketFd;

namespace ts {
    class Communicator;
    class CommunicatorService;
    
    enum CommunicatorServiceEvent {
        CommunicatorServiceEventSuccess = 0,
        CommunicatorServiceEventBroken = 1,
        CommunicatorServiceEventError = 2
    };
    
    // [TODO]状态机需要重构
    struct CommunicatorServiceStatus {
        bool isInit:1;
        bool isConnected:1;
        bool isConnecting:1;
        bool isClosing:1;
        bool isPause:1;
        bool canRead:1;
        bool canWrite:1;
        uint8_t unDefined:1;
    };
    
    class CommunicatorServiceDelegate {
    public:
        virtual void serviceDidReadData(ts::SocketAddress address, uchar *data, int len, std::shared_ptr<ts::CommunicatorService> service) {
            
        }
        virtual void serviceDidUpdateStatus(ts::CommunicatorService &service) {
            
        }
        virtual void serviceDidReceiveEvent(ts::CommunicatorServiceEvent event) {
            
        };
    };
    
    class Communicator {
    public:
        virtual bool writeData(const uchar *data, int len) = 0;
        virtual bool start() = 0;
        virtual bool pause() = 0;
        virtual bool resume() = 0;
        virtual bool close() = 0;
    };
    
    class CommunicatorService : public Communicator {
    public:
        virtual bool writeData(const uchar *data, int len) {
            return false;
        }
        virtual bool start() {
            return false;
        }
        virtual bool pause() {
            return false;
        }
        virtual bool resume() {
            return false;
        }
        virtual bool close() {
            return false;
        }
        CommunicatorServiceStatus mStatus = {0};
        std::weak_ptr<ts::CommunicatorServiceDelegate> mDelegate;
    };
    
};
#endif /* Communicator_hpp */
