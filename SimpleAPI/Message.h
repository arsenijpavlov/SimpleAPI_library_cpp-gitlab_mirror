#ifndef MESSAGE_H
#define MESSAGE_H

#include "EECounter.h"
#include "IpPort.h"
#include "Json.h"

#include <cstdint>
#include <string>
#include <vector>


using Packet = std::vector<uint8_t>;

Packet      convert_to_packet(const std::string& str);
Packet      convert_to_packet(const char* str);
std::string convert_from_packet(const Packet& packet);
std::string to_string(const Packet& packet);

enum PacketType {
    eControlType    = 0,
    eDataType       = 1
};
std::string to_string(PacketType type);

class PacketMessage {
public:
    IpPort      ipPort;
    Packet      packet;
    EECounter   sn;
    PacketType  type;

    bool        incorrectCRC;
    bool        isBuiltComplete;

    PacketMessage();

    void clear();
    std::string to_string();
};

class JsonMessage {
public:
    IpPort      ipPort;
    Json        json;

    JsonMessage(){};
    JsonMessage(const JsonMessage& jm);
    JsonMessage(const PacketMessage& pm);

    void clear();
    std::string to_string(int arg = -1);
};

#endif // MESSAGE_H
