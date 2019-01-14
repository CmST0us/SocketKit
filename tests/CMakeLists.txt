cmake_minimum_required(VERSION 3.0)

project(socket_kit_tests)

# Set Compiler Flags
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DDEBUG")
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -DDEBUG")
endif()

add_executable(
    test_runloop
    test_runloop.cpp
)

add_executable(
    test_tcpsocket
    test_tcpsocket.cpp
    ../src/TCP/TCPSocket.cpp
    ../src/TCP/TCPConnector.cpp
    ../src/TCP/TCPAcceptor.cpp
    ../src/SocketException.cpp
    ../src/SocketKit.cpp
)

target_include_directories(
    test_tcpsocket PUBLIC
    ../include/libSocketKit
)