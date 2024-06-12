#include "Socket.h"
#include <iostream>

#include <unistd.h>
//#include <netdb.h>
//#include <sys/types.h>
#include <sys/select.h>
//#include <sys/un.h>
#include <errno.h>
//#include <list>
//#include <assert.h>
//#include <stdint.h>
//#include <stdio.h>

Packet convert_to_packet(const std::string& str)
{
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}

Packet convert_to_packet(const char *str) { return convert_to_packet(std::string(str)); }

std::string to_string(const Packet& packet)
{
    return std::string((char*)packet.data(), packet.size());
}

uint8_t Socket::packHeader(const PacketType type, const uint8_t version,
                           const bool isFirstFragment, const bool isChip, const CRC crcLevel)
{
    return (type << 6) | (version << 4) | (isFirstFragment << 3) | (isChip << 2) | crcLevel;
}

SNumber Socket::getSeqNumber(const struct sockaddr_in& sock)
{
    std::string ipKey;
    ipKey.resize(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(sock.sin_addr), (char*)ipKey.data(), INET_ADDRSTRLEN);
    ipKey += htons(sock.sin_port);

    auto it = mapActiveConnections.find(ipKey);
    if(it == mapActiveConnections.end()) {
        it = mapActiveConnections.insert(std::make_pair(ipKey, 0)).first;
    }

    return it->second++;
}

Socket::Socket() : mSocketFD(-1), maxLength(1500) {}

bool Socket::isActive() { return mSocketFD > 0; }

void Socket::enableCRC(CRC crcLevel) { this->crcLevel = crcLevel; }

void Socket::setMaxLength(uint16_t newMaxSize) { maxLength = newMaxSize; }

void Socket::setUseApiVersion(ApiVersion version)
{
    switch(version) {
//    case ...
    default: //NOTE: по умолчанию всегда самая последняя из списка!
    case Version_1: useApiVersion = version;    break;
    }
}

void Socket::close() {
    if(mSocketFD) {
        ::close(this->mSocketFD);
        std::cout << "The socket has been freed" << std::endl;
        this->mSocketFD = -1;
    }
}

bool UDPSocket::sendFragments(const PacketType type, sockaddr_in &sock, const Packet &packet)
{
    std::cout << "Message to socket (" << packet.size() << " bytes)" << std::endl;

    Packet bigMsg;
    //оставляем место для поля CRC
    switch(crcLevel) {
    case eCRC_8:
        bigMsg.push_back(0);
        break;
    case eCRC_16:
        bigMsg.push_back(0);
        bigMsg.push_back(0);
        break;
    case eCRC_32:
        bigMsg.push_back(0);
        bigMsg.push_back(0);
        bigMsg.push_back(0);
        bigMsg.push_back(0);
        break;
    default:        break;
    }
    //упаковка размера сообщения
    bigMsg.push_back(packet.size() >> 8); //NOTE: emplace_back отказывается работать
    bigMsg.push_back(packet.size() & 0xFF);
    //упаковка данных во временный пакет
    size_t tempSize = bigMsg.size();
    bigMsg.resize(tempSize + packet.size());
    std::copy(packet.begin(), packet.end(), bigMsg.begin() + tempSize);
//    bigMsg.emplace_back(packet.data()); //не работает
    //обновляем поле CRC
    switch(crcLevel) {
    case eCRC_8:    utils::checkCrc8(bigMsg);  break;
    case eCRC_16:   utils::checkCrc16(bigMsg); break;
    case eCRC_32:   utils::checkCrc32(bigMsg); break;
    default:        break;
    }

    std::string stype;
    switch(type) {
    case eControlType:  stype = "CONTROL";  break;
    case eDataType:     stype = "DATA";     break;
    case eJsonType:     stype = "JSON";     break;
    default:            stype = "UNKNOWN";
    }
    std::cout << "Prepare to send [" << stype << "] (" << bigMsg.size() << " bytes), MTU=" << this->maxLength << std::endl;

    Packet      fragment;
    size_t      currentPos = 0;
    uint16_t    currentFragmentSize;

    bool isFirstFragment = true;
    while(bigMsg.size() - currentPos > 0) {
        uint16_t availableSize = this->maxLength;

        //упаковываем фрагмент
        Packet buf;
        buf.push_back(packHeader(type, useApiVersion, isFirstFragment, isChiphering(), crcLevel));
        buf.push_back(getSeqNumber(sock)); //для текущего клиента
        availableSize -= buf.size();

        //высчитываем размер данных
        uint16_t leftSize = bigMsg.size() - currentPos;
        currentFragmentSize = leftSize > availableSize ? availableSize : leftSize;
        if(isFirstFragment) //первый пакет в списке
            isFirstFragment = false;
        fragment.resize(currentFragmentSize);
        std::copy(bigMsg.begin() + currentPos, bigMsg.begin() + currentPos + currentFragmentSize, fragment.begin());

//        buf.emplace_back(fragment); //не работает
        tempSize = buf.size();
        buf.resize(fragment.size() + tempSize);
        std::copy(fragment.begin(), fragment.end(), buf.begin() + tempSize);

        //отправляем фрагмент
        int res = sendto(mSocketFD, (char*)buf.data(), buf.size(), 0, (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
        if(res < 0) {
            std::cout << "ErrNo: " << errno << std::endl;
            return false;
        }
        std::cout << "Sent " << res << " bytes" << std::endl;

        //определяем расположение следующего фрагмента
        currentPos += currentFragmentSize;
    }

    return true;
}

UDPSocket::UDPSocket(uint16_t localPort, std::string localIP) {
    open(localPort, localIP);
}

UDPSocket::~UDPSocket() { close(); }

void UDPSocket::open(const uint16_t localPort, const std::string& localIP) {
    // create
    mSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketFD < 0) {
        perror("socket() failed");
    }

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(localPort);
    if(localIP.empty())
        sock.sin_addr.s_addr = INADDR_ANY;
    else {
        if(!inet_pton(AF_INET, localIP.c_str(), &sock.sin_addr.s_addr))
            std::cout << "inet_pton(): return ERROR" << std::endl;
    }
    int res = bind(mSocketFD, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0) {
        perror(std::string("bind() failed with localIP(" + localIP + ")"
                           + ", port(" + std::to_string(localPort) + ")").c_str());
        close();
        return;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr.s_addr), str, INET_ADDRSTRLEN);
    std::cout << "Socket binded at " << str << ":" << localPort << std::endl;
}

bool UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr))
        std::cout << "inet_pton(): return ERROR" << std::endl;
    return sendFragments(eDataType, sock, packet);
}

bool UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr))
        std::cout << "inet_pton(): return ERROR" << std::endl;
    return sendFragments(eJsonType, sock, convert_to_packet(json.to_string(-1))); //отправит Json в текстовом формате без пробелов
}

int UDPSocket::recvMsg(Packet& packet, const int timeout) {
    if(!this->isActive()) return -1;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mSocketFD, &fds);
    struct timeval t;
    if(timeout > 0) {
        t.tv_sec    = timeout / 1000;
        t.tv_usec   = (timeout % 1000) * 1000;
    }
    char buf[MAX_PACKET_LENGTH];
    int recv_num;

    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    socklen_t socklen = sizeof(sock);
    recv_num = select(mSocketFD + 1, &fds, NULL, NULL, (timeout > 0 ? &t : NULL));
    if(recv_num < 0) {
        if(errno != EINTR) /* Interrupted system call */
//TODO: (LOG) сделать флаг для возможности отключения/перенаправления сообщений от API
//TODO: (LOG) сделать внутреннюю функцию-логгер для API
            std::cout << "Error in select(), errno=" << errno << std::endl;
        return -1;
    }
    if(recv_num > 0) {
        recv_num = recvfrom(mSocketFD, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
//        std::cout << "recv_num: " << recv_num << std::endl;
    }

    if(recv_num < 0) {
//        std::cout << "Error reading msg" << std::endl;
        return -1;
    } else if(recv_num > 0) {
        std::string remoteIP;
        remoteIP.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)remoteIP.data(), INET_ADDRSTRLEN);
        std::cout << "ip:" << remoteIP << std::endl;
        std::cout << "port:" << ntohs(sock.sin_port) << std::endl;
        std::cout << "message[" << recv_num << "]: \"" << std::string(buf, recv_num) << "\"" << std::endl;

        packet = Packet(buf, buf + recv_num);
    }

    return recv_num;
}

