#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#define FULL_MSG_COLOR          {logs::COLOR::eYELLOW_BG, logs::COLOR::eBLACK_FG, logs::COLOR::eBOLD_TEXT}
#define GLOBAL_APPEND_MSG_COLOR {logs::COLOR::eBRIGHT_GRAY_BG, logs::COLOR::eBRIGHT_RED_FG}
#define CRITICAL_MSG_COLOR      {logs::eRED_BG, logs::eWHITE_FG}
#define OUTPUT_MSG_COLOR        {logs::eBLUE_BG, logs::eWHITE_FG}
#define INPUT_MSG_COLOR         {logs::eCYAN_BG, logs::eWHITE_FG}

bool Socket::checkCorrectIp(const std::string& ip_string) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ip_string.c_str(), &sock.sin_addr.s_addr)) {
        log(logs::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
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
        log(logs::eDEBUG,
            "add connection(1): " + ip_port.to_string()
                + ", map_size:" + std::to_string(m_map_connections.size()),
            logs::to_color_string(logs::eBRIGHT_GREEN_BG, "add connection(1): " + ip_port.to_string())
                + ", " + "map_size:" + std::to_string(m_map_connections.size()));
    }

    return it->second.m_out_sn;
}

void Socket::appendNewFragment(const PacketMessage& received_pm)
{
    auto it = m_map_connections.find(received_pm.ipPort);
    if(it == m_map_connections.end()) {
        it = m_map_connections.insert(std::make_pair(received_pm.ipPort, Connection())).first;
        log(logs::eDEBUG,
            "add connection(1): " + received_pm.ipPort.to_string()
                + ", map_size:" + std::to_string(m_map_connections.size()),
            logs::to_color_string(logs::eBRIGHT_GREEN_BG, "add connection(1): " + received_pm.ipPort.to_string())
                + ", " + "map_size:" + std::to_string(m_map_connections.size()));
    }
    it->second.m_last_input_activity = std::chrono::system_clock::now();

    log(logs::eDEBUG3, "buildPacket(), mapConnection size: " + std::to_string(m_map_connections.size()));

    if(received_pm.packet.empty()) {
        log(logs::eDEBUG3, "~buildPacket(), packet empty");
        return;
    }

    log(logs::eDEBUG,
        "received sn=" + std::to_string(received_pm.sn.get())
                          + ", expected_sn=" + std::to_string(it->second.m_in_next_sn.get()),
        logs::to_color_string(logs::eBLUE_FG, "received sn=" + std::to_string(received_pm.sn.get())
         + ", expected_sn=" + std::to_string(it->second.m_in_next_sn.get())));

    if(it->second.m_in_sn_last_recv < received_pm.sn)
        it->second.m_in_sn_last_recv = received_pm.sn;
    //всё, что пришло до этого - удалится
    EECounter rmSn = it->second.m_in_sn_last_recv - (it->second.m_in_sn_last_recv.size() / 2); //размер окна - половина диапазонаe

    if(received_pm.sn == it->second.m_in_next_sn) {
        //NOTE: игнор уже пришедших фрагментов произойдёт здесь же
        it->second.m_map_recv_builded_messages.insert(std::make_pair(received_pm.sn, received_pm));
        it->second.m_in_next_sn++;
        log(logs::eDEBUG, "processing build...");
    } else if (received_pm.sn < it->second.m_in_next_sn) {
        log(logs::eDEBUG,
            "IGNORING, fragment has already been received!",
            logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "IGNORING") + ", fragment has already been received!");

        //если имеющийся пакет с таким SN отличается по содержанию, то необходимо обновить мапу
        auto it_fragment = it->second.m_map_recv_fragments.find(received_pm.sn);
        if(it_fragment != it->second.m_map_recv_fragments.end()) {
            if(it_fragment->second.packet != received_pm.packet) {
                log(logs::eDEBUG,
                    "This fragment is different from the existing one, updating map...",
                    logs::to_color_string(logs::eBRIGHT_YELLOW_BG,
                                          "This fragment is different from the existing one, updating map..."));
                it->second.m_map_recv_fragments.erase(it_fragment);
                it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.sn, received_pm));
            }
        }
    } else {
        log(logs::eDEBUG, "fragment received, but will be processed later");
        it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.sn, received_pm));
    }

    //пройтись по poolRecvMessages и добрать по порядку к mapRecvBuildedMessages
    auto it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn);
    while(it_pool != it->second.m_map_recv_fragments.end()) {
        it->second.m_map_recv_builded_messages.insert(std::make_pair(it_pool->second.sn, it_pool->second));
        it->second.m_in_next_sn++;

        it_pool = it->second.m_map_recv_fragments.erase(it_pool);
        log(logs::eDEBUG, "buildPacket(), new expected_sn:" + std::to_string(it->second.m_in_next_sn.get()));
        it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn); //ищем следующий фрагмент очереди
    }

    //удалить всё, что теперь вне окна ожидания
    if(it->second.m_in_sn_last_recv.get() > (it->second.m_in_sn_last_recv.size() / 2)
        || it->second.m_in_sn_last_recv.get_glob() > 0
        ) {
        log(logs::eDEBUG,
            "buildPacket(), rmSn=" + std::to_string(rmSn.get()),
            logs::to_color_string(logs::eBRIGHT_YELLOW_BG, "buildPacket(), rmSn=" + std::to_string(rmSn.get())));
        it_pool = it->second.m_map_recv_fragments.begin();
        while(it_pool != it->second.m_map_recv_fragments.end()) {
            if(it_pool->first < rmSn) {
                log(logs::eDEBUG,
                    "buildPacket(), remove wait sn=" + std::to_string(it->second.m_in_next_sn.get()),
                    logs::to_color_string(logs::COLOR::eBRIGHT_YELLOW_BG,
                                          "buildPacket(), remove wait sn="
                                              + std::to_string(it->second.m_in_next_sn.get())));

                it_pool = it->second.m_map_recv_fragments.erase(it_pool);
                if(it_pool == it->second.m_map_recv_fragments.end()) break;
            }
            it_pool++;
        }
    }
}

PacketMessage Socket::buildPacket(MapConnectionsIterator& it) {
    log(logs::eDEBUG3, "buildPacket(), prepare to build");
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

        //всё, что пришло до этого - удалится
        EECounter rmSn = it->second.m_in_sn_last_recv - (it->second.m_in_sn_last_recv.size() / 2); //размер окна - половина диапазонаe
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

            log(logs::eDEBUG3, "~buildPacket(1), mapConnection size: " + std::to_string(m_map_connections.size()));
            return pm;
        }

        //пакет соберётся, копируем в выходной PM.packet
        if(isStarted && isFinished) {
            pm.sn = firstCounter; //номер первого фрагмента для индикации доставки глобального сообщения
            log(logs::eDEBUG, "built recv packet: sn=" + std::to_string(pm.sn.get()) + ", type=" + to_string(pm.header.type));
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

                log(logs::eDEBUG3, "~buildPacket(2), mapConnection size: " + std::to_string(m_map_connections.size()));
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

    log(logs::eDEBUG3, "~buildPacket(3), mapConnection size: " + std::to_string(m_map_connections.size()));
    return pm;
}

void Socket::updateLastOutputActivityTime(const IpPort& remote_ip_port) {
    log(logs::eDEBUG, "updateLastOutputActivityTime " + remote_ip_port.to_string());

    auto it = m_map_connections.find(remote_ip_port);
    if(it == m_map_connections.end()) {
        it = m_map_connections.insert(std::make_pair(remote_ip_port, Connection())).first;
        log(logs::eDEBUG,
            "add connection(1): " + remote_ip_port.to_string()
                + ", map_size:" + std::to_string(m_map_connections.size()),
            logs::to_color_string(logs::eBRIGHT_GREEN_BG, "add connection(1): " + remote_ip_port.to_string())
                + ", " + "map_size:" + std::to_string(m_map_connections.size()));
    }

    it->second.m_last_output_activity = std::chrono::system_clock::now();
}

void Socket::log(const logs::LEVEL level, const std::string log_message, const std::string color_log_message)
{
    LoggerSettings::LogCallback currentCallback = nullptr;
    LoggerSettings::LogCallback currentColorCallback = nullptr;
    std::string levelSubstring = "";

    std::string timeString = "";
    std::vector<logs::COLOR> timeColor = {};
    if(m_settings.isLogTimeEnabled()) {
        timeString = logs::get_time_string() + " ";
        if(timeString != m_last_time_string) {
            m_time_color_flag = !m_time_color_flag;
            m_last_time_string = timeString;
        }
        if(m_time_color_flag)   timeColor = {logs::eWHITE_BG, logs::eGREEN_FG};
        else                    timeColor = {logs::eWHITE_BG, logs::eBLACK_FG};
    }

    if(level <= m_settings.getLogLevel()) {
        switch(level) {
        case logs::eWARNING:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".w";
            break;
        case logs::eINFO:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".i";
            break;
        case logs::eDEBUG:
        case logs::eDEBUG2:
        case logs::eDEBUG3:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".d";
            break;
        case logs::eERROR:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".e";
        default:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".unknown";
            break;
        }
    }

    //обычный вывод
    if(currentCallback)
        currentCallback(
            timeString
            + logs::columned(std::string("[") + to_string(m_socket_type)
                                     + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                     + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " "
            + log_message
            + "\n");
    //цветной вывод
    if(currentColorCallback)
        currentColorCallback(
            logs::to_color_string(timeColor, timeString)
            + logs::columned(level, std::string("[") + to_string(m_socket_type)
                                        + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                        + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " "
            + (color_log_message.empty() ? log_message : color_log_message)
            + "\n");
}

bool Socket::sendRawMsg(const PacketMessage &packet_message) {
    log(logs::eDEBUG2, std::string("sendRaw ")
                          + "(" + std::to_string(packet_message.packet.size()) + ")"
                          + "[0x" + utils::to_hex_string(packet_message.packet) + "] "
                          + packet_message.ipPort.to_string("to"));
    return sendRawMsg(packet_message.ipPort.ip, packet_message.ipPort.port, packet_message.packet);
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
        log(logs::eWARNING, "The socket " + IpPort{m_local_ip, m_local_port}.to_string() + " has been freed");
        m_socket_fd = -1;
    }
}

void UDPSocket::sendFragments(const IpPort &remote_ip_port, const PacketType type,
                              const Packet &packet, const bool need_ack) {
    Json json;
    json.parseJson(convert_from_packet(packet));

    log(type != eControlType ? logs::eINFO : logs::eDEBUG, //TODO: придумать более логичное решение log()
        "Send: " + to_string(type) + " "
            + (json.isEmpty() ? "[Data:0x" + utils::to_hex_string(packet) + "]"
                              : "[Json:" + json.to_string(-1) + "]"
                                    + " / [Data:" + "0x" + utils::to_hex_string(packet) + "]"
               ) + " "
            + remote_ip_port.to_string("to"),
        "Send: " + to_string(type) + " "
            + (json.isEmpty() ? "[Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::to_hex_string(packet)) + "]"
                              : "[Json:" + logs::to_color_string(FULL_MSG_COLOR, json.to_string(-1)) + "]"
                                    + " / [Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::to_hex_string(packet)) + "]"
               ) + " "
            + remote_ip_port.to_string("to"));

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
    if(need_ack) {
        m_sent_global_packets.push_back(pm);

        JsonMessage jm = pm;
        std::string appendString = "";
        appendString = "to map_global_packets, size: " + std::to_string(m_sent_global_packets.size());
        log(logs::eDEBUG,
            "append ["
                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(pm.packet)
                                     : "Json:" + jm.json.to_string(-1))
                + "] " + appendString);//,
//            logs::to_color_string(GLOBAL_APPEND_MSG_COLOR, "append ["
//                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(pm.packet)
//                                     : "Json:" + jm.json.to_string(-1))
//                + "] " + appendString));
    }
}

void UDPSocket::tick() {
    std::map<IpPort, Json> acknowledgeMap;
    checkConnections();

    //отправка фрагментов
    sendAutoMsg();

    //приём фрагментов
    Json jRecv = recvAutoMsg(1);

    //сборка пакетов для всех активных соединений
    for(auto it = m_map_connections.begin(); it != m_map_connections.end(); it++) {
        PacketMessage b_pm = buildPacket(it);

        Json jProc = processingBuiltPacket(b_pm);
        jProc.put(jRecv);

        auto it_json = acknowledgeMap.find(it->first);
        if(it_json != acknowledgeMap.end())
            jProc.put(it_json->second);

        if(!jProc.isEmpty()) {
            log(logs::eDEBUG2, "send ack: " + jProc.to_string(-1) + b_pm.ipPort.to_string("to"));
            sendFragments(it->first, eControlType, convert_to_packet(jProc.to_string(-1)), false);
        }
    }

    //выдача собранных пакетов пользователю API
    PacketMessage pm = getOutPacket();
    if(!pm.packet.empty() && m_settings.getPacketCallback())
        m_settings.getPacketCallback()(pm);
    JsonMessage jm = getOutJson();
    if(!jm.json.isEmpty() && m_settings.getJsonCallback())
        m_settings.getJsonCallback()(jm);
}

void UDPSocket::checkConnections()
{
    log(logs::eDEBUG2, "checkConnections()");

    Json jPing;
    jPing.put("ping", this->getLocalIpPort().to_string());

    //перепосылка недоставленных глобальных пакетов ==================================
    struct prepPacket {
        Packet packet;
        PacketType type;
        IpPort ipPort;
    };
    std::vector<prepPacket> packetsForSend;
    //--------------------------------------------------------------------------------

    for(auto it = m_map_connections.begin(); it != m_map_connections.end(); it++) {

        auto _now = std::chrono::system_clock::now();
        auto _inactivity = std::chrono::milliseconds(m_settings.getInactivityTimer());
        auto _halfInactivity = std::chrono::milliseconds(m_settings.getInactivityTimer() / 2);
        //если не было сообщений ОТ адреса дольше this->inactivityTimer/2, то отправить пинг
        log(logs::eDEBUG3, "checkConnections(), pings");
        if(it->second.m_last_output_activity + _halfInactivity < _now
            && it->second.m_last_input_activity + _halfInactivity < _now
            ) {
            log(logs::eDEBUG2, "Send ping to " + it->first.to_string());
            log(logs::eDEBUG3, "Expected time: " + logs::get_time_string(it->second.m_last_output_activity + _halfInactivity));
            sendFragments(it->first, eControlType, convert_to_packet(jPing.to_string(-1)), false);
            it->second.m_last_output_activity = std::chrono::system_clock::now();
            continue;
        }

        IpPort _currentIpPort = it->first;
        //если долгое время не было сообщений от абонента, удалить все сообщения до него
        log(logs::eDEBUG3, "checkConnections(), bad connection");
        if(it->second.m_last_input_activity + _inactivity < _now) {
            log(logs::eWARNING,
                "Connection " + _currentIpPort.to_string()
                    + " removed, last activity at "
                    + logs::get_time_string(it->second.m_last_input_activity),
                logs::to_color_string(CRITICAL_MSG_COLOR, "Connection " + _currentIpPort.to_string()
                 + " removed, last activity at "
                 + logs::get_time_string(it->second.m_last_input_activity)));
            it = m_map_connections.erase(it);

            //удаление всех фрагментов, которые находятся в очереди отправки, с совпадающим адресатом
            log(logs::eDEBUG3, "checkConnections(), removing fragments");
            for(auto it2 = m_map_auto_sent_packets.begin(); it2 != m_map_auto_sent_packets.end(); it2++) {
                if(it2->second.ipPort == it->first)
                    it2 = m_map_auto_sent_packets.erase(it2);

                //если дошли до конца диапазона
                if(it2 == m_map_auto_sent_packets.end()) break;
            }

            //перепосылка недоставленных глобальных пакетов ==================================
            log(logs::eDEBUG3, "checkConnections(), prepare to resend global packets");
            for(auto it_global_packet = m_sent_global_packets.begin();
                 it_global_packet != m_sent_global_packets.end(); it_global_packet++
                 ) {
                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                IpPort ipPort = it_global_packet->ipPort;
                if(ipPort == _currentIpPort) {
                    packetsForSend.push_back({it_global_packet->packet, it_global_packet->header.type, ipPort});

                    it_global_packet = m_sent_global_packets.erase(it_global_packet);

                    if(it_global_packet == m_sent_global_packets.end()) break;
                }
            }

            //если дошли до конца диапазона
            if(it == m_map_connections.end()) break;
        }

    }

    //перепосылка недоставленных глобальных пакетов ==================================
    log(logs::eDEBUG3, "checkConnections(), send found prepared packets");
    for(const prepPacket& current : packetsForSend) {
        if(!current.packet.empty()) {
            log(logs::eDEBUG, "New try to send packet [0x" + utils::to_hex_string(current.packet) + "]");
            sendFragments(current.ipPort, current.type, current.packet); //переотправка
        }
    }
    //================================================================================
}

void UDPSocket::sendAutoMsg() {
    log(logs::eDEBUG2, "sendAutoMsg()");

    int counter = 0; //общий счётчик за проход функции
    m_output_threads_mutex.lock();

    //перепосылка недоставленных пакетов =============================================
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
            log(logs::eDEBUG, "New try to send [" + std::to_string(it->second.sn.get()) + "] fragment");

            counter++;
            if(it == m_map_auto_sent_packets.end()) break;
        }
    }
    //================================================================================


    //постепенная отправка пакетов в сокет ===========================================
    while(!m_send_packets_buffer.empty()
           && ((counter < m_settings.getMaxMsgsSentOnTick()) || (m_settings.getMaxMsgsSentOnTick() < 0))
           ) {
        PacketMessage pm = m_send_packets_buffer.front();
        m_send_packets_buffer.pop_front();
        log(logs::eDEBUG,
            "Sending [" + std::to_string(pm.sn.get()) + "] sn fragment, data:[0x"
                + utils::to_hex_string(pm.packet) + "] " + pm.ipPort.to_string("to"),
            logs::to_color_string(OUTPUT_MSG_COLOR,"Sending")
                + " [" + std::to_string(pm.sn.get()) + "] sn fragment, data:[0x"
                + utils::to_hex_string(pm.packet) + "] " + pm.ipPort.to_string("to"));

        Socket::sendRawMsg(pm); //отправили
        updateLastOutputActivityTime(pm.ipPort);

        if(pm.header.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
            //запоминаем для ожидания ответа или досылки
            time_point_default current_time = std::chrono::system_clock::now();
            m_map_auto_sent_packets.insert(std::make_pair(current_time, pm));
        }

        counter++;
    }
    //================================================================================

    m_output_threads_mutex.unlock();
}

Json UDPSocket::recvAutoMsg(int timeout) {
    log(logs::eDEBUG2, "recvAutoMsg()");

    PacketMessage pm = recvRawMsg(1);
    if(pm.packet.empty()) return {};

    pm.header = unpackHeader(pm.packet[0]);
    uint8_t glob_sn = pm.packet[1]; //TODO: нужна защита от некорректного размера чтения!
    uint8_t sn      = pm.packet[2];
    pm.sn = EECounter(255);
    pm.sn.set_glob_pos(glob_sn);
    pm.sn.set_pos(sn);
    pm.packet.erase(pm.packet.begin(), pm.packet.begin() + 3); //удалить первые три байта

    log(logs::eDEBUG,
        "Received [" + std::to_string(sn) + "] sn fragment of type "
            + to_string(pm.header.type)
            + ", data:[0x"
            + utils::to_hex_string(pm.packet) + "] " + pm.ipPort.to_string("from"),
        logs::to_color_string(INPUT_MSG_COLOR, "Received")
            + " [" + std::to_string(sn) + "] sn fragment of type "
            + to_string(pm.header.type)
            + ", data:[0x"
            + utils::to_hex_string(pm.packet) + "] " + pm.ipPort.to_string("from"));

    Json outputJson;
    auto it = m_map_connections.find(pm.ipPort);
    if(it == m_map_connections.end() && pm.header.type == eControlType) {
        //если первый пакет от адресата является контрольным и НЕ требует отчёта о доставке
        log(logs::eDEBUG,
            "Send initial ping for message sn=" + std::to_string(pm.sn.get())
                + " " + pm.ipPort.to_string(),
            "Send initial ping for message sn=" + std::to_string(pm.sn.get())
                + " " + logs::to_color_string(logs::eRED_BG, pm.ipPort.to_string()));
        outputJson.put("ping", this->getLocalIpPort().to_string());
    }

    if(pm.header.type != eControlType)
        outputJson.put("ack_sn", (double)pm.sn.get());

    appendNewFragment(pm);

    return outputJson;
}

Json UDPSocket::processingBuiltPacket(const PacketMessage &pm) {
    JsonMessage jm = pm;

    if(!pm.packet.empty()) {
        log(pm.header.type != eControlType ? logs::eINFO : logs::eDEBUG,
            "Built packet: "+ to_string(pm.header.type) + " ["
                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(pm.packet)
                                     : "Json:" + jm.json.to_string(-1))
                + "] " + pm.ipPort.to_string("from"),
            logs::to_color_string(INPUT_MSG_COLOR, "Built packet") + ": "+ to_string(pm.header.type) + " ["
                + (jm.json.isEmpty() ? "Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::to_hex_string(pm.packet))
                                     : "Json:" + logs::to_color_string(FULL_MSG_COLOR, jm.json.to_string(-1)))
                + "] " + pm.ipPort.to_string("from"));

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
                        std::string appendString = "";
                        appendString = ", map_global_packets size: " + std::to_string(m_sent_global_packets.size());
                        log(it->header.type != eControlType ? logs::eINFO : logs::eDEBUG,
                            "Message delivered ["
                                + (jm.json.isEmpty() ? "Data:0x" + utils::to_hex_string(tempPM.packet)
                                                     : "Json:" + jm.json.to_string(-1))
                                + "]" + appendString,
                            "Message delivered ["
                                + (jm.json.isEmpty() ? "Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::to_hex_string(tempPM.packet))
                                                     : "Json:" + logs::to_color_string(FULL_MSG_COLOR, jm.json.to_string(-1)))
                                + "]" + logs::to_color_string({logs::COLOR::eBLACK_BG, logs::COLOR::eRED_FG}, appendString));
                        it = m_sent_global_packets.erase(it);
                        break;
                    }
                }
            }
            if(jm.json.contains("packet_error_last_sn")) {
                uint8_t last_err_sn = jm.json["packet_error_last_sn"].getNum(); //номер первого фрагмента сообщения

                //удалить все упоминания фрагментов пакета из очереди переотправок
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.sn <= last_err_sn) {
                        it = m_map_auto_sent_packets.erase(it);
                        if(it == m_map_auto_sent_packets.end()) break;
                    }
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
                    log(logs::eDEBUG, "New try to send packet [0x" + utils::to_hex_string(packet) + "]");
                    sendFragments(ipPort, type, packet); //переотправка
                }
            }
        } else {
            m_input_threads_mutex.lock();
            if(jm.json.isEmpty()) {
                log(logs::eDEBUG,
                    "insert packet " + to_string(pm.header.type) + " to storage",
                    logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "insert packet " + to_string(pm.header.type) + " to storage"));
                m_map_recv_packets_buffer.push_back(pm);
            } else {
                log(logs::eDEBUG,
                    "insert packet " + to_string(pm.header.type) + " to storage",
                    logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "insert packet " + to_string(pm.header.type) + " to storage"));
                m_map_recv_jsons_buffer.push_back(jm);
            }
            m_input_threads_mutex.unlock();
        }
    }

    //=========================================================================================
    Json controlAcknowledgement;
    if(pm.header.type != eControlType) {
        if(pm.isBuiltComplete)
            controlAcknowledgement.put("ack_all_packet", (double)pm.sn.get()); //TODO: общий тип для всех числовых значений JSON
        if(pm.isError)
            controlAcknowledgement.put("packet_error_last_sn", (double)pm.error.sn_finish.get());//TODO: проверка ошибок и переотправка
    }
    //=========================================================================================
    return controlAcknowledgement;
}


UDPSocket::UDPSocket(const IpPort &local_ip_port, const SocketSettings& settings) {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_ip_port.port, local_ip_port.ip);
}

UDPSocket::UDPSocket(const uint16_t local_port, const std::string& local_ip, const SocketSettings& settings) {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_port, local_ip);
}

bool UDPSocket::sendRawMsg(const std::string &remote_ip, const uint16_t remote_port, const Packet &packet) {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remote_port);
    if(!inet_pton(AF_INET, remote_ip.c_str(), &sock.sin_addr.s_addr))
        log(logs::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
    int res = sendto(m_socket_fd, (char*)packet.data(), packet.size(), 0,
                     (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
    if(res < 0) {
        log(logs::eERROR, "sendTo() failed, error(" + std::to_string(errno) + ")");
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
            log(logs::eERROR, "select() failed, error(" + std::to_string(errno) + ")");
        return PacketMessage();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(m_socket_fd, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
    }

    if(recv_num < 0) {
        log(logs::eERROR, "recvfrom() failed, error(" + std::to_string(errno) + ")");
    } else if(recv_num > 0) {
        PacketMessage rpacket;
//        rpacket.ipPort.ip.resize(INET_ADDRSTRLEN);
        char rpacket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket_ip, INET_ADDRSTRLEN);

        rpacket.ipPort.ip = rpacket_ip;
        rpacket.ipPort.port = ntohs(sock.sin_port);
        rpacket.packet = Packet(buf, buf + recv_num);

        log(logs::eDEBUG2, std::string("recvRaw ")
                              + "(" + std::to_string(rpacket.packet.size()) + ")"
                              + "[0x" + utils::to_hex_string(rpacket.packet) + "] "
                              + rpacket.ipPort.to_string("from"));

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
        log(logs::eERROR, "socket() failed, error(" + std::to_string(errno) + ")");

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(local_port);
    if(local_ip.empty())
        sock.sin_addr.s_addr = INADDR_ANY;
    else {
        if(!inet_pton(AF_INET, local_ip.c_str(), &sock.sin_addr.s_addr))
            log(logs::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
    }
    int res = bind(m_socket_fd, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0) {
        log(logs::eERROR, "bind() failed with localIP(" + local_ip + ")"
                              + ", port(" + std::to_string(local_port) + "), error("
                              + std::to_string(errno) + ")");
        close();
        return;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr.s_addr), str, INET_ADDRSTRLEN);
    log(logs::eWARNING, "Socket binded at " + IpPort{str, local_port}.to_string());
}

bool UDPSocket::isConnected(const IpPort &remote_ip_port)
{
    auto it = m_map_connections.find(remote_ip_port);
    if(it != m_map_connections.end())   return true;
    else                                return false;
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


