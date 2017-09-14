//
//  EchoProtocolSyntaxAdapter.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "EchoProtocolSyntaxAdapter.hpp"
#include "Stream.hpp"

using namespace ts;
EchoProtocolSyntaxAdapter::EchoProtocolSyntaxAdapter() {
    
}

EchoProtocolSyntaxAdapter::~EchoProtocolSyntaxAdapter() {
    
}

void EchoProtocolSyntaxAdapter::feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) {
    ssize_t len = inputStream->length();
    unsigned char *buf = new unsigned char[len];
    memset(buf, 0, len);
    ssize_t readLen = inputStream->read(buf, len);
    outputStream->write(buf, readLen);
}

void EchoProtocolSyntaxAdapter::feed(ProtocolSyntax::EventType type, Buffer &input, Buffer &output, void *ctx) {
    
}

void EchoProtocolSyntaxAdapter::onError(void *ctx) {
    
}
