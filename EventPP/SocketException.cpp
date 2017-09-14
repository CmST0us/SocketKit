//
//  SocketException.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "SocketException.hpp"

SocketException SocketException::hostCannotResolve(10000, "Host cannot resolve");

SocketException
SocketException::connectListenerCreateError(10001, "Cannot create listener socket, may the port has been used");

SocketException SocketException::connectError(10002, "Cannot connet target");

SocketException SocketException::socks5UnsupportStateError(9001, "Socks5 client change to unsupport state");
