//
// Created by CmST0us on 2019/2/25.
//

#include "UsbmuxdProtocol.hpp"

using namespace socketkit;

UsbmuxdListenRequest UsbmuxdProtocol::makeListenRequestWithHandler(UsbmuxdResultHandler handler) {
    UsbmuxdListenRequest request;
    uint32_t tag = _tag++;
    request.header.length = kUsbmuxdHeaderLength;
    request.header.version = (uint32_t)UsbmuxdProtocolVersion::Binary;
    request.header.type = (uint32_t)UsbmuxdMessageType::Listen;
    request.header.tag = tag;

    _tagHeaderMap[tag] = request.header;
    _tagHandlerMap[tag] = handler;
    return request;
}

UsbmuxdConnectRequest UsbmuxdProtocol::makeConnectRequestWithHandler(uint32_t deviceId, uint16_t port, UsbmuxdResultHandler handler) {
    UsbmuxdConnectRequest request;
    uint32_t tag = _tag++;
    request.header.length = sizeof(UsbmuxdConnectRequest);
    request.header.tag = tag;
    request.header.type = (uint32_t)UsbmuxdMessageType::Connect;
    request.header.version = (uint32_t)UsbmuxdProtocolVersion::Binary;

    request.deviceId = deviceId;
    request.port = port;

    _tagHeaderMap[tag] = request.header;
    _tagHandlerMap[tag] = handler;
    return request;
}