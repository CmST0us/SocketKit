//
//  EventCallbackInterface.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef EventCallbackInterface_hpp
#define EventCallbackInterface_hpp

#include <event2/event.h>
#include <event2/listener.h>

namespace ts{
    
class EventCallbackInterface {
public:
    virtual void onAcceptEvent(struct evconnlistener *listener,
                               evutil_socket_t fd,
                               struct sockaddr *address,
                               int socklen,
                               void *ctx) = 0;
    
    virtual void onAcceptErrorEvent(struct evconnlistener *listener, void *ctx) = 0;
    
    virtual void onConnectionEvent() = 0;
    virtual void onReadableEvent() = 0;
    virtual void onWritenEvent() = 0;
    virtual void onCloseEvent() = 0;
    virtual void onErrorEvent() = 0;
};
    
}
#endif /* EventCallbackInterface_hpp */
