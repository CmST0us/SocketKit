
#pragma once

namespace socketkit {
namespace utils {

class IAsync {
    virtual ~IAsync() = default;
    virtual Runloop *getRunloop() = 0;
};

};
};
