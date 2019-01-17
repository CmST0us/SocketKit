//
//  SocketException.cpp
//  SocketKit
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "SocketException.hpp"

SocketException SocketException::hostCannotResolve(-10000, "Host cannot resolve");
SocketException SocketException::connectError(-10001, "Cannot connet target");

SocketException SocketException::socketFdInitError(-20001, "Cannot init socket fd");
SocketException SocketException::socketBindError(-20002, "Cannot bind socket");
SocketException SocketException::socketListenError(-20003, "Cannot listen socket");
