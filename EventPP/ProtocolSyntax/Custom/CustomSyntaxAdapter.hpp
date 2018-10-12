//
//  CustomSyntaxAdapter.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef CustomSyntaxAdapter_hpp
#define CustomSyntaxAdapter_hpp

#include "../ProtocolSyntax.hpp"
#include <functional>
#include "TCPConnection.hpp"
namespace ts {
class InputStream;
class OutputStream;
class Buffer;
class CustomSyntaxAdapter: public ProtocolSyntax {
private:
    void _init();
public:
    std::function<void(ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx)> mStreamHandler;
    std::function<void(ProtocolSyntax::EventType type, Buffer &inputBuffer, Buffer &outputBuffer, void *ctx)> mBufferHandler;
    std::function<void(void *ctx)> mOnEOFEventHandler;
    std::function<void(void *ctx)> mWillEndReadHandler;
    std::function<void(void *ctx)> mWillEndWriteHandler;
    std::function<void(void *ctx)> mOnErrorHandler;
    std::function<void(void *ctx)> mOnWriteableHandler;
    std::function<void(int fd, short what, void *arg)> mOnSignalEventHandler;
    CustomSyntaxAdapter(decltype(mBufferHandler) h);
    CustomSyntaxAdapter(decltype(mStreamHandler) h);
    CustomSyntaxAdapter();
    ~CustomSyntaxAdapter();
    
    
    virtual void onEOFEvent(void *ctx) override{this->mOnEOFEventHandler(ctx);};
    virtual void willEndRead(void *ctx) override{this->mWillEndReadHandler(ctx);};
    virtual void willEndWrite(void *ctx) override{this->mWillEndWriteHandler(ctx);};
    virtual void onError(void *ctx) override{this->mOnErrorHandler(ctx);};
    virtual void onSignalEvent(int fd, short what, void *arg) override{this->mOnSignalEventHandler(fd, what, arg);};
    virtual void onWriteable(void *ctx) override{this->mOnWriteableHandler(ctx);};
    virtual void feed(ProtocolSyntax::EventType type, Buffer &input, Buffer &output, void *ctx) override {
        this->mBufferHandler(type, input, output, ctx);
    }
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override {
        this->mStreamHandler(type, inputStream, outputStream, ctx);
    }
};
}

#endif /* CustomSyntaxAdapter_hpp */
