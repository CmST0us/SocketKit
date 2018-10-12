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

namespace ts {
    class CommunicatorService;
    
    enum CommunicatorServiceEvent {
        CommunicatorServiceEventSuccess = 0,
        CommunicatorServiceEventBroken = 1,
        CommunicatorServiceEventError = 2
    };
    
    class CommunicatorDelegate {
    public:
        virtual void serviceDidReadData(uchar *data, int len, CommunicatorService &service);
        
        virtual void serviceDidUpdateStatus(CommunicatorService &service);
        
        virtual void serviceDidReceiveEvent(CommunicatorServiceEvent event) {
            
        };
    };
    
    class Communicator {
    public:
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
    };
    
    class CommunicatorService : public Communicator {
    public:
        virtual bool writeData(const uchar *data, int len);
        virtual bool start();
        virtual bool pause();
        virtual bool resume();
        virtual bool close();
        
        std::weak_ptr<CommunicatorDelegate> mDelegate;
    };
    
};
#endif /* Communicator_hpp */
