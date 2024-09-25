#ifndef IP_PORT_H
#define IP_PORT_H

#include <string>

struct IpPort {
    std::string ip;
    uint16_t    port;

    bool operator==(const IpPort& other) const noexcept;
    bool operator!=(const IpPort& other) const noexcept;
    bool operator<(const IpPort& other) const noexcept;
    bool operator>(const IpPort& other) const noexcept;

    std::string to_string(std::string info = "") const noexcept;
    bool from_string(std::string ip_port_string) noexcept;
};

#endif // IP_PORT_H
