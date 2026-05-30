#pragma once

#include <string>


namespace simpleapi {

class IpPort {
public:
    std::string ip;
    uint16_t    port;

    bool operator==(const IpPort& other)            const noexcept;
    bool operator!=(const IpPort& other)            const noexcept;
    bool operator<(const IpPort& other)             const noexcept;
    bool operator>(const IpPort& other)             const noexcept;

    // для вывода в лог с пояснением
    std::string toString(std::string info = "")    const noexcept;

    // выставить параметры на основе строки формата "X.X.X.X:port"
    // вернёт true, если получилось выполнить преобразование
    bool setFromString(std::string ip_port_string)    noexcept;
};

} // namespace simpleapi
