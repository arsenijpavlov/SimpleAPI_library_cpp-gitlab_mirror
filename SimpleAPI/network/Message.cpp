#include "Message.h"


namespace simpleapi {

Packet ConvertToPacket(const std::string& str) noexcept {
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}

Packet ConvertToPacket(const char *str) noexcept {
    return ConvertToPacket(std::string(str)); //не перемещать реализацию в header
}

std::string ConvertFromPacket(const Packet &packet) noexcept {
    std::string str;
    str.resize(packet.size());
    std::copy(packet.begin(), packet.end(), str.begin());
    return str;
}

std::string ToString(const Packet &packet) noexcept {
    return std::string((char*)packet.data(), packet.size()); //не перемещать реализацию в header
}

std::string ToString(PacketType type) noexcept {
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

std::string PacketMessage::toString() noexcept {
    std::string out;

    out = m_ip_port.toString() + " ";
    out += "[(" + std::to_string(m_packet.size()) + ") " + ToString(m_packet) + "]";

    return out;
}

JsonMessage::JsonMessage(const PacketMessage &pm) noexcept {
    m_ip_port = pm.m_ip_port;
    m_json.parseJson(ConvertFromPacket(pm.m_packet));
    //ошибки конвертации не должны вызывать callback для RecvJson
    if(m_json.error())
        m_json.clear();
}

void JsonMessage::clear() noexcept {
    m_ip_port = IpPort{"", 0};
    m_json.clear();
}

std::string JsonMessage::toString() noexcept {
    std::string out;

    out = m_ip_port.toString() + " ";
    out += "[(" + std::to_string(m_json.size()) + ")" + m_json.toString() + "]";

    return out;
}

ApiVersion GetLastApiVersion() noexcept {
    return ApiVersion::eVersion_1; //NOTE: при новых версиях заменять вручную
}

} // namespace simpleapi

