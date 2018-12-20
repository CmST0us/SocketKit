cmake_minimum_required(VERSION 3.0)

project(socket_kit)

# Set Compiler Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# Set CMake Env
set(CMAKE_INSTALL_PREFIX "libSocketKit")

# Source File Directoy
add_subdirectory(src)

