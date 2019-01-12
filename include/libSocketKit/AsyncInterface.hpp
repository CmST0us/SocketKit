
#pragma once

#include "Runloop.hpp"

namespace socketkit {
namespace utils {

class IAsync {
public:
    virtual ~IAsync() = default;
    virtual Runloop *getRunloop() = 0;
};

};
};
