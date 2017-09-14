//
//  TCPConnectionFactory.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/9.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "TCPConnectionFactory.hpp"
using namespace ts;
TCPConnection * TCPConnectionFactory::shortLinkTCPConnection(Buffer &buffer) {
    auto ret = new TCPConnection();
    auto customSyntaxAdapter = new CustomSyntaxAdapter();
    
    customSyntaxAdapter->mStreamHandler = [&](ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx){
        auto connection = (TCPConnection *)ctx;
        switch (type) {
            case ProtocolSyntax::Read: {
                int len = inputStream->length();
                auto buf = new unsigned char[len];
                memset(buf, 0, len);
                inputStream->read(buf, len);
                buffer.write(buf, len);
                delete [] buf;
            }
                break;
            case ProtocolSyntax::Connect: {
                int len = buffer.length();
                auto buf = new unsigned char[len];
                memset(buf, 0, len);
                buffer.read(buf, len);
                outputStream->write(buf, len);
                delete [] buf;
                
            }
                break;
            default:
                break;
        }
    };
    customSyntaxAdapter->mOnWriteableHandler = [&](void *ctx) {
        auto connection = (TCPConnection *)ctx;
        if (connection->getOutputStream()->length() == 0) {
            connection->shutdown(SHUT_WR);
        }
    };
    customSyntaxAdapter->mWillEndWriteHandler = [&](void *ctx) {
        
    };
    
    customSyntaxAdapter->mWillEndReadHandler = [&](void *ctx) {
        
    };
    
    customSyntaxAdapter->mOnErrorHandler = [&](void *ctx) {
        auto c = (TCPConnection *)ctx;
        c->close();
    };
    
    customSyntaxAdapter->mOnEOFEventHandler = [&](void *ctx) {
        auto c = (TCPConnection *)ctx;
        c->close();
    };
    
    
    auto sync = std::shared_ptr<ProtocolSyntax>(customSyntaxAdapter);
    ret->setProtocolSyntax(sync);
    return ret;
}

TCPConnection * TCPConnectionFactory::longLinkTCPConnection() {
    auto ret = new TCPConnection();
    auto customSyntaxAdapter = new CustomSyntaxAdapter();
    
    auto sync = std::shared_ptr<ProtocolSyntax>(customSyntaxAdapter);
    ret->setProtocolSyntax(sync);
    return ret;
}
