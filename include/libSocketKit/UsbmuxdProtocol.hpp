//
// Created by CmST0us on 2019/2/25.
//

#pragma once

#include <functional>
#include <map>
#include <vector>
#include <memory>

#include "NoCopyable.hpp"
#include "Data.hpp"

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
    uint32_t location;
} __attribute__((__packed__));

class UsbmuxdProtocol final : public utils::NoCopyable {
public:
    UsbmuxdProtocol();
    virtual ~UsbmuxdProtocol();

    using UsbmuxdResultHandler = std::function<void(UsbmuxdHeader req, UsbmuxdResultMessage res)>;
    using UsbmuxdDeviceRecordHandler = std::function<void(bool isAttach, UsbmuxdDeviceRecord record)>;

    UsbmuxdDeviceRecordHandler mDeviceRecordHandler{nullptr};

    std::shared_ptr<utils::Data> makeListenRequestWithHandler(UsbmuxdResultHandler handler);
    std::shared_ptr<utils::Data> makeConnectRequestWithHandler(uint32_t deviceId, uint16_t port, UsbmuxdResultHandler handler);

    void parsePlistPayloadMessage(UsbmuxdHeader header, uint8_t *data, size_t len);
private:
    std::map<uint32_t, UsbmuxdHeader> _tagHeaderMap;
    std::map<uint32_t, UsbmuxdResultHandler> _tagHandlerMap;

    uint32_t _tag{0};

    void parseResultMessage(UsbmuxdHeader header, void *plistObj);
    void parseAttachMessage(UsbmuxdHeader header, void *plistObj);
    void parseDetachedMessage(UsbmuxdHeader header, void *plistObj);
    void parseDeviceRecord(UsbmuxdHeader header, void *plistObj);
};

}
