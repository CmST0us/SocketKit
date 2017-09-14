//
//  EchoProtocolSyntaxAdapter.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/6.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef EchoProtocolSyntaxAdapter_hpp
#define EchoProtocolSyntaxAdapter_hpp

#include "ProtocolSyntax.hpp"

namespace ts {
class EchoProtocolSyntaxAdapter: public ProtocolSyntax{
private:
    
public:
    virtual void feed(ProtocolSyntax::EventType type, InputStream *inputStream, OutputStream *outputStream, void *ctx) override;
    virtual void feed(ProtocolSyntax::EventType type, Buffer &input, Buffer &output, void *ctx) override;
    virtual void onError(void *ctx) override;
    
    EchoProtocolSyntaxAdapter();
    virtual ~EchoProtocolSyntaxAdapter();
};
}

#endif /* EchoProtocolSyntaxAdapter_hpp */
