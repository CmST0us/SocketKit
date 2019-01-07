cmake_minimum_required(VERSION 3.0)

project(socket_kit_tests)

# Set Compiler Flags
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
endif()

if (CMAKE_COMPILER_IS_GNUC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
endif()

if(NOT DEFINED SOCKETKIT_DIR)
    message(
        FATAL_ERROR
        "Please use -DSOCKETKIT_DIR to specific libSocketKit install path"
    )
endif()

message("SOCKETKIT_DIR is'${SOCKETKIT_DIR}'")

include_directories(${SOCKETKIT_DIR}/include)
link_directories(${SOCKETKIT_DIR}/lib)
link_libraries(SocketKit)
link_libraries(ws2_32)

add_executable(
    test_udp
    test_udp.cpp
)
