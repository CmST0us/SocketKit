//
//  EvBufferAdapter.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/5.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef EvBufferAdapter_hpp
#define EvBufferAdapter_hpp

#include <event2/buffer.h>

#include "Buffer.hpp"

namespace ts {
class EvBufferAdapter: public Buffer {
private:
    struct evbuffer *mBuffer = NULL;
    
public:
    
    EvBufferAdapter();
    EvBufferAdapter(struct evbuffer* buf);
    ~EvBufferAdapter();
    
    virtual void init();
    virtual ssize_t read(void *dst, int len);
    virtual ssize_t write(const void *src, int len);
    virtual ssize_t fetch(void *dst, int len);
    virtual ssize_t length();
    virtual void flush();
    virtual void free();
};
    
}

#endif /* EvBufferAdapter_hpp */
