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

void Socket::unpackHeader(uint8_t header, PacketHeader& ph) {
    ph.type             = (PacketType)(header >> 6); //2
    ph.version          = (ApiVersion)((header >> 4) & 0xFF); //2
    ph.isFirstFragment  = (header >> 3) & 0xF; //1
    ph.isChip           = (header >> 2) & 0xF; //1
    ph.crcLevel         = (CRC)(header & 0xFF); //2
}

//TODO: Socket::getSeqNumber()
EECounter Socket::getSeqNumber(const IpPort& ipPort) {
    auto it = mapActiveConnections.find(ipPort);
    if(it == mapActiveConnections.end())
        it = mapActiveConnections.insert(std::pair<IpPort, EECounter>(ipPort, EECounter(255))).first;

//    std::cout << "current sn: " << it->second.get() << std::endl;

    return it->second++;
}

Socket::Socket() :
    mSocketFD(-1),
    maxLength(1500),
    maxMsgsSentOnTick(-1),
    inactivityTimer(1000) {}

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
        std::cout << "The socket " << IpPort{this->localIP, this->localPort}.to_string() << " has been freed" << std::endl;
        this->mSocketFD = -1;
    }
}

void Socket::sendFragments(const IpPort &remoteIpPort, const PacketType type, const Packet &packet) {
    sendFragments(remoteIpPort.ip, remoteIpPort.port, type, packet);
}

void UDPSocket::sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet)
{
    Json json;
    json.parseJson(convert_from_packet(packet));

    std::cout << "Send: " << to_string(type) << " ["
              << (json.isEmpty() ? "Data:0x" + utils::to_hex_string(packet) : "Json:" + json.to_string(-1))
              << "] --(to)--> " << IpPort{remoteIP, remotePort}.to_string() << std::endl;

    Packet innerData;
    //=CHIP_and_CRC_and_SIZE_and_DATA===========================================
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

    size_t tempSize;
    //упаковка размера сообщения
    innerData.push_back(packet.size() >> 8); //NOTE: emplace_back отказывается работать
    innerData.push_back(packet.size() & 0xFF);
    //упаковка данных во временный пакет
    tempSize = innerData.size();
    innerData.resize(tempSize + packet.size());
    std::copy(packet.begin(), packet.end(), innerData.begin() + tempSize);
    if(type != eControlType) {
        //обновляем поле CRC
        switch(crcLevel) {
        case eCRC_8:    utils::checkCrc8(innerData);    break;
        case eCRC_16:   utils::checkCrc16(innerData);   break;
        case eCRC_32:   utils::checkCrc32(innerData);   break;
        default:        break;
        }
        //применяем шифрование на данный пакет
        chiphering(innerData);
    }
    //==========================================================================

    //подготавливаем пакеты к отправке
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
            pm.ip       = remoteIP;
            pm.port     = remotePort;
            pm.packet   = buf;
            pm.sn       = fragment_sn;
            pm.type     = type;
        }
        fragments.push_back(pm); //запоминаем фрагмент

        //определяем расположение следующего фрагмента
        currentPos += currentFragmentSize;
    }

    //помещаем получившиеся фрагменты в очередь на отправку
    this->outputThreadsMutex.lock();
    if(type == eControlType) {
        /*обратный порядок упаковки, чтобы раздробленное контрольное сообщение ушло
         * в правильном порядке, но с приоритетом */
        for(auto back_it = fragments.rbegin(); back_it != fragments.rend(); back_it++)
            this->mapSendPacketsBuffer.push_front(*back_it); //контрольные пакеты имеют приоритет при отправке
    } else {
        for(PacketMessage& p : fragments)
            this->mapSendPacketsBuffer.push_back(p);
    }
    this->outputThreadsMutex.unlock();
    //дальнейшая обработка пакета происходит в функции tick()

    PacketMessage pm;
    pm.packet   = packet;
    pm.ip       = remoteIP;
    pm.port     = remotePort;
    pm.type     = type;
    this->mapSentGlobalPackets.push_back(pm);
}

void UDPSocket::tick() {
    sendAutoMsg();
    recvAutoMsg(1);
}

void UDPSocket::sendAutoMsg() {
    int counter = 0; //общий счётчик за проход функции
    this->outputThreadsMutex.lock();

    //перепосылка недоставленных пакетов
    for(auto it = this->mapAutoSentPackets.begin();
         it != this->mapAutoSentPackets.end() && ((counter < maxMsgsSentOnTick) || (maxMsgsSentOnTick < 0));
         it++) {
        time_point_default tp = it->first;
        tp += std::chrono::milliseconds(this->inactivityTimer);
        PacketMessage pm = it->second;
        if(tp < std::chrono::system_clock::now()) { //нужно переотправить
            it = this->mapAutoSentPackets.erase(it);
            this->mapSendPacketsBuffer.push_front(pm);
            counter++;
        }
    }

    //постепенная отправка пакетов в сокет
    while(!this->mapSendPacketsBuffer.empty()
           && ((counter < maxMsgsSentOnTick) || (maxMsgsSentOnTick < 0))
           ) {
        PacketMessage pm = this->mapSendPacketsBuffer.front();
        this->mapSendPacketsBuffer.pop_front();
        Socket::sendRawMsg(pm); //отправили

        if(pm.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
            //запоминаем для ожидания ответа или досылки
            time_point_default current_time = std::chrono::system_clock::now();
            mapAutoSentPackets.insert(std::make_pair(current_time, pm));
        }

        counter++;
    }

    this->outputThreadsMutex.unlock();
}

void UDPSocket::recvAutoMsg(int timeout) {
    PacketMessage pm = recvRawMsg(1);
    if(pm.packet.empty()) return;

    PacketHeader ph;
    unpackHeader(pm.packet[0], ph);
    uint8_t sequence_number = pm.packet[1]; //TODO: нужна защита от некорректного размера чтения!
    uint16_t size = (pm.packet[2] << 8) + pm.packet[3];

    pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 4); //удалить первые две пары элементов
    if(ph.type != eControlType)
        sendAutoAck(sequence_number, {pm.ip, pm.port});

    Json json;
    json.parseJson(convert_from_packet(pm.packet));

    std::cout << "Recv: " << to_string(ph.type) << " ["
              << (json.isEmpty() ? "Data:0x" + utils::to_hex_string(pm.packet) : "Json:" + json.to_string(-1))
              << "] <-(from)- " << IpPort{pm.ip, pm.port}.to_string() << std::endl;

    switch(ph.type) {
    case eControlType: {
        if(json.contains("ack_sn")) {
            uint8_t sn = *json["ack_sn"].getNum();
//TODO:            auto it = FindSentSn(sn);
            for(auto& it : this->mapAutoSentPackets) {
                if(it.second.sn.get() == sn) {
                    this->mapAutoSentPackets.erase(it.first);
                    break;
                }
            }
        }
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
}

void UDPSocket::sendAutoAck(uint8_t sn, const IpPort& ipPort)
{
    std::cout << "Send acknowledge for message(" << sn << ")" << std::endl;

    Json jAck;
    jAck.put("ack_sn", (double)sn); //TODO: общий тип для всех числовых значений

    Socket::sendFragments(ipPort, eControlType, convert_to_packet(jAck.to_string(-1)));
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
//    std::cout << "Sent (" << res << ") -> " << IpPort{remoteIP, remotePort}.to_string() << std::endl;
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
        std::cout << "Error reading msg" << std::endl;
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
    this->localIP = localIP;
    this->localPort = localPort;

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
    std::cout << "Socket binded at " << IpPort{str, localPort}.to_string() << std::endl;
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

PacketMessage UDPSocket::getOutPacket()
{
    PacketMessage pm;
    if(this->mapRecvPacketsBuffer.size() > 0) {
        this->inputThreadsMutex.lock();
        pm = this->mapRecvPacketsBuffer.front();
        this->mapSendPacketsBuffer.pop_front();
        this->inputThreadsMutex.unlock();
    }
    return pm;
}

JsonMessage UDPSocket::getOutJson()
{
    JsonMessage jm;
    if(this->mapRecvJsonsBuffer.size() > 0) {
        this->inputThreadsMutex.lock();
        jm = this->mapRecvJsonsBuffer.front();
        this->mapRecvJsonsBuffer.pop_front();
        this->inputThreadsMutex.unlock();
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

std::string PacketMessage::to_string()
{
    std::string out;

    out = "[" + this->ip + ":" + std::to_string(this->port) + "] ";
    out += "[(" + std::to_string(this->packet.size()) + ") " + ::to_string(this->packet) + "]";

    return out;
}

std::string JsonMessage::to_string(int arg)
{
    std::string out;

    out = "[" + this->ip + ":" + std::to_string(this->port) + "] ";
    out += "[(" + std::to_string(this->json.size()) + ")" + this->json.to_string(arg) + "]";

    return out;
}

std::string to_string(PacketType type) {
    switch(type){
    case eControlType:  return "[CONTROL]";
    case eDataType:     return "[DATA]";
    case eJsonType:     return "[JSON]";
    default: return "UNKNOWN";
    }
}
