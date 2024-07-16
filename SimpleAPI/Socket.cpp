#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>


bool Socket::checkCorrectIp(const std::string& ipString) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ipString.c_str(), &sock.sin_addr.s_addr)) {
        Log(logs::eERROR, "inet_pton(): return ERROR");
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

PacketHeader Socket::unpackHeader(uint8_t header) {
    PacketHeader ph;
    ph.type             = (PacketType)(header >> 7); //1
    ph.version          = (ApiVersion)((header >> 5) & 0x3); //2
    ph.isFirstFragment  = (header >> 4) & 0x1; //1
    ph.isLastFragment   = (header >> 3) & 0x1; //1
    ph.isChip           = (header >> 2) & 0x1; //1
    ph.crcLevel         = (CRC)(header & 0x3); //2

    return ph;
}

EECounter& Socket::getOutSeqNumber(const IpPort& ipPort) {
    auto it = this->mapConnections.find(ipPort);
    if(it == this->mapConnections.end()) {
        it = this->mapConnections.insert(
            std::make_pair(ipPort,
                           Connection{EECounter(255), EECounter(255), EECounter(255)})).first;
    }

    return it->second.outSn;
}

PacketMessage Socket::buildPacket(PacketMessage receivedPM)
{
    auto it = this->mapConnections.find(receivedPM.ipPort);
    if(it == this->mapConnections.end()) {
        it = this->mapConnections.insert(
            std::make_pair(receivedPM.ipPort,
                           Connection{EECounter(255), EECounter(255), EECounter(255)})).first;
    }

    if(receivedPM.packet.empty())
        return {};

    if(it->second.inSnLastRecv < receivedPM.sn)
        it->second.inSnLastRecv = receivedPM.sn;
    //всё, что пришло до этого - удалится
    EECounter rmSn = it->second.inSnLastRecv - (it->second.inSnLastRecv.size() / 2); //размер окна - половина диапазона

    if(receivedPM.sn == it->second.inNextSn) {
        //NOTE: игнор уже пришедших фрагментов произойдёт здесь же
        it->second.mapRecvBuildedMessages.insert(std::make_pair(receivedPM.sn, receivedPM));
        it->second.inNextSn++;
    } else
        it->second.mapRecvFragments.insert(std::make_pair(receivedPM.sn, receivedPM));

    //пройтись по poolRecvMessages и добрать по порядку к mapRecvBuildedMessages
    auto it_pool = it->second.mapRecvFragments.find(it->second.inNextSn);
    while(it_pool != it->second.mapRecvFragments.end()) {
        it->second.mapRecvBuildedMessages.insert(std::make_pair(it_pool->second.sn, it_pool->second));
        it->second.inNextSn++;

        it_pool = it->second.mapRecvFragments.erase(it_pool);
        it_pool = it->second.mapRecvFragments.find(it->second.inNextSn); //ищем следующий фрагмент очереди
    }

    //удалить всё, что теперь вне окна ожидания
    it_pool = it->second.mapRecvFragments.begin();
    while(it_pool != it->second.mapRecvFragments.end()) {
        if(it_pool->first < rmSn)
            it_pool = it->second.mapRecvFragments.erase(it_pool);
        it_pool++;
    }

    //попытаться собрать ОДИН пакет
    PacketMessage pm;
    pm.isBuiltComplete      = false;
    pm.isError              = false;

    bool isStarted          = false;
    bool isFinished         = false;
    bool isFirstCounterSet  = false;
    EECounter firstCounter(255);
    EECounter lastCounter(255);

    if(!it->second.mapRecvBuildedMessages.empty()) {
        //запоминаем индексы первого встречного пакета
        for(auto it_build = it->second.mapRecvBuildedMessages.begin();
             it_build != it->second.mapRecvBuildedMessages.end(); it_build++) {
            if(!isFirstCounterSet) {
                firstCounter = it_build->first;
                isFirstCounterSet = true;

                pm.ipPort = it_build->second.ipPort;
                pm.header = it_build->second.header;
            }

            if(it_build->second.header.isLastFragment) {
                lastCounter = it_build->first;
                break;
            }
        }
        if(it->second.mapRecvBuildedMessages.find(firstCounter)->second.header.isFirstFragment)
            isStarted = true;
        if(it->second.mapRecvBuildedMessages.find(lastCounter)->second.header.isLastFragment)
            isFinished = true;

        //пакет не состоялся, удалить все фрагменты
        if(!isStarted && isFinished && (firstCounter < rmSn)) {
            auto it_build = it->second.mapRecvBuildedMessages.begin();
            while(it_build != it->second.mapRecvBuildedMessages.end()
                   && (it_build->first < lastCounter)) {
                it_build = it->second.mapRecvBuildedMessages.erase(it_build);
                it_build++;
            }
            pm.isError = true;
            pm.error.sn_finish = lastCounter;
            return pm;
        }

        //пакет соберётся, копируем в выходной PM.packet
        if(isStarted && isFinished) {
            pm.sn = firstCounter; //номер первого фрагмента для индикации доставки глобального сообщения
            //скопировать и удалить задействованные фрагменты
            auto it_build = it->second.mapRecvBuildedMessages.begin();
            while(it_build != it->second.mapRecvBuildedMessages.end() && (it_build->first <= lastCounter)) {
                std::copy(std::begin(it_build->second.packet),
                          std::end(it_build->second.packet),
                          std::back_insert_iterator<Packet>(pm.packet));
                it_build = it->second.mapRecvBuildedMessages.erase(it_build);
            }

            uint16_t size = (pm.packet[0] << 8) + pm.packet[1];
            pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 2); //размер поля данных
            //проверка ошибки размера
            if(size != pm.packet.size()) {
                pm.isError = true;
                pm.error.sn_finish = lastCounter;
                return pm;
            }
\
            //дешифрация
            dechiphering(pm.packet);
            //проверка контрольной суммы
            if(pm.header.type != eControlType) {
                //обновляем поле CRC
                switch(settings.crcLevel) {
                case eCRC_8:    pm.isError = !utils::checkCrc8(pm.packet);    break;
                case eCRC_16:   pm.isError = !utils::checkCrc16(pm.packet);   break;
                case eCRC_32:   pm.isError = !utils::checkCrc32(pm.packet);   break;
                default:        break;
                }
            }

            if(!pm.isError)
                pm.isBuiltComplete = true;
        }
    }

    return pm;
}

void Socket::Log(logs::LEVEL level, std::string log_message)
{
    if(level <= this->logLevel) {
        switch(level) {
        case logs::eINFO:
        case logs::eDEBUG: {
            if(this->logCallback)
                this->logCallback(to_color_string(level, to_string(this->mSocketType)) + " " + log_message + "\n");
            break;
        }
        case logs::eWARNING:
        case logs::eERROR:
        default: {
            if(this->logErrorCallback)
                this->logErrorCallback(to_color_string(level, to_string(this->mSocketType)) + " " + log_message + "\n");
            break;
        }
        }
    }
}

void Socket::setCallbackLogOutput(void (*callback)(std::string)) {
    this->logCallback = callback;
}

void Socket::setCallbackLogErrorOutput(void (*callback)(std::string)) {
    this->logErrorCallback = callback;
}

void Socket::setCallbackSocketReadRawData(void (*callback)(PacketMessage)) {
    this->packetCallback = callback;
}

void Socket::setCallbackSocketReadJsonData(void (*callback)(JsonMessage)) {
    this->jsonCallback = callback;
}

Socket::Socket() :
    mSocketFD(-1), logLevel(logs::eINFO),
    packetCallback(nullptr), jsonCallback(nullptr),
    logCallback(nullptr), logErrorCallback(nullptr)
{
    settings.maxLength          = 1500;
    settings.maxMsgsSentOnTick  = -1;
    settings.inactivityTimer    = 5000;
}

void Socket::setLogLevel(logs::LEVEL logLevel) {
    this->logLevel = logLevel;
}

void Socket::setLogErrorLevel(logs::LEVEL logLevel) {
    this->logErrorLevel = logLevel;
}

bool Socket::sendRawMsg(const PacketMessage &packetMessage) {
    Log(logs::eDEBUG, "sendRawMsg()");
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
        Log(logs::eINFO, "The socket " + IpPort{this->localIP, this->localPort}.to_string() + " has been freed");
        this->mSocketFD = -1;
    }
}

void Socket::sendFragments(const std::string& remoteIp, const uint16_t remotePort, const PacketType type, const Packet& packet) {
    sendFragments(IpPort{remoteIp, remotePort}, type, packet);
}

void UDPSocket::sendFragments(const IpPort &remoteIpPort, const PacketType type, const Packet &packet) {
    Json json;
    json.parseJson(convert_from_packet(packet));

    Log(logs::eINFO, "send: " + to_string(type) + " ["
                         + (json.isEmpty() ? "Data:0x" + utils::to_hex_string(packet) : "Json:" + json.to_string(-1))
                         + "] --(to)--> " + remoteIpPort.to_string());

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
    innerData.push_back(packet.size() >> 8);
    innerData.push_back(packet.size() & 0xFF);
//    std::cout << "send size: " << (packet.size() >> 8) << ", " << (packet.size() & 0xFF) << std::endl;
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
    EECounter lastSn(255);

    std::vector<PacketMessage> fragments;
    bool isStart    = true;
    bool isFinish   = false;
    while(innerData.size() - currentPos > 0) {
        uint16_t availableSize = this->settings.maxLength;

        EECounter& fragment_sn = getOutSeqNumber(remoteIpPort);
        if(!isFirstEECsaved)
            firstSn = fragment_sn;

        EECounter saved = fragment_sn;
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
            saved = fragment_sn;
            buf.push_back(fragment_sn.get_glob()); //для текущего клиента
            buf.push_back(fragment_sn.get_add()); //для текущего клиента
//            availableSize -= buf.size(); //not used after

            tempSize = buf.size();
            buf.resize(fragment.size() + tempSize);
            std::copy(fragment.begin(), fragment.end(), buf.begin() + tempSize);
        }

        PacketMessage pm;
        {
            pm.ipPort       = remoteIpPort;
            pm.packet       = buf;
            pm.sn           = saved; //для записи НЕ инкрементированного значения
            pm.header.type  = type;
        }
        fragments.push_back(pm); //запоминаем фрагмент
        lastSn = saved; //для определения границ глобального пакета

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
        pm.ipPort       = remoteIpPort;
        pm.packet       = packet;
        pm.header.type  = type;
        //границы глобального пакета
        pm.range.start  = firstSn;
        pm.range.finish = lastSn;
    }
    this->sentGlobalPackets.push_back(pm);
}

void UDPSocket::tick() {
    checkConnections();

    sendAutoMsg();
    recvAutoMsg(1);

    PacketMessage pm = getOutPacket();
    if(!pm.packet.empty() && this->packetCallback)
        this->packetCallback(pm);
    JsonMessage jm = getOutJson();
    if(!jm.json.isEmpty() && this->jsonCallback)
        this->jsonCallback(jm);
}

void UDPSocket::checkConnections()
{
    Json jPing;
    jPing.put("ping", this->getLocalIpPort().to_string());

    for(auto it = this->mapConnections.begin(); it != this->mapConnections.end(); it++) {
        //если не было сообщений ОТ адреса дольше this->inactivityTimer/2, то отправить пинг
        auto it_last = this->mapLastActivity.find(it->first);
        if(it_last == this->mapLastActivity.end()
            || it_last->second + std::chrono::milliseconds(this->settings.inactivityTimer / 2)
                   > std::chrono::system_clock::now()) {
            sendFragments(it->first, eControlType, convert_to_packet(jPing.to_string(-1)));
            continue;
        }

        //если долгое время не было сообщений от абонента, удалить все сообщения до него
        if(it_last == this->mapLastActivity.end()
            || it_last->second + std::chrono::milliseconds(this->settings.inactivityTimer)
                   > std::chrono::system_clock::now()) {
            it = this->mapConnections.erase(it);
            Log(logs::eINFO, "Connection " + it->first.to_string() + " removed");
        }

        //если дошли до конца диапазона
        if(it == this->mapConnections.end())
            break;
    }
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
            Log(logs::eDEBUG, "new try to send [" + std::to_string(it->second.sn.get()) + "] fragment");
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

        if(pm.header.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
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

    Log(logs::eDEBUG, "received raw message: [0x" + utils::to_hex_string(pm.packet) + "]");
    //записать время прихода нового сообщения от сокета
    auto it = this->mapLastActivity.begin();
    if(it == this->mapLastActivity.end())
        it = this->mapLastActivity.insert(std::make_pair(pm.ipPort, std::chrono::system_clock::now())).first;
    else
        it->second = std::chrono::system_clock::now();

    pm.header = unpackHeader(pm.packet[0]);
    uint8_t glob_sn = pm.packet[1]; //TODO: нужна защита от некорректного размера чтения!
    uint8_t sn      = pm.packet[2];
    pm.sn = EECounter(255);
    pm.sn.set_glob_pos(glob_sn);
    pm.sn.set_pos(sn);
    pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 3); //удалить первые три байта

    PacketMessage b_pm = buildPacket(pm);
    JsonMessage jm = b_pm;

    Json controlAcknoledge;
    if(pm.header.type != eControlType) {
        Log(logs::eDEBUG, "Send acknowledge for message(" + std::to_string(pm.sn.get()) + ")");
        controlAcknoledge.put("ack_sn", (double)pm.sn.get()); //TODO: общий тип для всех числовых значений
        if(b_pm.isBuiltComplete)
            controlAcknoledge.put("ack_all_packet", (double)b_pm.sn.get());
        if(b_pm.isError) {
            controlAcknoledge.put("packet_error_last_sn", (double)b_pm.error.sn_finish.get());//TODO: проверка ошибок и переотправка
        }
    }

    if(!b_pm.packet.empty()) {
        Log(logs::eINFO, "Recv: " + to_string(b_pm.header.type) + " ["
                             + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(b_pm.packet)
                                                  : "Json:" + jm.json.to_string(-1))
                             + "] <-(from)- " + b_pm.ipPort.to_string());

        //обработка собранного пакета (1 за проход)
        if(pm.header.type == eControlType) {
            if(jm.json.contains("ack_sn")) {
                uint8_t sn = jm.json["ack_sn"].getNum();
                for(auto it = this->mapAutoSentPackets.begin(); it != this->mapAutoSentPackets.end(); it++) {
                    if(it->second.sn.get() == sn) {
                        this->mapAutoSentPackets.erase(it->first);
                        break;
                    }
                }
            }
            if(jm.json.contains("ack_all_packet")) {
                uint8_t first_sn = jm.json["ack_all_packet"].getNum(); //номер первого фрагмента сообщения

                for(auto it = this->sentGlobalPackets.begin(); it != this->sentGlobalPackets.end(); it++) {
                    if(it->range.start.get() == first_sn) {
                        it = this->sentGlobalPackets.erase(it);
                        break;
                    }
                }
            }
            if(jm.json.contains("packet_error_last_sn")) {
                uint8_t last_err_sn = jm.json["packet_error_last_sn"].getNum(); //номер первого фрагмента сообщения

                //удалить все упоминания фрагментов пакета из очереди переотправок
                for(auto it = this->mapAutoSentPackets.begin(); it != this->mapAutoSentPackets.end(); it++) {
                    if(it->second.sn <= last_err_sn)
                        it = this->mapAutoSentPackets.erase(it);
                }

                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                Packet packet;
                IpPort ipPort;
                PacketType type;
                for(auto it = this->sentGlobalPackets.begin(); it != this->sentGlobalPackets.end(); it++) {
                    if(it->range.finish.get() == last_err_sn) {
                        packet  = it->packet;
                        type    = it->header.type;
                        ipPort  = it->ipPort;
                        it = this->sentGlobalPackets.erase(it);
                        break;
                    }
                }

                if(!packet.empty()) {
                    Log(logs::eDEBUG, "new try resend packet [0x" + utils::to_hex_string(packet) + "]");
                    sendFragments(ipPort, type, packet); //переотправка
                }
            }
        }
        else {
            this->inputThreadsMutex.lock();
            if(jm.json.isEmpty())
                this->mapRecvPacketsBuffer.push_back(b_pm);
            else
                this->mapRecvJsonsBuffer.push_back(jm);
            this->inputThreadsMutex.unlock();
        }
    }

    if(!controlAcknoledge.isEmpty())
        sendFragments(pm.ipPort, eControlType, convert_to_packet(controlAcknoledge.to_string(-1)));
}

void UDPSocket::setMaxMsgsSentOnTick(int count) {
    settings.maxMsgsSentOnTick = count;
}

UDPSocket::UDPSocket(const IpPort &ipPort,
                     void (*callbackRecvPacket)(PacketMessage),
                     void (*callbackRecvJson)(JsonMessage),
                     void (*callbackLog)(std::string),
                     void (*callbackLogError)(std::string),
                     const logs::LEVEL logLevel,
                     const logs::LEVEL logErrorLevel
                     ) {
    this->mSocketType       = SocketType::eUDP;

    this->packetCallback    = callbackRecvPacket;
    this->jsonCallback      = callbackRecvJson;
    this->logCallback       = callbackLog;
    this->logErrorCallback  = callbackLogError;
    this->logLevel          = logLevel;
    this->logErrorLevel     = logErrorLevel;

    open(ipPort.port, ipPort.ip);
}

UDPSocket::UDPSocket(uint16_t localPort, std::string localIP,
                     void (*callbackRecvPacket)(PacketMessage),
                     void (*callbackRecvJson)(JsonMessage),
                     void (*callbackLog)(std::string),
                     void (*callbackLogError)(std::string),
                     const logs::LEVEL logLevel,
                     const logs::LEVEL logErrorLevel
                     ) {
    this->mSocketType       = SocketType::eUDP;

    this->packetCallback    = callbackRecvPacket;
    this->jsonCallback      = callbackRecvJson;
    this->logCallback       = callbackLog;
    this->logErrorCallback  = callbackLogError;
    this->logLevel          = logLevel;
    this->logErrorLevel     = logErrorLevel;

    open(localPort, localIP);
}

UDPSocket::~UDPSocket() {
    close();
}

bool UDPSocket::sendRawMsg(const std::string &remoteIP, const uint16_t remotePort, const Packet &packet) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr))
        Log(logs::eERROR, "inet_pton(): return ERROR");
    int res = sendto(mSocketFD, (char*)packet.data(), packet.size(), 0, (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
    if(res < 0) {
        Log(logs::eERROR, "ErrNo: " + std::to_string(errno));
        return false;
    }
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
            Log(logs::eERROR, "Error in select(), errno=" + std::to_string(errno));
        return PacketMessage();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(mSocketFD, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
    }

    if(recv_num < 0) {
        Log(logs::eERROR, "Error reading msg");
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

void UDPSocket::startServer() {
    if(!isServerActive())
        open(this->localPort, this->localIP);
}

void UDPSocket::stopServer() {
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
            Log(logs::eERROR, "inet_pton(): return ERROR");
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
    Log(logs::eINFO, "Socket binded at " + IpPort{str, localPort}.to_string());
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
    this->inputThreadsMutex.lock();
    PacketMessage pm;
    if(this->mapRecvPacketsBuffer.size() > 0) {
        pm = this->mapRecvPacketsBuffer.front();
        this->mapRecvPacketsBuffer.pop_front();
    }
    this->inputThreadsMutex.unlock();

    return pm;
}

JsonMessage UDPSocket::getOutJson()
{
    this->inputThreadsMutex.lock();
    JsonMessage jm;
    if(this->mapRecvJsonsBuffer.size() > 0) {
        jm = this->mapRecvJsonsBuffer.front();
        this->mapRecvJsonsBuffer.pop_front();
    }
    this->inputThreadsMutex.unlock();

    return jm;
}

std::string to_string(SocketType type)
{
    switch(type){
    case eUDP:      return "UDP";
    case eTCP:      return "TCP";
    default:        return "SOCKET";
    }
}
