//
//  EvBufferAdapter.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/5.
//  Copyright © 2017年 CmST0us. All rights reserved.
//


#include <event2/buffer.h>

#include "EvBufferAdapter.hpp"
using namespace ts;
EvBufferAdapter::EvBufferAdapter() {
    
}

EvBufferAdapter::EvBufferAdapter(struct evbuffer* buf) {
    this->mBuffer = buf;
}

EvBufferAdapter::~EvBufferAdapter() {
    //显示调用free()来释放
}

void EvBufferAdapter::init() {
    this->mBuffer = evbuffer_new();
}
ssize_t EvBufferAdapter::read(void *dst, int len) {
    if (this->mBuffer != nullptr) {
        return evbuffer_remove(this->mBuffer, dst, len);
    } else {
        return -1;
    }
}

ssize_t EvBufferAdapter::write(const void *src, int len) {
    if (this->mBuffer != nullptr) {
        return evbuffer_add(this->mBuffer, src, len);
    } else {
        return -1;
    }
}

ssize_t EvBufferAdapter::fetch(void *dst, int len) {
    if (this->mBuffer != nullptr) {
        return evbuffer_copyout(this->mBuffer, dst, len);
    } else {
        return -1;
    }
}

ssize_t EvBufferAdapter::length() {
    if (this->mBuffer != nullptr) {
        return evbuffer_get_length(this->mBuffer);
    }
    return -1;
}

void EvBufferAdapter::flush() {
    if (this->mBuffer != nullptr) {
        evbuffer_drain(this->mBuffer, evbuffer_get_length(this->mBuffer));
    }
}

void EvBufferAdapter::free() {
    if (this->mBuffer != nullptr) {
        evbuffer_free(this->mBuffer);
        this->mBuffer = nullptr;
    }
}
