cmake_minimum_required(VERSION 3.0)

option(BUILD_FOR_IOS "Build For iOS" OFF)
if (NOT DEFINED BUILD_FOR_IOS)
	set(BUILD_FOR_IOS OFF)
endif()

option(DEBUG "Debug Build" ON)
if (NOT DEFINED DEBUG)
	set(DEBUG ON)
endif()

if(BUILD_FOR_IOS STREQUAL "ON")
    set(CMAKE_XCODE_ATTRIBUTE_SDKROOT "iphoneos")
    set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
	set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "10.0")
endif()

if(DEBUG STREQUAL "ON")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
endif()

project(socket_kit)

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

# Set CMake Env
set(CMAKE_INSTALL_PREFIX "libSocketKit")

# Source File Directoy
add_subdirectory(src)

