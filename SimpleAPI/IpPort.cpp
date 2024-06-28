#include "IpPort.h"


const bool IpPort::operator==(const IpPort &other) {
    if(this->ip == other.ip && this->port == other.port)    return true;
    else                                                    return false;
}

const bool IpPort::operator!=(const IpPort &other) {
    if(this->ip != other.ip || this->port != other.port)    return true;
    else                                                    return false;
}

const bool IpPort::operator<(const IpPort &other) const
{
    if(this->port == other.port) {
        if(this->ip < other.ip) return true;
        else                    return false;
    }

    if(this->port < other.port) return true;
    else                        return false;
}

const bool IpPort::operator>(const IpPort &other) const
{
    if(this->port == other.port) {
        if(this->ip > other.ip) return true;
        else                    return false;
    }

    if(this->port > other.port) return true;
    else                        return false;
}

std::string IpPort::to_string() { return "[" + this->ip + ":" + std::to_string(this->port) + "]"; }
