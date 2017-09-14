//
//  Stream.hpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//

#ifndef Stream_hpp
#define Stream_hpp

#include <memory>

#include "Buffer.hpp"

namespace ts {
    
class Stream {
protected:
    std::unique_ptr<Buffer> mBuffer;
public:
    Stream() {
        
    };
    Stream(std::unique_ptr<Buffer> &buffer) {
        this->mBuffer = std::move(buffer);
    }
    ssize_t length() {
        return this->mBuffer->length();
    }
    //由于某些buffer的引用计数问题，需要显式调用释放
    void free() {
        this->mBuffer->free();
    }
    ~Stream() {
        
    }
};
    
class InputStream: public Stream {
private:
    bool mCanRead = true;
public:
    void setCanRead(bool f) {
        this->mCanRead = f;
    }
    bool canRead() {
        return this->mCanRead;
    }
    
    InputStream() {};
    InputStream(std::unique_ptr<Buffer> &buffer) {
        this->mBuffer = std::move(buffer);
    }
    
    ssize_t read(void *dst, int len) {
        if (this->mCanRead) {
            return this->mBuffer->read(dst, len);
        }
        return -1;
    }
    
    ssize_t fetch(void *dst, int len) {
        if (this->mCanRead) {
           return this->mBuffer->fetch(dst, len);
        }
        return -1;
    }
    void flush() {
        this->mBuffer->flush();
    }
};
    
class OutputStream: public Stream {
private:
    bool mCanWrite = true;
public:
    void setCanWrite(bool f) {
        this->mCanWrite = f;
    }
    
    bool canWrite() {
        return this->mCanWrite;
    }
    
    OutputStream() {};
    OutputStream(std::unique_ptr<Buffer> &buffer) {
        this->mBuffer = std::move(buffer);
    }
    ssize_t write(const void *src, int len) {
        if (this->mCanWrite) {
           return this->mBuffer->write(src, len);
        }
        return -1;
    }
};
    
}
#endif /* Stream_hpp */
