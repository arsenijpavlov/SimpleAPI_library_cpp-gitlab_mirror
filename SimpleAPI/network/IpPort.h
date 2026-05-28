#ifndef IP_PORT_H
#define IP_PORT_H

#include <string>


namespace simpleapi {

struct IpPort {
    std::string ip;
    uint16_t    port;

    bool operator==(const IpPort& other)            const noexcept;
    bool operator!=(const IpPort& other)            const noexcept;
    bool operator<(const IpPort& other)             const noexcept;
    bool operator>(const IpPort& other)             const noexcept;

    std::string to_string(std::string info = "")    const noexcept;
    bool from_string(std::string ip_port_string)    noexcept;
};

} // namespace simpleapi

#endif // IP_PORT_H
