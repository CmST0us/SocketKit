//
// Created by CmST0us on 2019/1/15.
//

#pragma once
#include <string.h>
#include "NoCopyable.hpp"

namespace socketkit {
namespace utils {

class Data final : public NoCopyable {
public:
    Data(void *ptr, int size) : _ptr{ptr}, _size{size} {
        clear();
    }

    Data(int size) : _size{size} {
        _ptr = (void *)new unsigned char[size];
        memset(_ptr, 0, size);
    }

    void *getDataAddress() {
        return _ptr;
    }

    const int getDataSize() const {
        return _dataSize;
    }

    const int getSize() const {
        return _size;
    }

    bool copy(const void *src, int size) {
        if (size > _size) {
            return false;
        }
        memcpy(_ptr, src, size);
        _dataSize = size;
        return true;
    }

    void clear() {
        memset(_ptr, 0, _size);
        _dataSize = 0;
    }

    void expansion(int newSize) {
        if (newSize > _size) {
            _ptr = ::realloc(_ptr, newSize);
            _size = newSize;
        }
        clear();
    }

    void setDataSize(int size) {
        _dataSize = size;
    }

    ~Data() {
        if (_ptr != nullptr) {
            free(_ptr);
        }
        _ptr = nullptr;
        _size = 0;
        _dataSize = 0;
    }
private:
    void *_ptr{nullptr};
    int _size{0};
    int _dataSize{0};
};

};
};
