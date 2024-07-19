#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>


bool Socket::checkCorrectIp(const std::string& ip_string) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ip_string.c_str(), &sock.sin_addr.s_addr)) {
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

EECounter& Socket::getOutSeqNumber(const IpPort& ip_port) {
    auto it = m_map_connections.find(ip_port);
    if(it == m_map_connections.end()) {
        it = m_map_connections.insert(std::make_pair(ip_port, Connection())).first;
    }

    return it->second.m_out_sn;
}

PacketMessage Socket::buildPacket(PacketMessage received_pm)
{
    auto it = m_map_connections.find(received_pm.ipPort);
    if(it == m_map_connections.end()) {
        it = m_map_connections.insert(std::make_pair(received_pm.ipPort, Connection())).first;
    }
    it->second.m_last_activity = std::chrono::system_clock::now();
    Log(logs::eDEBUG, "buildPacket(), mapConnection size: " + std::to_string(m_map_connections.size()));

    if(received_pm.packet.empty()) {
        Log(logs::eDEBUG, "~buildPacket(), packet empty");
        return {};
    }

    if(it->second.m_in_sn_last_recv < received_pm.sn)
        it->second.m_in_sn_last_recv = received_pm.sn;
    //всё, что пришло до этого - удалится
    EECounter rmSn = it->second.m_in_sn_last_recv - (it->second.m_in_sn_last_recv.size() / 2); //размер окна - половина диапазона

    if(received_pm.sn == it->second.m_in_next_sn) {
        //NOTE: игнор уже пришедших фрагментов произойдёт здесь же
        it->second.m_map_recv_builded_messages.insert(std::make_pair(received_pm.sn, received_pm));
        it->second.m_in_next_sn++;
    } else
        it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.sn, received_pm));

    //пройтись по poolRecvMessages и добрать по порядку к mapRecvBuildedMessages
    auto it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn);
    while(it_pool != it->second.m_map_recv_fragments.end()) {
        it->second.m_map_recv_builded_messages.insert(std::make_pair(it_pool->second.sn, it_pool->second));
        it->second.m_in_next_sn++;

        it_pool = it->second.m_map_recv_fragments.erase(it_pool);
        it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn); //ищем следующий фрагмент очереди
        Log(logs::eDEBUG, "buildPacket(), find()");
    }

    //удалить всё, что теперь вне окна ожидания
    it_pool = it->second.m_map_recv_fragments.begin();
    while(it_pool != it->second.m_map_recv_fragments.end()) {
        if(it_pool->first < rmSn)
            it_pool = it->second.m_map_recv_fragments.erase(it_pool);
        it_pool++;
    }

    Log(logs::eDEBUG, "buildPacket(), prepare to build");
    //попытаться собрать ОДИН пакет
    PacketMessage pm;
    pm.isBuiltComplete      = false;
    pm.isError              = false;

    bool isStarted          = false;
    bool isFinished         = false;
    bool isFirstCounterSet  = false;
    EECounter firstCounter(255);
    EECounter lastCounter(255);

    if(!it->second.m_map_recv_builded_messages.empty()) {
        //запоминаем индексы первого встречного пакета
        for(auto it_build = it->second.m_map_recv_builded_messages.begin();
             it_build != it->second.m_map_recv_builded_messages.end(); it_build++) {
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
        if(it->second.m_map_recv_builded_messages.find(firstCounter)->second.header.isFirstFragment)
            isStarted = true;
        if(it->second.m_map_recv_builded_messages.find(lastCounter)->second.header.isLastFragment)
            isFinished = true;

        //пакет не состоялся, удалить все фрагменты
        if(!isStarted && isFinished && (firstCounter < rmSn)) {
            auto it_build = it->second.m_map_recv_builded_messages.begin();
            while(it_build != it->second.m_map_recv_builded_messages.end()
                   && (it_build->first < lastCounter)) {
                it_build = it->second.m_map_recv_builded_messages.erase(it_build);
                it_build++;
            }
            pm.isError = true;
            pm.error.sn_finish = lastCounter;
            Log(logs::eDEBUG, "~buildPacket(1), mapConnection size: " + std::to_string(m_map_connections.size()));
            return pm;
        }

        //пакет соберётся, копируем в выходной PM.packet
        if(isStarted && isFinished) {
            pm.sn = firstCounter; //номер первого фрагмента для индикации доставки глобального сообщения
            //скопировать и удалить задействованные фрагменты
            auto it_build = it->second.m_map_recv_builded_messages.begin();
            while(it_build != it->second.m_map_recv_builded_messages.end() && (it_build->first <= lastCounter)) {
                std::copy(std::begin(it_build->second.packet),
                          std::end(it_build->second.packet),
                          std::back_insert_iterator<Packet>(pm.packet));
                it_build = it->second.m_map_recv_builded_messages.erase(it_build);
            }

            uint16_t size = (pm.packet[0] << 8) + pm.packet[1];
            pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 2); //размер поля данных
            //проверка ошибки размера
            if(size != pm.packet.size()) {
                pm.isError = true;
                pm.error.sn_finish = lastCounter;
                Log(logs::eDEBUG, "~buildPacket(2), mapConnection size: " + std::to_string(m_map_connections.size()));
                return pm;
            }

            if(pm.header.type != eControlType) {
                //дешифрация
                dechiphering(pm.packet);
                //проверка контрольной суммы
                switch(m_settings.getCrcLevel()) {
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

    Log(logs::eDEBUG, "~buildPacket(3), mapConnection size: " + std::to_string(m_map_connections.size()));
    return pm;
}

void Socket::Log(logs::LEVEL level, std::string log_message)
{
    if(level <= m_settings.getLogLevel()) {
        switch(level) {
        case logs::eWARNING:
        case logs::eINFO:
        case logs::eDEBUG: {
            if(m_settings.getLogCallback())
                m_settings.getLogCallback()(
                    logs::get_time_string() + " "
                    + to_color_string(level, to_string(m_socket_type))
                    + " " + log_message + "\n");
            break;
        }
        case logs::eERROR:
        default: {
            if(m_settings.getLogErrorCallback())
                m_settings.getLogErrorCallback()(
                    logs::get_time_string() + " "
                    + to_color_string(level, to_string(m_socket_type))
                    + " " + log_message + "\n");
            break;
        }
        }
    }
}

Socket::Socket() :
    m_socket_fd(-1),
    m_settings(SocketSettings())
{}

bool Socket::sendRawMsg(const PacketMessage &packet_message) {
    Log(logs::eDEBUG, std::string("sendRaw ")
                          + "(" + std::to_string(packet_message.packet.size()) + ")"
                          + "[0x" + utils::to_hex_string(packet_message.packet) + "]"
                          + " to " + packet_message.ipPort.to_string());
    return sendRawMsg(packet_message.ipPort.ip, packet_message.ipPort.port, packet_message.packet);
}

bool Socket::isServerActive() {
    return m_socket_fd > 0;
}

bool Socket::sendMsg(const std::string& remote_ip, const uint16_t remote_port, const Packet& packet) {
    if(!checkCorrectIp(remote_ip)) return false;

    return sendMsg(IpPort{remote_ip, remote_port}, packet);
}

bool Socket::sendMsg(const std::string& remote_ip, const uint16_t remote_port, const Json& json) {
    if(!checkCorrectIp(remote_ip)) return false;

    return sendMsg(IpPort{remote_ip, remote_port}, json);
}

void Socket::close() {
    if(m_socket_fd) {
        ::close(m_socket_fd);
        Log(logs::eINFO, "The socket " + IpPort{m_local_ip, m_local_port}.to_string() + " has been freed");
        m_socket_fd = -1;
    }
}

void Socket::sendFragments(const std::string& remote_ip, const uint16_t remote_port, const PacketType type, const Packet& packet) {
    sendFragments(IpPort{remote_ip, remote_port}, type, packet);
}

void UDPSocket::sendFragments(const IpPort &remote_ip_port, const PacketType type, const Packet &packet) {
    Json json;
    json.parseJson(convert_from_packet(packet));

    Log(type != eControlType ? logs::eINFO : logs::eDEBUG,
        "send: " + to_string(type) + " "
            + (json.isEmpty() ? "[Data:0x" + utils::to_hex_string(packet) + "]"
                              : "[Json:" + json.to_string(-1) + "]"
                                    + " / [Data:0x" + utils::to_hex_string(packet) + "]"
               )
            + " --> " + remote_ip_port.to_string());

    Packet innerData;
    //=CHIP_and_CRC_and_SIZE_and_DATA===========================================
    //оставляем место для поля CRC
    switch(m_settings.getCrcLevel()) {
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
        switch(m_settings.getCrcLevel()) {
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
        uint16_t availableSize = m_settings.getMaxLength();

        EECounter& fragment_sn = getOutSeqNumber(remote_ip_port);
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
                ph.version  = m_settings.getApiVersion();
                ph.isFirstFragment = isStart;
                ph.isLastFragment = isFinish;
                ph.isChip   = isChiphering();
                ph.isChip   = isChiphering();
                ph.crcLevel = m_settings.getCrcLevel();

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
            pm.ipPort       = remote_ip_port;
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
    m_output_threads_mutex.lock();
    if(type == eControlType) {
        /*обратный порядок упаковки, чтобы раздробленное контрольное сообщение ушло
         * в правильном порядке, но с приоритетом */
        for(auto back_it = fragments.rbegin(); back_it != fragments.rend(); back_it++)
            m_send_packets_buffer.push_front(*back_it); //контрольные пакеты имеют приоритет при отправке
    } else {
        for(PacketMessage& p : fragments)
            m_send_packets_buffer.push_back(p);
    }
    m_output_threads_mutex.unlock();
    //дальнейшая обработка пакета происходит в функции tick()

    PacketMessage pm;
    {
        pm.ipPort       = remote_ip_port;
        pm.packet       = packet;
        pm.header.type  = type;
        //границы глобального пакета
        pm.range.start  = firstSn;
        pm.range.finish = lastSn;
    }
    m_sent_global_packets.push_back(pm);
}

void UDPSocket::tick() {
    checkConnections();

    sendAutoMsg();
    recvAutoMsg(1);

    PacketMessage pm = getOutPacket();
    if(!pm.packet.empty() && m_settings.getPacketCallback())
        m_settings.getPacketCallback()(pm);
    JsonMessage jm = getOutJson();
    if(!jm.json.isEmpty() && m_settings.getJsonCallback())
        m_settings.getJsonCallback()(jm);
}

void UDPSocket::checkConnections()
{
    Log(logs::eDEBUG, "checkConnections()");

    Json jPing;
    jPing.put("ping", this->getLocalIpPort().to_string());

    for(auto it = m_map_connections.begin(); it != m_map_connections.end(); it++) {
        Log(logs::eDEBUG, "checkConnections() #1");

        auto _now = std::chrono::system_clock::now();
        auto _inactivity = std::chrono::milliseconds(m_settings.getInactivityTimer());
        auto _halfInactivity = std::chrono::milliseconds(m_settings.getInactivityTimer() / 2);
        //если не было сообщений ОТ адреса дольше this->inactivityTimer/2, то отправить пинг
        if(it->second.m_last_ping_time + _halfInactivity < _now
            && it->second.m_last_activity + _halfInactivity < _now
            ) {
            Log(logs::eDEBUG, "send ping to " + it->first.to_string());
            Log(logs::eDEBUG, "expected time: " + logs::get_time_string(it->second.m_last_ping_time + _halfInactivity));
            sendFragments(it->first, eControlType, convert_to_packet(jPing.to_string(-1)));
            it->second.m_last_ping_time = std::chrono::system_clock::now();
            continue;
        }

        Log(logs::eDEBUG, "checkConnections() #2");
        //если долгое время не было сообщений от абонента, удалить все сообщения до него
        if(it->second.m_last_activity + _inactivity < _now) {
            Log(logs::eWARNING, "Connection " + it->first.to_string()
                                    + " removed, last activity at "
                                    + logs::get_time_string(it->second.m_last_activity));
            it = m_map_connections.erase(it);
        }

        Log(logs::eDEBUG, "checkConnections() #3");
        //если дошли до конца диапазона
        if(it == m_map_connections.end())
            break;

        Log(logs::eDEBUG, "checkConnections() #4");
    }
}

void UDPSocket::sendAutoMsg() {
    Log(logs::eDEBUG, "sendAutoMsg()");

    int counter = 0; //общий счётчик за проход функции
    m_output_threads_mutex.lock();

    //перепосылка недоставленных пакетов
    for(auto it = m_map_auto_sent_packets.begin();
         it != m_map_auto_sent_packets.end() && ((counter < m_settings.getMaxMsgsSentOnTick())
                                                  || (m_settings.getMaxMsgsSentOnTick() < 0));
         it++) {
        time_point_default tp = it->first;
        tp += std::chrono::milliseconds(m_settings.getInactivityTimer());
        PacketMessage pm = it->second;
        if(tp < std::chrono::system_clock::now()) { //нужно переотправить
            it = m_map_auto_sent_packets.erase(it);
            m_send_packets_buffer.push_front(pm);
            Log(logs::eDEBUG, "new try to send [" + std::to_string(it->second.sn.get()) + "] fragment");
            counter++;
        }
    }

    //постепенная отправка пакетов в сокет
    while(!m_send_packets_buffer.empty()
           && ((counter < m_settings.getMaxMsgsSentOnTick()) || (m_settings.getMaxMsgsSentOnTick() < 0))
           ) {
        PacketMessage pm = m_send_packets_buffer.front();
        m_send_packets_buffer.pop_front();
        Socket::sendRawMsg(pm); //отправили

        if(pm.header.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
            //запоминаем для ожидания ответа или досылки
            time_point_default current_time = std::chrono::system_clock::now();
            m_map_auto_sent_packets.insert(std::make_pair(current_time, pm));
        }

        counter++;
    }

    m_output_threads_mutex.unlock();
}

void UDPSocket::recvAutoMsg(int timeout) {
    Log(logs::eDEBUG, "recvAutoMsg()");

    PacketMessage pm = recvRawMsg(1);
    if(pm.packet.empty()) return;

    pm.header = unpackHeader(pm.packet[0]);
    uint8_t glob_sn = pm.packet[1]; //TODO: нужна защита от некорректного размера чтения!
    uint8_t sn      = pm.packet[2];
    pm.sn = EECounter(255);
    pm.sn.set_glob_pos(glob_sn);
    pm.sn.set_pos(sn);
    pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 3); //удалить первые три байта

    PacketMessage b_pm = buildPacket(pm);
    JsonMessage jm = b_pm;

    Json controlAcknowledgement;
    if(pm.header.type != eControlType) {
        Log(logs::eDEBUG, "Send acknowledge for message(" + std::to_string(pm.sn.get()) + ")");
        controlAcknowledgement.put("ack_sn", (double)pm.sn.get()); //TODO: общий тип для всех числовых значений
        if(b_pm.isBuiltComplete)
            controlAcknowledgement.put("ack_all_packet", (double)b_pm.sn.get());
        if(b_pm.isError) {
            controlAcknowledgement.put("packet_error_last_sn", (double)b_pm.error.sn_finish.get());//TODO: проверка ошибок и переотправка
        }
    }

    if(!b_pm.packet.empty()) {
        Log(b_pm.header.type != eControlType ? logs::eINFO : logs::eDEBUG,
            "Recv: "+ to_string(b_pm.header.type) + " ["
                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(b_pm.packet)
                                     : "Json:" + jm.json.to_string(-1))
                + "] <-- " + b_pm.ipPort.to_string());

        //обработка собранного пакета (1 за проход)
        if(pm.header.type == eControlType) {
            if(jm.json.contains("ack_sn")) {
                uint8_t sn = jm.json["ack_sn"].getNum();
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.sn.get() == sn) {
                        m_map_auto_sent_packets.erase(it->first);
                        break;
                    }
                }
            }
            if(jm.json.contains("ack_all_packet")) {
                uint8_t first_sn = jm.json["ack_all_packet"].getNum(); //номер первого фрагмента сообщения

                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->range.start.get() == first_sn) {
                        PacketMessage tempPM;
                        tempPM.packet = it->packet;
                        jm.clear();
                        jm = tempPM;
                        Log(it->header.type != eControlType ? logs::eINFO : logs::eDEBUG,
                            "message delivered ["
                                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(tempPM.packet)
                                                     : "Json:" + jm.json.to_string(-1))
                                + "]");
                        it = m_sent_global_packets.erase(it);
                        break;
                    }
                }
            }
            if(jm.json.contains("packet_error_last_sn")) {
                uint8_t last_err_sn = jm.json["packet_error_last_sn"].getNum(); //номер первого фрагмента сообщения

                //удалить все упоминания фрагментов пакета из очереди переотправок
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.sn <= last_err_sn)
                        it = m_map_auto_sent_packets.erase(it);
                }

                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                Packet packet;
                IpPort ipPort;
                PacketType type;
                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->range.finish.get() == last_err_sn) {
                        packet  = it->packet;
                        type    = it->header.type;
                        ipPort  = it->ipPort;
                        it = m_sent_global_packets.erase(it);
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
            m_input_threads_mutex.lock();
            if(jm.json.isEmpty())
                m_map_recv_packets_buffer.push_back(b_pm);
            else
                m_map_recv_jsons_buffer.push_back(jm);
            m_input_threads_mutex.unlock();
        }
    }

    if(!controlAcknowledgement.isEmpty())
        sendFragments(pm.ipPort, eControlType, convert_to_packet(controlAcknowledgement.to_string(-1)));
}


UDPSocket::UDPSocket(const IpPort &local_ip_port, SocketSettings settings) {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_ip_port.port, local_ip_port.ip);
}

UDPSocket::UDPSocket(uint16_t local_port, std::string local_ip, SocketSettings settings) {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_port, local_ip);
}

bool UDPSocket::sendRawMsg(const std::string &remote_ip, const uint16_t remote_port, const Packet &packet) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remote_port);
    if(!inet_pton(AF_INET, remote_ip.c_str(), &sock.sin_addr.s_addr))
        Log(logs::eERROR, "inet_pton(): return ERROR");
    int res = sendto(m_socket_fd, (char*)packet.data(), packet.size(), 0,
                     (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
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
    FD_SET(m_socket_fd, &fds);
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
    recv_num = select(m_socket_fd + 1, &fds, NULL, NULL, (timeout > 0 ? &t : NULL));
    if(recv_num < 0) {
        if(errno != EINTR) /* Interrupted system call */
            Log(logs::eERROR, "Error in select(), errno=" + std::to_string(errno));
        return PacketMessage();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(m_socket_fd, buf,
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

        Log(logs::eDEBUG, std::string("sendRaw ")
                              + "(" + std::to_string(rpacket.packet.size()) + ")"
                              + "[0x" + utils::to_hex_string(rpacket.packet) + "]"
                              + " from " + rpacket.ipPort.to_string());

        return rpacket;
    }

    return {};
}

void UDPSocket::startServer() {
    if(!isServerActive())
        open(m_local_port, m_local_ip);
}

void UDPSocket::stopServer() {
    if(isServerActive())
        close();
}

void UDPSocket::open(const uint16_t local_port, const std::string& local_ip) {
    m_local_ip      = local_ip;
    m_local_port    = local_port;

    // create
    m_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket_fd < 0)
        perror("socket() failed");

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(local_port);
    if(local_ip.empty())
        sock.sin_addr.s_addr = INADDR_ANY;
    else {
        if(!inet_pton(AF_INET, local_ip.c_str(), &sock.sin_addr.s_addr))
            Log(logs::eERROR, "inet_pton(): return ERROR");
    }
    int res = bind(m_socket_fd, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0) {
        perror(std::string("bind() failed with localIP(" + local_ip + ")"
                           + ", port(" + std::to_string(local_port) + ")").c_str());
        close();
        return;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr.s_addr), str, INET_ADDRSTRLEN);
    Log(logs::eINFO, "Socket binded at " + IpPort{str, local_port}.to_string());
}

bool UDPSocket::sendMsg(const IpPort& remote_ip_port, const Packet& packet) {
    sendFragments(remote_ip_port, eDataType, packet);
    return true;
}

bool UDPSocket::sendMsg(const IpPort& remote_ip_port, const Json& json) {
    //отправит Json в текстовом формате без пробелов
    sendFragments(remote_ip_port, eDataType, convert_to_packet(json.to_string(-1)));
    return true;
}

PacketMessage UDPSocket::getOutPacket()
{
    m_input_threads_mutex.lock();
    PacketMessage pm;
    if(m_map_recv_packets_buffer.size() > 0) {
        pm = m_map_recv_packets_buffer.front();
        m_map_recv_packets_buffer.pop_front();
    }
    m_input_threads_mutex.unlock();

    return pm;
}

JsonMessage UDPSocket::getOutJson()
{
    m_input_threads_mutex.lock();
    JsonMessage jm;
    if(m_map_recv_jsons_buffer.size() > 0) {
        jm = m_map_recv_jsons_buffer.front();
        m_map_recv_jsons_buffer.pop_front();
    }
    m_input_threads_mutex.unlock();

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


