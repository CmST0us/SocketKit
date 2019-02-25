cmake_minimum_required(VERSION 3.0)

option(BUILD_USBMUXD_COMM "build usbmuxd comm" OFF)
set(BUILD_USBMUXD_COMM ON)
aux_source_directory(. socket_kit_lib_source)
aux_source_directory(./TCP socket_kit_lib_source_tcp)
aux_source_directory(./UDP socket_kit_lib_source_udp)
if (BUILD_USBMUXD_COMM STREQUAL "ON")
	aux_source_directory(./usbmuxd socket_kit_lib_source_usbmuxd)
endif()

# Target
add_library(
    SocketKit 
    STATIC
    ${socket_kit_lib_source}
    ${socket_kit_lib_source_tcp}
    ${socket_kit_lib_source_udp}
    ${socket_kit_lib_source_usbmuxd}
)

target_include_directories(SocketKit PUBLIC
    ../include/libSocketKit
)

target_include_directories(SocketKit PRIVATE
    .
    ./TCP
    ./UDP
	./usbmuxd
)

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
    DIRECTORY
		../include/libSocketKit
    DESTINATION
		include
)
