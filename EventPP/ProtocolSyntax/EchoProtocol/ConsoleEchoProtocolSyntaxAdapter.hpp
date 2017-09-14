//
//  ConsoleEchoProtocolSyntaxAdapter.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef ConsoleEchoProtocolSyntaxAdapter_hpp
#define ConsoleEchoProtocolSyntaxAdapter_hpp

#include "EchoProtocolSyntaxAdapter.hpp"
namespace ts {
class ConsoleEchoProtocolSyntaxAdapter: public EchoProtocolSyntaxAdapter {
public:
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
    ~ConsoleEchoProtocolSyntaxAdapter();
};
}

#endif /* ConsoleEchoProtocolSyntaxAdapter_hpp */
