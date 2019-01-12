//
// Created by CmST0us on 2019/1/12.
//

#pragma once

namespace socketkit {
namespace utils {

class NoCopyable {
protected:
    NoCopyable() {};
    ~NoCopyable() {};
private:
    NoCopyable(const NoCopyable&);
    const NoCopyable& operator=(const NoCopyable&);
};

}
}