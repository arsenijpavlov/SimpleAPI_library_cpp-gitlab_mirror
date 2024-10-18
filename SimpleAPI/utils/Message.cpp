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
    m_ip_port   = IpPort{"", 0};
    m_packet    = {};
    m_sn.reset();
    m_is_error  = false;
    m_is_built_complete = false;
}

std::string PacketMessage::to_string() noexcept {
    std::string out;

    out = m_ip_port.to_string() + " ";
    out += "[(" + std::to_string(m_packet.size()) + ") " + ::to_string(m_packet) + "]";

    return out;
}

JsonMessage::JsonMessage(const PacketMessage &pm) noexcept {
    m_ip_port = pm.m_ip_port;
    m_json.parseJSON(convert_from_packet(pm.m_packet));
}

void JsonMessage::clear() noexcept {
    m_ip_port = IpPort{"", 0};
    m_json.clear();
}

std::string JsonMessage::to_string(int arg) noexcept {
    std::string out;

    out = m_ip_port.to_string() + " ";
    out += "[(" + std::to_string(m_json.size()) + ")" + m_json.to_string(arg) + "]";

    return out;
}

ApiVersion getLastApiVersion() noexcept {
    return ApiVersion::eVersion_1; //NOTE: при новых версиях заменять вручную
}

