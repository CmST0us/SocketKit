/* Don't actually copy this code: it is a poor way to implement an
 HTTP client.  Have a look at evhttp instead.
 */

#include <stdio.h>
#include <iostream>

#include "../EventPP/SocketAddress.hpp"


int main(int argc, char **argv)
{
    
    ts::SocketAddress address("localhost");
    std::cout<<address.getIpString();
    return 0;
    
}
