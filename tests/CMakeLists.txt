cmake_minimum_required(VERSION 3.0)

project(socket_kit_tests)

# Set Compiler Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

if(NOT DEFINED SOCKETKIT_DIR)
    message(
        FATAL_ERROR
        "Please use -DSOCKETKIT_DIR to specific libSocketKit install path"
    )
endif()

message("SOCKETKIT_DIR is'${SOCKETKIT_DIR}'")

include_directories(${SOCKETKIT_DIR}/include)
link_libraries(${SOCKETKIT_DIR}/lib/libSocketKit.a)

add_executable(
    test_udp
    test_udp.cpp
)
