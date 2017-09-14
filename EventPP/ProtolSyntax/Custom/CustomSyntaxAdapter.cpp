//
//  CustomSyntaxAdapter.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "CustomSyntaxAdapter.hpp"
#include <functional>

using namespace ts;
void CustomSyntaxAdapter::_init() {
    this->mOnWriteableHandler = [](void *ctx) {};
    this->mOnErrorHandler = [](void *ctx){};
    this->mWillEndWriteHandler = [](void *ctx){};
    this->mWillEndReadHandler = [](void *ctx){};
    this->mOnEOFEventHandler = [](void *ctx){};
    this->mOnSignalEventHandler = [](int fd, short what, void* arg){};
    this->mBufferHandler = [](ProtocolSyntax::EventType type, Buffer &inputBuffer, Buffer &outputBuffer, void *ctx){};
    this->mStreamHandler = [](ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx){};
}

CustomSyntaxAdapter::CustomSyntaxAdapter(std::function<void(ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx)> h) {
    this->_init();
    this->mStreamHandler = h;
}

CustomSyntaxAdapter::CustomSyntaxAdapter(std::function<void(ProtocolSyntax::EventType type, Buffer &inputBuffer, Buffer &outputBuffer, void *ctx)> h) {
    this->_init();
    this->mBufferHandler = h;
}
CustomSyntaxAdapter::~CustomSyntaxAdapter() {
    
}
CustomSyntaxAdapter::CustomSyntaxAdapter() {
    this->_init();
}

