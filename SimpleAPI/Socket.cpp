#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>


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
    return (pm.type << 7)
           | (pm.version << 5)
           | (pm.isFirstFragment << 4)
           | (pm.isLastFragment << 3)
           | (pm.isChip << 2)
           | pm.crcLevel;
}

void Socket::unpackHeader(uint8_t header, PacketHeader& ph) {
    ph.type             = (PacketType)(header >> 7); //1
    ph.version          = (ApiVersion)((header >> 5) & 0x3); //2
    ph.isFirstFragment  = (header >> 4) & 0x1; //1
    ph.isLastFragment   = (header >> 3) & 0x1; //1
    ph.isChip           = (header >> 2) & 0x1; //1
    ph.crcLevel         = (CRC)(header & 0x3); //2
}

EECounter Socket::getOutSeqNumber(const IpPort& ipPort) {
    auto it = mapConnections.find(ipPort);
    if(it == mapConnections.end())
        it = mapConnections.insert(std::pair<IpPort, Connection>(ipPort, {EECounter(255), EECounter(255)})).first;

    return it->second.outSn;
}

EECounter Socket::getInSeqNumber(const IpPort& ipPort) {
    auto it = mapConnections.find(ipPort);
    if(it == mapConnections.end())
        it = mapConnections.insert(std::pair<IpPort, Connection>(ipPort, {EECounter(255), EECounter(255)})).first;

    return it->second.inSn;
}

PacketMessage Socket::buildPacket(PacketMessage receivedPM)
{
//TODO: сборка пакетов
    return {};
}

Socket::Socket() : mSocketFD(-1) {
    settings.maxLength          = 1500;
    settings.maxMsgsSentOnTick  = -1;
    settings.inactivityTimer    = 1000;
}

bool Socket::sendRawMsg(const PacketMessage &packetMessage) {
    return sendRawMsg(packetMessage.ipPort.ip, packetMessage.ipPort.port, packetMessage.packet);
}

bool Socket::isServerActive() {
    return mSocketFD > 0;
}

void Socket::enableCRC(CRC crcLevel) {
    this->settings.crcLevel = crcLevel;
}

bool Socket::sendMsg(const std::string& remoteIp, const uint16_t remotePort, const Packet& packet) {
    if(!checkCorrectIp(remoteIp)) return false;

    return sendMsg(IpPort{remoteIp, remotePort}, packet);
}

bool Socket::sendMsg(const std::string& remoteIp, const uint16_t remotePort, const Json& json) {
    if(!checkCorrectIp(remoteIp)) return false;

    return sendMsg(IpPort{remoteIp, remotePort}, json);
}

void Socket::setMaxLength(uint16_t newMaxSize) {
    settings.maxLength = newMaxSize;
}

void Socket::setUseApiVersion(ApiVersion version) {
    switch(version) {
//    case ...
    default: //NOTE: по умолчанию всегда самая последняя из списка!
    case Version_1: settings.useApiVersion = version;    break;
    }
}

void Socket::close() {
    if(mSocketFD) {
        ::close(this->mSocketFD);
        std::cout << "The socket " << IpPort{this->localIP, this->localPort}.to_string() << " has been freed" << std::endl;
        this->mSocketFD = -1;
    }
}

void Socket::sendFragments(const std::string& remoteIp, const uint16_t remotePort, const PacketType type, const Packet& packet) {
    sendFragments(IpPort{remoteIp, remotePort}, type, packet);
}

void UDPSocket::sendFragments(const IpPort &remoteIpPort, const PacketType type, const Packet &packet) {
    Json json;
    json.parseJson(convert_from_packet(packet));

    std::cout << "Send: " << to_string(type) << " ["
              << (json.isEmpty() ? "Data:0x" + utils::to_hex_string(packet) : "Json:" + json.to_string(-1))
              << "] --(to)--> " << remoteIpPort.to_string() << std::endl;

    Packet innerData;
    //=CHIP_and_CRC_and_SIZE_and_DATA===========================================
    //оставляем место для поля CRC
    switch(settings.crcLevel) {
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
        switch(settings.crcLevel) {
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

    bool isFirstEECsaved = false;
    EECounter firstSn(255);

    std::vector<PacketMessage> fragments;
    bool isStart    = true;
    bool isFinish   = false;
    while(innerData.size() - currentPos > 0) {
        uint16_t availableSize = this->settings.maxLength;

        EECounter fragment_sn = getOutSeqNumber(remoteIpPort);
        if(!isFirstEECsaved)
            firstSn = fragment_sn;

        //упаковываем фрагмент
        Packet buf;
        {
            //высчитываем размер данных
            uint16_t leftSize = innerData.size() - currentPos;
            currentFragmentSize = leftSize > availableSize ? availableSize : leftSize;
            fragment.resize(currentFragmentSize);
            std::copy(innerData.begin() + currentPos,
                      innerData.begin() + currentPos + currentFragmentSize,
                      fragment.begin());

            //формируем заголовок
            PacketHeader ph;
            {
                isFinish = currentFragmentSize == leftSize;

                ph.type     = type;
                ph.version  = settings.useApiVersion;
                ph.isFirstFragment = isStart;
                ph.isLastFragment = isFinish;
                ph.isChip   = isChiphering();
                ph.isChip   = isChiphering();
                ph.crcLevel = settings.crcLevel;

                if(isStart) //первый пакет в списке
                    isStart = false;
            }
            buf.push_back(packHeader(ph));
            buf.push_back(fragment_sn.get_add()); //для текущего клиента
            availableSize -= buf.size();

            tempSize = buf.size();
            buf.resize(fragment.size() + tempSize);
            std::copy(fragment.begin(), fragment.end(), buf.begin() + tempSize);
        }

        PacketMessage pm;
        {
            pm.ipPort   = remoteIpPort;
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
            this->sendPacketsBuffer.push_front(*back_it); //контрольные пакеты имеют приоритет при отправке
    } else {
        for(PacketMessage& p : fragments)
            this->sendPacketsBuffer.push_back(p);
    }
    this->outputThreadsMutex.unlock();
    //дальнейшая обработка пакета происходит в функции tick()

    PacketMessage pm;
    {
        pm.ipPort   = remoteIpPort;
        pm.packet   = packet;
        pm.type     = type;
        pm.sn       = firstSn;
    }
    this->sentGlobalPackets.push_back(pm);
}

void UDPSocket::tick() {
//TODO:    checkConnections();
    sendAutoMsg();
    recvAutoMsg(1);
}

void UDPSocket::sendAutoMsg() {
    int counter = 0; //общий счётчик за проход функции
    this->outputThreadsMutex.lock();

    //перепосылка недоставленных пакетов
    for(auto it = this->mapAutoSentPackets.begin();
         it != this->mapAutoSentPackets.end() && ((counter < settings.maxMsgsSentOnTick) || (settings.maxMsgsSentOnTick < 0));
         it++) {
        time_point_default tp = it->first;
        tp += std::chrono::milliseconds(this->settings.inactivityTimer);
        PacketMessage pm = it->second;
        if(tp < std::chrono::system_clock::now()) { //нужно переотправить
            it = this->mapAutoSentPackets.erase(it);
            this->sendPacketsBuffer.push_front(pm);
            counter++;
        }
    }

    //постепенная отправка пакетов в сокет
    while(!this->sendPacketsBuffer.empty()
           && ((counter < settings.maxMsgsSentOnTick) || (settings.maxMsgsSentOnTick < 0))
           ) {
        PacketMessage pm = this->sendPacketsBuffer.front();
        this->sendPacketsBuffer.pop_front();
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

    //записать время прихода нового сообщения от сокета
    auto it = this->mapLastActivity.begin();
    if(it == this->mapLastActivity.end())
        it = this->mapLastActivity.insert(std::make_pair(pm.ipPort, std::chrono::system_clock::now())).first;
    it->second = std::chrono::system_clock::now();

    PacketHeader ph;
    unpackHeader(pm.packet[0], ph);
    uint8_t sequence_number = pm.packet[1]; //TODO: нужна защита от некорректного размера чтения!
//    uint16_t size = (pm.packet[2] << 8) + pm.packet[3];

    pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 4); //удалить первые две пары элементов

    bool isPacketComplete   = false; //пришёл последний фрагмент И пакет собран И дешифрация И проверка CRC
    uint8_t builded_sn      = 0;
    bool isBuildError       = false; //пришёл последний фрагмент И (пакет не собран ИЛИ !проверка CRC)

    //TODO: сборка пакетов
    PacketMessage b_pm = buildPacket(pm);
    JsonMessage jm = b_pm;

    if(!b_pm.packet.empty()) {
        std::cout << "Recv: " << to_string(b_pm.type) << " ["
                  << (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(b_pm.packet)
                                        : "Json:" + jm.json.to_string(-1))
                  << "] <-(from)- " << b_pm.ipPort.to_string() << std::endl;

        //обработка собранного пакета (1 за проход)
        switch(ph.type) {
        case eControlType: {
            if(jm.json.contains("ack_sn")) {
                uint8_t sn = *jm.json["ack_sn"].getNum();
                for(auto& it : this->mapAutoSentPackets) {
                    if(it.second.sn.get() == sn) {
                        this->mapAutoSentPackets.erase(it.first);
                        break;
                    }
                }
            }
            if(jm.json.contains("ack_all_packet")) {
                uint8_t first_sn = *jm.json["ack_all_packet"].getNum(); //номер первого фрагмента сообщения

                for(auto it = this->sentGlobalPackets.begin(); it != this->sentGlobalPackets.end(); it++) {
                    if(it->sn.get() == first_sn) {
                        it = this->sentGlobalPackets.erase(it);
                        break;
                    }
                }
            }
            break;
        }
        case eDataType: {
            if(jm.json.isEmpty())
                this->mapRecvPacketsBuffer.push_back(b_pm);
            else
                this->mapRecvJsonsBuffer.push_back(jm);
            break;
        }
        default: std::cout << "Error: unknown received type(" << ph.type << ")" << std::endl;
        }
    }


    Json controlAcknoledge;
    if(ph.type != eControlType) {
        std::cout << "Send acknowledge for message(" << pm.sn.get() << ")" << std::endl;
        controlAcknoledge.put("ack_sn", (double)pm.sn.get()); //TODO: общий тип для всех числовых значений
    }
    if(b_pm.isBuiltComplete)
        controlAcknoledge.put("ack_all_packet", (double)builded_sn);
    if(b_pm.incorrectCRC)
        controlAcknoledge.put("packet_error", (double)builded_sn); //TODO: проверка ошибок и переотправка

    if(!controlAcknoledge.isEmpty())
        Socket::sendFragments(pm.ipPort, eControlType, convert_to_packet(controlAcknoledge.to_string(-1)));
}

void UDPSocket::setMaxMsgsSentOnTick(int count) {
    settings.maxMsgsSentOnTick = count;
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
        rpacket.ipPort.ip.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket.ipPort.ip.data(), INET_ADDRSTRLEN);
        rpacket.ipPort.port = ntohs(sock.sin_port);
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

bool UDPSocket::sendMsg(const IpPort& remoteIpPort, const Packet& packet) {
    sendFragments(remoteIpPort, eDataType, packet);
    return true;
}

bool UDPSocket::sendMsg(const IpPort& remoteIpPort, const Json& json) {
    //отправит Json в текстовом формате без пробелов
    sendFragments(remoteIpPort, eDataType, convert_to_packet(json.to_string(-1)));
    return true;
}

PacketMessage UDPSocket::getOutPacket()
{
    PacketMessage pm;
    if(this->mapRecvPacketsBuffer.size() > 0) {
        this->inputThreadsMutex.lock();
        pm = this->mapRecvPacketsBuffer.front();
        this->mapRecvPacketsBuffer.pop_front();
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
