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

std::string convert_from_packet(const Packet &packet) {
    std::string str;
    str.resize(packet.size());
    std::copy(packet.begin(), packet.end(), str.begin());
    return str;
}

std::string to_string(const Packet& packet) {
    return std::string((char*)packet.data(), packet.size());
}

bool checkCorrectIp(const std::string& ipString) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ipString.c_str(), &sock.sin_addr.s_addr)) {
        std::cout << "inet_pton(): return ERROR" << std::endl;
        return false;
    }
    return true;
}

uint8_t Socket::packHeader(const PacketHeader& pm) {
    return (pm.type << 6)
           | (pm.version << 4)
           | (pm.isFirstFragment << 3)
           | (pm.isChip << 2)
           | pm.crcLevel;
}

void Socket::unpackHeader(uint8_t header, PacketHeader& pm) {
//    type =
}

EECounter Socket::getSeqNumber(const IpPort& ipPort) {
    auto it = mapActiveConnections.find(ipPort);
    if(it == mapActiveConnections.end())
        it = mapActiveConnections.insert(std::pair<IpPort, time_t>(ipPort, 0)).first;

    return it->second++;
}

Socket::Socket() : mSocketFD(-1), maxLength(1500), maxMsgsSentOnTick(-1) {}

bool Socket::sendRawMsg(const PacketMessage &packetMessage) {
    return sendRawMsg(packetMessage.ip, packetMessage.port, packetMessage.packet);
}

bool Socket::isServerActive() {
    return mSocketFD > 0;
}

void Socket::enableCRC(CRC crcLevel) {
    this->crcLevel = crcLevel;
}

bool Socket::sendMsg(const IpPort &remoteIpPort, const Packet &packet) {
    return sendMsg(remoteIpPort.ip, remoteIpPort.port, packet);
}

bool Socket::sendMsg(const IpPort &remoteIpPort, const Json &json) {
    return sendMsg(remoteIpPort.ip, remoteIpPort.port, json);
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
    std::copy(packet.begin(), packet.end(), innerData.begin() + tempSize);
    //обновляем поле CRC
    switch(crcLevel) {
    case eCRC_8:    utils::checkCrc8(innerData);    break;
    case eCRC_16:   utils::checkCrc16(innerData);   break;
    case eCRC_32:   utils::checkCrc32(innerData);   break;
    default:        break;
    }
    //применяем шифрование на данный пакет
    chiphering(innerData);
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

    std::vector<PacketMessage> fragments;
    bool isFirstFragment = true;
    while(innerData.size() - currentPos > 0) {
        uint16_t availableSize = this->maxLength;

        EECounter fragment_sn = getSeqNumber({remoteIP, remotePort});
        //упаковываем фрагмент
        Packet buf;
        {
            buf.push_back(packHeader({type, useApiVersion, isFirstFragment, isChiphering(), crcLevel}));
            buf.push_back(fragment_sn.get()); //для текущего клиента
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
        }

        PacketMessage pm;
        {
            pm.ip = remoteIP;
            pm.port = remotePort;
            pm.packet = buf;
            pm.sn = fragment_sn;
        }
        fragments.push_back(pm); //запоминаем фрагмент

        //определяем расположение следующего фрагмента
        currentPos += currentFragmentSize;
    }

    //помещаем получившиеся фрагменты в очередь на отправку
    this->outputThreadsMutex.lock();
    for(PacketMessage p : fragments)
        this->mapSendPacketsBuffer.push_back(p);
    this->outputThreadsMutex.unlock();
    //дальнейшая обработка пакета происходит в функции tick()
}

void UDPSocket::tick() {
    sendAutoMsg();
    recvAutoMsg(1);
}

//постепенная отправка пакетов в сокет
//перепосылка недоставленных пакетов
void UDPSocket::sendAutoMsg() {
    int counter = 0;
    for(PacketMessage pm : mapSendPacketsBuffer) {
        if((counter < maxMsgsSentOnTick) || (maxMsgsSentOnTick < 0)) {
            Socket::sendRawMsg(pm);             //отправили
            mapAutoSentPackets.push_back(pm);   //запомнили для ожидания ответа или досылки
        }
        counter++;
    }
}

void UDPSocket::recvAutoMsg(int timeout) {
    PacketMessage pm = recvRawMsg(1);
    PacketHeader ph;
    unpackHeader(pm.packet[0], ph);

    switch(ph.type) {
    case eControlType: {

        break;
    }
    case eDataType: {

        break;
    }
    case eJsonType: {

        break;
    }
    default: std::cout << "Error: unknown received type(" << ph.type << ")" << std::endl;
    }

    //TODO: если Json, то
//    tmpRecvJson = builtPacket;
}

void UDPSocket::setMaxMsgsSentOnTick(int count) {
    maxMsgsSentOnTick = count;
}

UDPSocket::UDPSocket(const IpPort &ipPort)
{
    open(ipPort.port, ipPort.ip);
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

PacketMessage UDPSocket::recvRawMsg(int timeout) {
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
        return PacketMessage();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(mSocketFD, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
    }

    if(recv_num < 0) {
//        std::cout << "Error reading msg" << std::endl;
    } else if(recv_num > 0) {
        PacketMessage rpacket;
        rpacket.ip.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket.ip.data(), INET_ADDRSTRLEN);
        rpacket.port = ntohs(sock.sin_port);
        rpacket.packet = Packet(buf, buf + recv_num);

        return rpacket;
    }

    return {};

}

void UDPSocket::startServer()
{
    if(!isServerActive())
        open(this->localPort, this->localIP);
}

void UDPSocket::stopServer()
{
    if(isServerActive())
        close();
}

void UDPSocket::open(const uint16_t localPort, const std::string& localIP) {
    // create
    mSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketFD < 0)
        perror("socket() failed");

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
    if(!checkCorrectIp(remoteIP)) return false;

    sendFragments(remoteIP, remotePort, eDataType, packet);
    return true;
}

bool UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json) {
    if(!checkCorrectIp(remoteIP)) return false;

    //отправит Json в текстовом формате без пробелов
    sendFragments(remoteIP, remotePort, eJsonType, convert_to_packet(json.to_string(-1)));
    return true;
}

//NOTE: ouput всегда один, потому без мьютекса
PacketMessage UDPSocket::getOutPacket()
{
    PacketMessage pm;
    if(this->mapRecvPacketsBuffer.size() > 0) {
        pm = this->mapRecvPacketsBuffer.front();
        this->mapSendPacketsBuffer.pop_front();
    }
    return pm;
}

JsonMessage UDPSocket::getOutJson()
{
    JsonMessage jm;
    if(this->mapRecvJsonsBuffer.size() > 0) {
        jm = this->mapRecvJsonsBuffer.front();
        this->mapRecvJsonsBuffer.pop_front();
    }
    return jm;
}

const bool IpPort::operator==(const IpPort &other) {
    if(this->ip == other.ip && this->port == other.port)    return true;
    else                                                    return false;
}

const bool IpPort::operator!=(const IpPort &other) {
    if(this->ip != other.ip || this->port != other.port)    return true;
    else                                                    return false;
}

const bool IpPort::operator<(const IpPort &other)
{
    if(this->port < other.port) return true;
    else                        return false;
}

const bool IpPort::operator>(const IpPort &other)
{
    if(this->port > other.port) return true;
    else                        return false;
}
