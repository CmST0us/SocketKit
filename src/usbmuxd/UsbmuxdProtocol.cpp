//
// Created by CmST0us on 2019/2/25.
//

#include <plist/plist++.h>
#include "UsbmuxdProtocol.hpp"

#define kUsbmuxdProtocolMessageTypeKey "MessageType"
#define kUsbmuxdProtocolMessageTypeListen "Listen"
#define kUsbmuxdProtocolMessageTypeConnect "Connect"
#define kUsbmuxdProtocolMessageTypeResult "Result"
#define kUsbmuxdProtocolMessageTypeAttached "Attached"
#define kUsbmuxdProtocolMessageTypeDetached "Detached"

using namespace socketkit;

UsbmuxdProtocol::UsbmuxdProtocol() {

}

UsbmuxdProtocol::~UsbmuxdProtocol() {

}

std::shared_ptr<utils::Data> UsbmuxdProtocol::makeListenRequestWithHandler(UsbmuxdResultHandler handler) {
    UsbmuxdListenRequest request;
    uint32_t tag = _tag++;
    request.header.version = (uint32_t)UsbmuxdProtocolVersion::Plist;
    request.header.type = (uint32_t)UsbmuxdMessageType::Plist;
    request.header.tag = tag;
    _tagHeaderMap[tag] = request.header;
    _tagHandlerMap[tag] = handler;

    plist_t dict = plist_new_dict();
    plist_t listenString = plist_new_string(kUsbmuxdProtocolMessageTypeListen);
    plist_dict_set_item(dict, kUsbmuxdProtocolMessageTypeKey, listenString);

    char *xml = nullptr;
    uint32_t xmlLen = 0;
    plist_to_xml(dict, &xml, &xmlLen);
    request.header.length = kUsbmuxdHeaderLength + xmlLen;

    std::shared_ptr<utils::Data> data = std::make_shared<utils::Data>(sizeof(request) + xmlLen);
    memcpy(data->getDataAddress(), &request, sizeof(request));
    memcpy(((uint8_t *)data->getDataAddress()) + sizeof(request), xml, xmlLen);

    free(xml);
    plist_free(dict);
    return data;
}

std::shared_ptr<utils::Data> UsbmuxdProtocol::makeConnectRequestWithHandler(uint32_t deviceId, uint16_t port, UsbmuxdResultHandler handler) {
    UsbmuxdHeader header;
    uint32_t tag = _tag++;
    header.tag = tag;
    header.type = (uint32_t)UsbmuxdMessageType::Plist;
    header.version = (uint32_t)UsbmuxdProtocolVersion::Plist;
    _tagHeaderMap[tag] = header;
    _tagHandlerMap[tag] = handler;

    port = ((port<<8) & 0xFF00) | (port>>8); // limit

    plist_t dict = plist_new_dict();
    plist_t messageType = plist_new_string(kUsbmuxdProtocolMessageTypeConnect);
    plist_t deviceID = plist_new_uint(deviceId);
    plist_t portNumber = plist_new_uint(port);
    plist_dict_set_item(dict, "DeviceID", deviceID);
    plist_dict_set_item(dict, kUsbmuxdProtocolMessageTypeKey, messageType);
    plist_dict_set_item(dict, "PortNumber", portNumber);

    char *xml = nullptr;
    uint32_t xmlLen = 0;
    plist_to_xml(dict, &xml, &xmlLen);
    header.length = kUsbmuxdHeaderLength + xmlLen;

    std::shared_ptr<utils::Data> data = std::make_shared<utils::Data>(kUsbmuxdHeaderLength + xmlLen);
    memcpy(data->getDataAddress(), &header, kUsbmuxdHeaderLength);
    memcpy(((uint8_t *)data->getDataAddress()) + kUsbmuxdHeaderLength, xml, xmlLen);

    free(xml);
    plist_free(dict);

    return data;
}


void UsbmuxdProtocol::parsePlistPayloadMessage(UsbmuxdHeader header, uint8_t *data, size_t len) {
    plist_t plistObj = nullptr;
    plist_from_xml((const char *)data, len, &plistObj);
    if (plistObj == nullptr) return;

    // dispatch Message Type
    plist_t messageType = plist_dict_get_item(plistObj, kUsbmuxdProtocolMessageTypeKey);
    static plist_t resultString = plist_new_string(kUsbmuxdProtocolMessageTypeResult);
    static plist_t attachString = plist_new_string(kUsbmuxdProtocolMessageTypeAttached);
    static plist_t detachedString = plist_new_string(kUsbmuxdProtocolMessageTypeDetached);

    if (plist_compare_node_value(messageType, resultString)) {
        parseResultMessage(header, plistObj);
    } else if (plist_compare_node_value(messageType, attachString)) {
        parseAttachMessage(header, plistObj);
    } else if (plist_compare_node_value(messageType, detachedString)) {
        parseDetachedMessage(header, plistObj);
    }
    
    plist_free(plistObj);
}

void UsbmuxdProtocol::parseResultMessage(UsbmuxdHeader header, void *plistObj) {
    plist_t resultNumber = plist_dict_get_item(plistObj, "Number");
    uint64_t num = 0;
    plist_get_uint_val(resultNumber, &num);

    UsbmuxdResultMessage resultMessage = {0};
    resultMessage.header = header;
    resultMessage.result = num;

    if (_tagHeaderMap.count(header.tag) > 0) {
        if (_tagHandlerMap.count(header.tag) > 0) {
            UsbmuxdResultHandler handler = _tagHandlerMap[header.tag];
            handler(_tagHeaderMap[header.tag], resultMessage);
            _tagHandlerMap.erase(header.tag);
        }
        _tagHeaderMap.erase(header.tag);
    }
}

void UsbmuxdProtocol::parseAttachMessage(UsbmuxdHeader header, void *plistObj) {
    UsbmuxdDeviceRecord record = {0};
    uint64_t num = 0;

    plist_t deviceID = plist_dict_get_item(plistObj, "DeviceID");
    plist_get_uint_val(deviceID, &num);
    record.deviceId =(uint32_t)num;

    plist_t property = plist_dict_get_item(plistObj, "Properties");
    plist_t productID = plist_dict_get_item(property, "ProductID");
    plist_t serialNumber = plist_dict_get_item(property, "SerialNumber");
    plist_t locationID = plist_dict_get_item(property, "LocationID");

    plist_get_uint_val(productID, &num);
    record.productId = num;

    plist_get_uint_val(locationID, &num);
    record.location = num;

    char *str = nullptr;
    plist_get_string_val(serialNumber, &str);
    memcpy(record.serialNumber, str, 256);
    free(str);

    if (mDeviceRecordHandler) {
        mDeviceRecordHandler(true, record);
    }
}

void UsbmuxdProtocol::parseDetachedMessage(UsbmuxdHeader header, void *plistObj) {
    UsbmuxdDeviceRecord record = {0};
    uint64_t num = 0;

    plist_t deviceID = plist_dict_get_item(plistObj, "DeviceID");
    plist_get_uint_val(deviceID, &num);

    record.deviceId = (uint32_t)num;
    if (mDeviceRecordHandler) {
        mDeviceRecordHandler(false, record);
    }
}

void UsbmuxdProtocol::parseDeviceRecord(UsbmuxdHeader header, void *plistObj) {

}
