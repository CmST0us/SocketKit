//
//  Buffer.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#include "Buffer.hpp"

Buffer::Buffer() {
    
}

Buffer::~Buffer() {
    
}

ssize_t Buffer::read(void *dst, int len) {
    return  -1;
};

ssize_t Buffer::write(const void *src, int len) {
    return -1;
};

ssize_t Buffer::fetch(void *dst, int len) {
    return -1;
};

ssize_t Buffer::length() {
    return -1;
}

void Buffer::flush() {
    
}
