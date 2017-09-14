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

namespace ts {
    
class Buffer {
public:
    virtual void init() {};
    virtual ssize_t read(void *dst, int len) {return  -1;};
    virtual ssize_t write(const void *src, int len) {return -1;};
    virtual ssize_t fetch(void *dst, int len) {return -1;};
    virtual ssize_t length() {return -1;};
    virtual void flush() {};
    virtual void free() {};
    virtual ~Buffer() {};
};
    
}
#endif /* Buffer_hpp */
