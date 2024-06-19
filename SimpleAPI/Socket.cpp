#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

Packet convert_to_packet(const std::string& str) {
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}

Packet convert_to_packet(const char *str) {
    return convert_to_packet(std::string(str));
}

std::string to_string(const Packet& packet) {
    return std::string((char*)packet.data(), packet.size());
}

uint8_t Socket::packHeader(const PacketType type, const uint8_t version,
                           const bool isFirstFragment, const bool isChip,
                           const CRC crcLevel) {
    return (type << 6)
           | (version << 4)
           | (isFirstFragment << 3)
           | (isChip << 2)
           | crcLevel;
}

EECounter Socket::getSeqNumber(const std::string& remoteIP, const uint16_t remotePort) {
    auto it = mapActiveConnections.find(ipPort);
    if(it == mapActiveConnections.end())
        it = mapActiveConnections.insert(std::make_pair(ipPort, 0)).first;

    return it->second++;
}

Socket::Socket() : mSocketFD(-1), maxLength(1500) {}

bool Socket::sendRawMsg(const IpPort &ipPort, const Packet &packet)
{
    std::string ip = toIp(ipPort);
    uint16_t port = toPort(ipPort);
    return sendRawMsg()
}

bool Socket::isServerActive() {
    return mSocketFD > 0;
}

void Socket::enableCRC(CRC crcLevel) {
    this->crcLevel = crcLevel;
}

void Socket::setMaxLength(uint16_t newMaxSize) {
    maxLength = newMaxSize;
}

void Socket::setUseApiVersion(ApiVersion version) {
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

void UDPSocket::sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet)
{
    std::cout << "Message to socket (" << packet.size() << " bytes)" << std::endl;

    //=CHIP_and_CRC_and_SIZE_and_DATA===========================================
    Packet innerData;
    //оставляем место для поля CRC
    switch(crcLevel) {
    case eCRC_8:
        innerData.push_back(0);
        break;
    case eCRC_16:
        innerData.push_back(0);
        innerData.push_back(0);
        break;
    case eCRC_32:
        innerData.push_back(0);
        innerData.push_back(0);
        innerData.push_back(0);
        innerData.push_back(0);
        break;
    default:        break;
    }
    //упаковка размера сообщения
    innerData.push_back(packet.size() >> 8); //NOTE: emplace_back отказывается работать
    innerData.push_back(packet.size() & 0xFF);
    //упаковка данных во временный пакет
    size_t tempSize = innerData.size();
    innerData.resize(tempSize + packet.size());
    std::copy(packet.begin(),
              packet.end(),
              innerData.begin() + tempSize);
    //обновляем поле CRC
    switch(crcLevel) {
    case eCRC_8:    utils::checkCrc8(innerData);    break;
    case eCRC_16:   utils::checkCrc16(innerData);   break;
    case eCRC_32:   utils::checkCrc32(innerData);   break;
    default:        break;
    }
    //применяем шифрование на данный пакет
//TODO:    chiphering(innerData);
    //==========================================================================

    //подготавливаем пакеты к отправке
    std::string stype;
    switch(type) {
    case eControlType:  stype = "CONTROL";  break;
    case eDataType:     stype = "DATA";     break;
    case eJsonType:     stype = "JSON";     break;
    default:            stype = "UNKNOWN";
    }
    std::cout << "Prepare to send [" << stype << "] (" << innerData.size() << " bytes), MTU=" << this->maxLength << std::endl;

    Packet      fragment;
    size_t      currentPos = 0;
    uint16_t    currentFragmentSize;

    bool isFirstFragment = true;
    while(innerData.size() - currentPos > 0) {
        uint16_t availableSize = this->maxLength;

        //упаковываем фрагмент
        Packet buf;
        buf.push_back(packHeader(type, useApiVersion, isFirstFragment, isChiphering(), crcLevel));
        buf.push_back(getSeqNumber(toIpPort(remoteIP, remotePort))); //для текущего клиента
        availableSize -= buf.size();

        //высчитываем размер данных
        uint16_t leftSize = innerData.size() - currentPos;
        currentFragmentSize = leftSize > availableSize ? availableSize : leftSize;
        if(isFirstFragment) //первый пакет в списке
            isFirstFragment = false;
        fragment.resize(currentFragmentSize);
        std::copy(innerData.begin() + currentPos,
                  innerData.begin() + currentPos + currentFragmentSize,
                  fragment.begin());

        tempSize = buf.size();
        buf.resize(fragment.size() + tempSize);
        std::copy(fragment.begin(), fragment.end(), buf.begin() + tempSize);

        //помещаем получившийся фрагмент в очередь на отправку
        this->outputThreadsMutex.lock();
        //TODO: вынести работу с мьютексом за цикл, чтобы за один раз положить все элементы
        Message message;
        message.remoteIP    = remoteIP;
        message.remotePort  = remotePort;
        message.packet      = packet;
        mapSendPacketsBuffer.push_back(message);

        this->outputThreadsMutex.unlock();

        //дальнейшая обработка пакета происходит в функции tick()


        //определяем расположение следующего фрагмента
        currentPos += currentFragmentSize;
    }
}

void UDPSocket::tick() {

}

void UDPSocket::sendAutoMsg()
{

}

void tick() {

}

UDPSocket::UDPSocket(uint16_t localPort, std::string localIP) {
    open(localPort, localIP);
}

UDPSocket::~UDPSocket() {
    close();
}

bool UDPSocket::sendRawMsg(const std::string &remoteIP, const uint16_t remotePort, const Packet &packet)
{
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr))
        std::cout << "inet_pton(): return ERROR" << std::endl;
    int res = sendto(mSocketFD, (char*)packet.data(), packet.size(), 0, (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
    if(res < 0) {
        std::cout << "ErrNo: " << errno << std::endl;
        return false;
    }
    std::cout << "Sent " << res << " bytes" << std::endl;;
    return res > 0;
}

ReceivedPacket UDPSocket::recvRawMsg(int timeout) {
    if(!this->isServerActive()) return {};

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
        return ReceivedPacket();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(mSocketFD, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
    }

    if(recv_num < 0) {
//        std::cout << "Error reading msg" << std::endl;
    } else if(recv_num > 0) {
        ReceivedPacket rpacket;
        rpacket.senderIp.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket.senderIp.data(), INET_ADDRSTRLEN);
        rpacket.senderPort = ntohs(sock.sin_port);
        rpacket.packet = Packet(buf, buf + recv_num);

        return rpacket;
    }

    return {};

}

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
    //проверка адреса назначения
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr)) {
        std::cout << "inet_pton(): return ERROR" << std::endl;
        return false;
    }

    sendFragments(remoteIP, remotePort, eDataType, packet);
    return true;
}

bool UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json) {
    //проверка адреса назначения
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr)) {
        std::cout << "inet_pton(): return ERROR" << std::endl;
        return false;
    }

    //отправит Json в текстовом формате без пробелов
    sendFragments(remoteIP, remotePort, eJsonType, convert_to_packet(json.to_string(-1)));
    return true;
}

