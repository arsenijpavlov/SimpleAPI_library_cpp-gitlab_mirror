#ifndef IP_PORT_H
#define IP_PORT_H

#include <string>


struct IpPort {
    std::string ip;
    uint16_t    port;

    const bool operator==(const IpPort& other);
    const bool operator!=(const IpPort& other);
    const bool operator<(const IpPort& other) const;
    const bool operator>(const IpPort& other) const;

    std::string to_string(std::string info = "") const;
};

#endif // IP_PORT_H
