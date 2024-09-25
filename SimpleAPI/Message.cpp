#include "Message.h"


Packet convert_to_packet(const std::string& str) noexcept {
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}

Packet convert_to_packet(const char *str) noexcept {
    return convert_to_packet(std::string(str)); //не перемещать реализацию в header
}

std::string convert_from_packet(const Packet &packet) noexcept {
    std::string str;
    str.resize(packet.size());
    std::copy(packet.begin(), packet.end(), str.begin());
    return str;
}

std::string to_string(const Packet &packet) noexcept {
    return std::string((char*)packet.data(), packet.size()); //не перемещать реализацию в header
}

std::string to_string(PacketType type) noexcept {
    switch(type){
    case eControlType:  return "[CTRL]";
    case eDataType:     return "[DATA]";
    default:            return "[UNKNOWN]";
    }
}

void PacketMessage::clear() noexcept {
    ipPort          = IpPort{"", 0};
    packet          = {};
    sn.reset();
    isError         = false;
    isBuiltComplete = false;
}

std::string PacketMessage::to_string() noexcept {
    std::string out;

    out = this->ipPort.to_string() + " ";
    out += "[(" + std::to_string(this->packet.size()) + ") " + ::to_string(this->packet) + "]";

    return out;
}

JsonMessage::JsonMessage(const PacketMessage &pm) noexcept {
    this->ipPort = pm.ipPort;
    this->json.parseJSON(convert_from_packet(pm.packet));
}

void JsonMessage::clear() noexcept {
    ipPort = IpPort{"", 0};
    json.clear();
}

std::string JsonMessage::to_string(int arg) noexcept {
    std::string out;

    out = this->ipPort.to_string() + " ";
    out += "[(" + std::to_string(this->json.size()) + ")" + this->json.to_string(arg) + "]";

    return out;
}

ApiVersion getLastApiVersion() noexcept {
    return ApiVersion::eVersion_1; //NOTE: при новых версиях заменять вручную
}

