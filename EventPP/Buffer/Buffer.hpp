//
//  Buffer.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Buffer_hpp
#define Buffer_hpp

#include <sys/types.h>
#include <strstream>
namespace ts {
    
    class Buffer {
    private:
        std::strstreambuf mBuffer;
    public:
        Buffer();
        
        virtual ssize_t read(void *dst, int len);
        virtual ssize_t write(const void *src, int len);
        virtual ssize_t fetch(void *dst, int len);
        virtual ssize_t length();
        virtual void flush();
        virtual ~Buffer();
    };
    
}
#endif /* Buffer_hpp */
