//
//  TCPConnectionFactory.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/9.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef TCPConnectionFactory_hpp
#define TCPConnectionFactory_hpp

#include "TCPConnection.hpp"
#include "CustomSyntaxAdapter.hpp"
namespace ts {
    
class TCPConnectionFactory {
public:
    //短链接一次性写入buffer中的数据，写完即关闭
    static TCPConnection * shortLinkTCPConnection(Buffer &buffer);
    
    //长链接需要根据具体需要修改事件回调
    static TCPConnection * longLinkTCPConnection();
    
};
    
}

#endif /* TCPConnectionFactory_hpp */
