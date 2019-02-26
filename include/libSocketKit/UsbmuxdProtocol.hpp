//
// Created by CmST0us on 2019/2/25.
//

#pragma once

#include <functional>
#include <map>
#include <vector>
#include "NoCopyable.hpp"

#define kUsbmuxdHeaderLength (16)

namespace socketkit {

enum class UsbmuxdMessageType {
    Result = 1,
    Connect = 2,
    Listen = 3,
    DeviceAdd = 4,
    DeviceRemove = 5,
    // ???
    // ???
    Plist = 8,
};

enum class UsbmuxdResult {
    OK = 0,
    BadCommand = 1,
    BadDev = 2,
    ConnectRefused = 3,
    // ???
    // ???
    BadVersion = 6,
};

enum class UsbmuxdProtocolVersion {
    Binary = 0,
    Plist = 1,
};

struct UsbmuxdHeader {
    uint32_t length; // 包括头长度
    uint32_t version; //UsbmuxdProtocolVersion
    uint32_t type; // UsbmuxdMessageType
    uint32_t tag;
} __attribute__((__packed__));

struct UsbmuxdResultMessage {
    UsbmuxdHeader header;
    uint32_t result;
} __attribute__((__packed__));

struct UsbmuxdConnectRequest {
    UsbmuxdHeader header;
    uint32_t deviceId;
    uint16_t port;
    uint16_t reserved;
} __attribute__((__packed__));

struct UsbmuxdListenRequest {
    UsbmuxdHeader header;
} __attribute__((__packed__));

struct UsbmuxdDeviceRecord {
    uint32_t deviceId;
    uint16_t productId;
    char serialNumber[256];
    uint16_t padding;
    uint32_t location;
} __attribute__((__packed__));

struct UsbmuxdDeviceRecordMessage {
    UsbmuxdHeader header;
    UsbmuxdDeviceRecord record;
} __attribute__((__packed__));

class UsbmuxdProtocol final : public utils::NoCopyable {
public:
    UsbmuxdProtocol();
    virtual ~UsbmuxdProtocol();

    using UsbmuxdResultHandler = std::function<void(UsbmuxdHeader req, UsbmuxdResultMessage res)>;
    // 确认是否有Header
    using UsbmuxdDeviceRecordHandler = std::function<void(UsbmuxdDeviceRecord record)>;
    UsbmuxdListenRequest makeListenRequestWithHandler(UsbmuxdResultHandler handler);
    UsbmuxdConnectRequest makeConnectRequestWithHandler(uint32_t deviceId, uint16_t port, UsbmuxdResultHandler handler);

    void parse();
    void recvResultMessage(UsbmuxdResultMessage msg);
    void recvDeviceRecordMessage(UsbmuxdDeviceRecord msg);
private:
    std::map<uint32_t, UsbmuxdHeader> _tagHeaderMap;
    std::map<uint32_t, UsbmuxdResultHandler> _tagHandlerMap;


    uint32_t _tag{1};

};

}