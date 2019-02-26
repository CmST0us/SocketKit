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
	link_directories(/Users/cmst0us/opt/usr/lib)
endif()

add_executable(
    test_tcpsocket
    test_tcpsocket.cpp
    ../src/TCP/TCPSocket.cpp
    ../src/TCP/TCPConnector.cpp
    ../src/TCP/TCPAcceptor.cpp
    ../src/SocketException.cpp
    ../src/SocketKit.cpp
)

add_executable(
    test_udpsocket
    test_udpsocket.cpp
    ../src/UDP/UDPSocket.cpp
    ../src/SocketKit.cpp
    ../src/SocketException.cpp
)

if (WIN32)

else()
add_executable(
	test_usbmuxd
	test_usbmuxd.cpp
	../src/usbmuxd/UsbmuxdConnector.cpp
	../src/usbmuxd/UsbmuxdProtocol.cpp
	../src/usbmuxd/UsbmuxdSocket.cpp
	../src/usbmuxd/UsbmuxdDeviceListener.cpp
	../src/SocketKit.cpp
	../src/SocketException.cpp
)
endif()

target_include_directories(
    test_tcpsocket PUBLIC
    ../include/libSocketKit
)

target_include_directories(
    test_udpsocket PUBLIC
    ../include/libSocketKit
)

target_include_directories(
	test_usbmuxd PUBLIC
	../include/libSocketKit
)

if (WIN32)
target_link_libraries(
	test_udpsocket PRIVATE
	ws2_32
)

target_link_libraries(
	test_tcpsocket PRIVATE
	ws2_32
)
else ()
target_link_libraries(
	test_udpsocket PRIVATE
	pthread
)
target_link_libraries(
	test_tcpsocket PRIVATE
	pthread
)
target_link_libraries(
	test_usbmuxd PRIVATE
	pthread
)

target_link_libraries(
		test_usbmuxd PRIVATE
		libplist.a
)
target_include_directories(
		test_usbmuxd PRIVATE
		/Users/cmst0us/opt/usr/include
)

endif()


