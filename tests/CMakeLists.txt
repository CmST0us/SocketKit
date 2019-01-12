cmake_minimum_required(VERSION 3.0)

project(socket_kit_tests)

# Set Compiler Flags
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

add_executable(
    test_runloop
    test_runloop.cpp
)

add_executable(
    test_tcpconnection
    test_tcpconnection.cpp
    ../src/TCP/TCPConnection.cpp
    ../src/SocketException.cpp
    ../src/SocketKit.cpp
)

target_include_directories(
    test_tcpconnection PUBLIC
    ../include/libSocketKit
)