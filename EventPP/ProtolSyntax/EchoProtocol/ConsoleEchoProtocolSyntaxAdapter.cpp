//
//  ConsoleEchoProtocolSyntaxAdapter.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//
#include <sstream>
#include <iostream>
#include <string.h>
#include "ConsoleEchoProtocolSyntaxAdapter.hpp"
#include "Stream.hpp"
#include "SocketAddress.hpp"
#include "TCPConnection.hpp"

using namespace ts;
void ConsoleEchoProtocolSyntaxAdapter::feed(ProtocolSyntax::EventType type, ts::InputStream *inputStream, ts::OutputStream *outputStream, void* ctx) {
    ssize_t len = inputStream->length();
    unsigned char *buf = new unsigned char[len];
    memset(buf, 0, len);
    ssize_t readLen = inputStream->read(buf, len);
    std::stringstream ss;
    auto addr = (TCPConnection *)ctx;
    ss<<"["<<addr->getSocketAddress().ipPortPairString().c_str()<<"]"<<" recv: "<<buf<<std::endl;
    outputStream->write(ss.str().c_str(), ss.str().length());
    delete [] buf;
}
ConsoleEchoProtocolSyntaxAdapter::~ConsoleEchoProtocolSyntaxAdapter() {
    
}
