//
// Created by CmST0us on 2019/1/15.
//

#pragma once

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
        return _size;
    }

    bool copy(const void *src, int size) {
        if (size > _size) {
            return false;
        }
        ::memcpy(_ptr, src, size);
        return true;
    }

    void clear() {
        memset(_ptr, 0, _size);
    }

    void expansion(int newSize) {
        if (newSize > _size) {
            _ptr = ::realloc(_ptr, newSize);
            _size = newSize;
        }
        clear();
    }


    ~Data() {
        if (_ptr != nullptr) {
            free(_ptr);
        }
        _ptr = nullptr;
        _size = 0;
    }
private:
    void *_ptr{nullptr};
    int _size{0};
};

};
};