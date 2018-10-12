//
//  ProtocolSyntax.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef ProtocolSyntax_hpp
#define ProtocolSyntax_hpp


namespace ts {
    
class Buffer;
class InputStream;
class OutputStream;
    
class ProtocolSyntax {
public:
    enum EventType{
        Accept,
        Connect,
        Read
    };
    
    virtual void onEOFEvent(void *ctx) {};
    virtual void willEndRead(void *ctx) {};
    virtual void willEndWrite(void *ctx) {};
    virtual void onError(void *ctx) {};
    virtual void onWriteable(void *ctx) {};
    virtual void onSignalEvent(int fd, short what, void *arg) {};
    virtual void feed(EventType type, Buffer &input, Buffer &output, void *ctx) {};
    virtual void feed(EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {};
    
    virtual ~ProtocolSyntax() {};
};
    
}

#endif /* ProtocolSyntax_hpp */
