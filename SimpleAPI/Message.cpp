#include "Message.h"


Packet convert_to_packet(const std::string& str) {
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}

Packet convert_to_packet(const char *str) {
    return convert_to_packet(std::string(str));
}

std::string convert_from_packet(const Packet &packet) {
    std::string str;
    str.resize(packet.size());
    std::copy(packet.begin(), packet.end(), str.begin());
    return str;
}

std::string to_string(const Packet& packet) {
    return std::string((char*)packet.data(), packet.size());
}

std::string to_string(PacketType type) {
    switch(type){
    case eControlType:  return "[CONTROL]";
    case eDataType:     return "[DATA]";
    default: return "UNKNOWN";
    }
}

PacketMessage::PacketMessage() : sn(0) {
    clear();
}

void PacketMessage::clear() {
    ip="";
    port=0;
    packet={};
    sn.reset();
}

std::string PacketMessage::to_string()
{
    std::string out;

    out = "[" + this->ip + ":" + std::to_string(this->port) + "] ";
    out += "[(" + std::to_string(this->packet.size()) + ") " + ::to_string(this->packet) + "]";

    return out;
}

void JsonMessage::clear() {
    ip="";
    port=0;
    json={};
}

std::string JsonMessage::to_string(int arg)
{
    std::string out;

    out = "[" + this->ip + ":" + std::to_string(this->port) + "] ";
    out += "[(" + std::to_string(this->json.size()) + ")" + this->json.to_string(arg) + "]";

    return out;
}
