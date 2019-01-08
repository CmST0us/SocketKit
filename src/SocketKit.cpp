
#include "SocketKit.hpp"

namespace socketkit
{
    bool initialize() {
#if _WIN32
        WSADATA wsaData;
        int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (ret != 0) {
            // error
            perror("WSAStartup Error");
            return false;
        }
#endif
        return true;
    };

    bool relesase() {
#if _WIN32
        int ret = WSACleanup();
        if (ret != 0) {
            perror("WSACleanup Error");
            return false;
        }
#endif
        return true;
    };
}
