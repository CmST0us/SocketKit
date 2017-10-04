//
//  SocketException.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef SocketException_hpp
#define SocketException_hpp
#include <string>
class SocketException {
private:
    int exceptionCode;
    std::string exceptionDescription;
    
public:
    SocketException(int code, std::string message) {
        this->exceptionCode = code;
        this->exceptionDescription = message;
    };
    std::string getExceptionDescription() {
        return this->exceptionDescription;
    }
    int getExceptionCode() {
        return this->exceptionCode;
    }
    
    static SocketException hostCannotResolve;
    static SocketException connectListenerCreateError;
    static SocketException connectError;
    static SocketException socks5UnsupportStateError;
    static SocketException socksFdInitError;
    static SocketException socksBindError;
    static SocketException eventInitError;
    static SocketException eventAddError;
};



#endif /* SocketException_hpp */
