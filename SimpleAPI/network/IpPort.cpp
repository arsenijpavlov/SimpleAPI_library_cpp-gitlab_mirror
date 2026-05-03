#include "IpPort.h"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>


namespace simpleapi {

bool IpPort::operator==(const IpPort &other) const noexcept {
    if(ip == other.ip && port == other.port)    return true;
    else                                        return false;
}

bool IpPort::operator!=(const IpPort &other) const noexcept {
    if(ip != other.ip || port != other.port)    return true;
    else                                        return false;
}

bool IpPort::operator<(const IpPort &other) const noexcept {
    if(ip < other.ip)
        return true;
    else if(ip == other.ip)
        return port < other.port;
    else
        return false;
}

bool IpPort::operator>(const IpPort &other) const noexcept {
    if(port == other.port) {
        if(ip > other.ip)   return true;
        else                return false;
    }

    if(port > other.port)   return true;
    else                    return false;
}

std::string IpPort::to_string(std::string info) const noexcept {
    return std::string("[")
           + (info.empty() ? "" : info + "_")
           + ip + ":" + std::to_string(port) + "]";
}

bool IpPort::from_string(std::string ip_port_string) noexcept {
    std::string ip_string = ip_port_string.substr(0, ip_port_string.find(':'));
//    std::cout << "ip_string: \"" << ip_string << "\"" << std::endl;
    std::string port_string = ip_port_string.substr(ip_port_string.find(':') + 1,
                                                    ip_port_string.size());
//    std::cout << "port_string: \"" << port_string << "\"" << std::endl;
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ip_string.c_str(), &sock.sin_addr.s_addr))
        return false;

    //проверка, что есть число в строке, иначе atoi плохо сработает
    //с strtoul() непонятно как работать, он тоже всегда будет возвращать 0 в случае ошибки
    //так хотя бы надёжно
    //FIXME: переделать на пример из Config, вынести общий код в Utils.h
    switch(port_string[0]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':   break;
    default:    return false;
    }

    uint16_t port_num = static_cast<uint16_t>(atoi(port_string.data()));
//    std::cout << "port: " << port_num << std::endl;

    ip      = ip_string;
    port    = port_num;

    return true;
}

} // namespace simpleapi

