cmake_minimum_required(VERSION 3.0)

option(BUILD_FOR_IOS "Build For iOS" OFF)

if(BUILD_FOR_IOS STREQUAL "ON")
    set(CMAKE_XCODE_ATTRIBUTE_SDKROOT "iphoneos")
    set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
    set(IPHONEOS_DEPLOYMENT_TARGET "10.0")
endif()

project(socket_kit)

# Set Compiler Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# Set CMake Env
set(CMAKE_INSTALL_PREFIX "libSocketKit")

# Source File Directoy
add_subdirectory(src)
