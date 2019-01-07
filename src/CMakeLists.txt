cmake_minimum_required(VERSION 3.0)

aux_source_directory(. socket_kit_lib_source)
aux_source_directory(./TCP socket_kit_lib_source_tcp)
aux_source_directory(./UDP socket_kit_lib_source_udp)

# Target
add_library(
    SocketKit 
    STATIC
    ${socket_kit_lib_source}
    ${socket_kit_lib_source_tcp}
    ${socket_kit_lib_source_udp}    
)

target_include_directories(SocketKit PUBLIC
    ../include
)

target_include_directories(SocketKit PRIVATE
    .
    ./TCP
    ./UDP
)

## Windows Link Flags
if (WIN32)
target_link_libraries(SocketKit PRIVATE
	ws2_32
)
endif()

# Install
## Install Target
install(
    TARGETS SocketKit
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

## Install Header
install(
    FILES
    ../include/Communicator.hpp
    ../include/SocketAddress.hpp
    ../include/SocketException.hpp
    ../include/TCPConnection.hpp
    ../include/TCPServer.hpp
    ../include/UDPConnection.hpp
    ../include/UDPServer.hpp
    DESTINATION
    include/libSocketKit
)
