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
    
    struct CommunicatorServiceStatus {
        bool isInit:1;  // 是否初始化成功，只有初始化成功时才可以进行连接。
        bool isConnected:1;   // 当连接上的时候会置为YES，连接断开即为NO。
        bool isConnecting:1;    // 开始连接时会设置为YES，连接成功或失败则置为NO。
        bool isClosing:1;   // 开始关闭时会设置为YES，断开连接之后或其他时则为NO。
        bool isPause:1; // 当保持连接并中止时，即为YES，使用中为NO。当连接中断时不予判断。
        bool canRead:1; // 可以读取数据。
        bool canWrite:1;    // 可以写入。
        uint8_t unDefined:1;
    };
    
    class CommunicatorServiceDelegate {
    public:
        virtual void serviceDidReadData(uchar *data, int len, std::shared_ptr<ts::CommunicatorService> service) {
            
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
