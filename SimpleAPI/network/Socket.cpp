#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include "../utils/Utils.h"


#define FULL_MSG_COLOR          {logs::COLOR::eYELLOW_BG, logs::COLOR::eBLACK_FG, logs::COLOR::eBOLD_TEXT}
#define GLOBAL_APPEND_MSG_COLOR {logs::COLOR::eBRIGHT_GRAY_BG, logs::COLOR::eBRIGHT_RED_FG}
#define CRITICAL_MSG_COLOR      {logs::eRED_BG, logs::eWHITE_FG}
#define OUTPUT_MSG_COLOR        {logs::eBLUE_BG, logs::eWHITE_FG}
#define INPUT_MSG_COLOR         {logs::eCYAN_BG, logs::eWHITE_FG}

bool Socket::checkCorrectIp(const std::string& ip_string) noexcept {
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ip_string.c_str(), &sock.sin_addr.s_addr)) {
        log(logs::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
        return false;
    }
    return true;
}

uint8_t Socket::packHeader(const PacketHeader& pm) noexcept {
    return (pm.type << 7)
           | (pm.version << 5)
           | (pm.isFirstFragment << 4)
           | (pm.isLastFragment << 3)
           | (pm.isChip << 2)
           | pm.crcLevel;
}

PacketHeader Socket::unpackHeader(uint8_t header) noexcept {
    PacketHeader ph;
    ph.type             = (PacketType)(header >> 7); //1
    ph.version          = (ApiVersion)((header >> 5) & 0x3); //2
    ph.isFirstFragment  = (header >> 4) & 0x1; //1
    ph.isLastFragment   = (header >> 3) & 0x1; //1
    ph.isChip           = (header >> 2) & 0x1; //1
    ph.crcLevel         = (CRC)(header & 0x3); //2

    return ph;
}

EECounter& Socket::getOutSeqNumber(const IpPort& ip_port) noexcept {
    auto it = findOrCreateConnection(ip_port);

    return it->second.m_out_sn;
}

void Socket::appendNewFragment(const PacketMessage& received_pm) noexcept {
    auto it = findOrCreateConnection(received_pm.m_ip_port);

    it->second.m_last_input_activity = std::chrono::system_clock::now();

    log(logs::eDEBUG3, "buildPacket(), mapConnection size: " + std::to_string(m_map_connections.size()));

    if(received_pm.m_packet.empty()) {
        log(logs::eDEBUG3, "~buildPacket(), packet empty");
        return;
    }

    log(logs::eDEBUG,
        "received sn=" + std::to_string(received_pm.m_sn.get())
                          + ", expected_sn=" + std::to_string(it->second.m_in_next_sn.get()),
        logs::to_color_string(logs::eBLUE_FG, "received sn=" + std::to_string(received_pm.m_sn.get())
         + ", expected_sn=" + std::to_string(it->second.m_in_next_sn.get())));

    if(it->second.m_in_sn_last_recv < received_pm.m_sn)
        it->second.m_in_sn_last_recv = received_pm.m_sn;
    //всё, что пришло до этого - удалится
    EECounter rmSn = it->second.m_in_sn_last_recv - (it->second.m_in_sn_last_recv.size() / 2); //размер окна - половина диапазонаe

    if(received_pm.m_sn == it->second.m_in_next_sn) {
        //NOTE: игнор уже пришедших фрагментов произойдёт здесь же
        it->second.m_map_recv_builded_messages.insert(std::make_pair(received_pm.m_sn, received_pm));
        it->second.m_in_next_sn++;
        log(logs::eDEBUG, "processing build...");
    } else if (received_pm.m_sn < it->second.m_in_next_sn) {
        log(logs::eDEBUG,
            "IGNORING, fragment has already been received!",
            logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "IGNORING") + ", fragment has already been received!");

        //если имеющийся пакет с таким SN отличается по содержанию, то необходимо обновить мапу
        auto it_fragment = it->second.m_map_recv_fragments.find(received_pm.m_sn);
        if(it_fragment != it->second.m_map_recv_fragments.end()) {
            if(it_fragment->second.m_packet != received_pm.m_packet) {
                log(logs::eDEBUG,
                    "This fragment is different from the existing one, updating map...",
                    logs::to_color_string(logs::eBRIGHT_YELLOW_BG,
                                          "This fragment is different from the existing one, updating map..."));
                it->second.m_map_recv_fragments.erase(it_fragment);
                it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.m_sn, received_pm));
            }
        }
    } else {
        log(logs::eDEBUG, "fragment received, but will be processed later");
        it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.m_sn, received_pm));
    }

    //пройтись по poolRecvMessages и добрать по порядку к mapRecvBuildedMessages
    auto it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn);
    while(it_pool != it->second.m_map_recv_fragments.end()) {
        it->second.m_map_recv_builded_messages.insert(std::make_pair(it_pool->second.m_sn, it_pool->second));
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

PacketMessage Socket::buildPacket(MapConnectionsIterator& it) noexcept {
    log(logs::eDEBUG3, "buildPacket(), prepare to build");
    //попытаться собрать ОДИН пакет
    PacketMessage pm;
    pm.m_is_built_complete  = false;
    pm.m_is_error           = false;

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
                log(logs::eDEBUG, "first sn: " + std::to_string(firstCounter.get()));
                isFirstCounterSet = true;
                isStarted = true;

                pm.m_ip_port = it_build->second.m_ip_port;
                pm.m_header = it_build->second.m_header;
            }

            if(it_build->second.m_header.isLastFragment) {
                lastCounter = it_build->first;
                log(logs::eDEBUG, "last sn: " + std::to_string(lastCounter.get()));
                isFinished = true;
                break;
            }
        }

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
            pm.m_is_error = true;
            pm.m_error.sn_finish = lastCounter;

            log(logs::eDEBUG3, "~buildPacket(1), mapConnection size: " + std::to_string(m_map_connections.size()));
            return pm;
        }

        //пакет соберётся, копируем в выходной PM.packet
        if(isStarted && isFinished) {
            for(const auto& _it : it->second.m_map_recv_builded_messages) {
                log(logs::eDEBUG2,
                    "MAP: current fragment: [0x" + utils::ToHexString(_it.second.m_packet) + "]",
                    logs::to_color_string(logs::eYELLOW_FG, "MAP: current fragment: [0x" + utils::ToHexString(_it.second.m_packet) + "]"));
            }

            pm.m_sn = firstCounter; //номер первого фрагмента для индикации доставки глобального сообщения
            log(logs::eDEBUG, "built recv packet: sn=" + std::to_string(pm.m_sn.get()) + ", type=" + ToString(pm.m_header.type));
            //скопировать и удалить задействованные фрагменты
            bool this_last_fragment = false;
            EECounter counter = firstCounter;
            for(auto it_build = it->second.m_map_recv_builded_messages.find(counter++);
                 it_build != it->second.m_map_recv_builded_messages.end();
                 it_build = it->second.m_map_recv_builded_messages.find(counter++)
                 ) {

                if(it_build->second.m_sn == lastCounter) {
                    this_last_fragment = true;
                    log(logs::eDEBUG, "this fragment is last");
                }

                log(logs::eDEBUG2, "current fragment(sn:" + std::to_string(it_build->first.get())
                                      + "): [0x" + utils::ToHexString(it_build->second.m_packet) + "]");
                std::copy(std::begin(it_build->second.m_packet),
                          std::end(it_build->second.m_packet),
                          std::back_insert_iterator<Packet>(pm.m_packet));

                it_build = it->second.m_map_recv_builded_messages.erase(it_build);

                if(this_last_fragment) break;
            }
            log(logs::eDEBUG, "result packet: [0x" + utils::ToHexString(pm.m_packet) + "]");

            //дешифрация
            dechiphering(pm.m_packet);
            //проверка контрольной суммы
            log(logs::eDEBUG2, "before CRC: " + utils::ToHexString(pm.m_packet));
            switch(pm.m_header.crcLevel) {
            case eCRC_8:
                pm.m_is_error = !utils::CheckCrc8(pm.m_packet);
                pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 1);
                break;
            case eCRC_16:
                pm.m_is_error = !utils::CheckCrc16(pm.m_packet);
                pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 2);
                break;
            case eCRC_32:
                pm.m_is_error = !utils::CheckCrc32(pm.m_packet);
                pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 4);
                break;
            default:        break;
            }
            log(logs::eDEBUG2, "after CRC: " + utils::ToHexString(pm.m_packet));

            uint16_t size = (pm.m_packet[0] << 8) + pm.m_packet[1];
            pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 2); //размер поля данных
            //проверка ошибки размера
            if(size != pm.m_packet.size()) {
                pm.m_is_error = true;
                pm.m_error.sn_finish = lastCounter;

                log(logs::eERROR, "Bad result packet size: expected " + std::to_string(pm.m_packet.size())
                                      + ", but received " + std::to_string(size));
                return pm;
            }

            if(!pm.m_is_error)
                pm.m_is_built_complete = true;
        }
    }

    log(logs::eDEBUG3, "~buildPacket(3), mapConnection size: " + std::to_string(m_map_connections.size()));
    return pm;
}

Socket::MapConnectionsIterator Socket::findOrCreateConnection(const IpPort &remote_ip_port) noexcept {
    auto it = m_map_connections.find(remote_ip_port);
    if(it != m_map_connections.end()) return it;

    log(logs::eDEBUG,
        "add connection(1): " + remote_ip_port.to_string()
            + ", map_size:" + std::to_string(m_map_connections.size()),
        logs::to_color_string(logs::eBRIGHT_GREEN_BG, "add connection(1): " + remote_ip_port.to_string())
            + ", " + "map_size:" + std::to_string(m_map_connections.size()));
    return m_map_connections.insert(std::make_pair(remote_ip_port, Connection())).first;
}

void Socket::updateLastOutputActivityTime(const IpPort& remote_ip_port) noexcept {
    log(logs::eDEBUG2, "updateLastOutputActivityTime " + remote_ip_port.to_string());

    auto it = m_map_connections.find(remote_ip_port);
    if(it == m_map_connections.end()) {
        it = findOrCreateConnection(remote_ip_port);

        //сигнализировать о новом подключении
        if(m_settings.getNewConnectionCallback())
            m_settings.getNewConnectionCallback()(remote_ip_port);
    }

    it->second.m_last_output_activity = std::chrono::system_clock::now();
}

void Socket::log(const logs::LEVEL level, const std::string& log_message,
                 const std::string& color_log_message) noexcept {
    if(level > m_settings.getLogLevel()) return;

    LoggerSettings::LogCallback currentCallback         = nullptr;
    LoggerSettings::LogCallback currentColorCallback    = nullptr;
    std::string levelSubstring      = "";
    std::string timeString          = "";
    std::string coloredTimeString   = "";
    if(m_settings.isLogTimeEnabled()) {
        timeString = logs::get_time_string() + " ";
        if(timeString != m_last_time_string) {
            m_time_color_flag = !m_time_color_flag;
            m_last_time_string = timeString;
        }

        std::vector<logs::COLOR> timeColor = {};
        if(m_time_color_flag)   timeColor = {logs::eBLACK_BG, logs::eWHITE_FG};
        else                    timeColor = {logs::eBLACK_BG, logs::eBRIGHT_GRAY_FG};

        coloredTimeString = logs::to_color_string(timeColor, logs::get_time_string()) + " ";
    }

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

    //обычный вывод
    if(currentCallback)
        currentCallback(
            timeString
            + logs::columned(std::string("[")
                                 + to_string(m_socket_type)
                                 + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                 + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " "
            + log_message
            + "\n"
            );
    //цветной вывод
    if(currentColorCallback)
        currentColorCallback(
            coloredTimeString
            + logs::columned(level, std::string("[")
                                        + to_string(m_socket_type)
                                        + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                        + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " "
            + (color_log_message.empty() ? log_message : color_log_message)
            + "\n"
            );
}

bool Socket::sendRawMsg(const PacketMessage &packet_message) noexcept {
    log(logs::eDEBUG2, std::string("sendRaw ")
                          + "(" + std::to_string(packet_message.m_packet.size()) + ")"
                          + "[0x" + utils::ToHexString(packet_message.m_packet) + "] "
                          + packet_message.m_ip_port.to_string("to"));
    return sendRawMsg(packet_message.m_ip_port.ip, packet_message.m_ip_port.port, packet_message.m_packet);
}

void Socket::close() noexcept {
    if(m_socket_fd) {
        ::close(m_socket_fd);
        log(logs::eWARNING, "The socket " + IpPort{m_local_ip, m_local_port}.to_string() + " has been freed");
        m_socket_fd = -1;
    }
}

void UDPSocket::sendFragments(const IpPort &remote_ip_port, const PacketType type,
                              const Packet &packet, const bool need_ack) noexcept {
    Config json;
    json.parseJson(ConvertFromPacket(packet));

    log(type != eControlType ? logs::eINFO : logs::eDEBUG,
        "Send: " + ToString(type) + " "
            + (json.isEmpty() ? "[Data:0x" + utils::ToHexString(packet) + "]"
                              : "[Json:" + json.toString() + "]"
                                    + " / [Data:" + "0x" + utils::ToHexString(packet) + "]"
               ) + " "
            + remote_ip_port.to_string("to"),
        "Send: " + ToString(type) + " "
            + (json.isEmpty() ? "[Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::ToHexString(packet)) + "]"
                              : "[Json:" + logs::to_color_string(FULL_MSG_COLOR, json.toString()) + "]"
                                    + " / [Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::ToHexString(packet)) + "]"
               ) + " "
            + remote_ip_port.to_string("to"));

    uint8_t techInformationSize = 1;//1B: заголовок
    techInformationSize += 2;       //2B: globSN, SN
    Packet innerData;
    //=CHIP_and_CRC_and_SIZE_and_DATA===========================================
    //оставляем место для поля CRC
    switch(m_settings.getCrcLevel()) {
    case eCRC_8:
        techInformationSize += 1;   //1B: CRC
        innerData.push_back(0);
        break;
    case eCRC_16:
        techInformationSize += 2;   //2B: CRC
        innerData.push_back(0);
        innerData.push_back(0);
        break;
    case eCRC_32:
        techInformationSize += 4;   //4B: CRC
        innerData.push_back(0);
        innerData.push_back(0);
        innerData.push_back(0);
        innerData.push_back(0);
        break;
    default:        break;
    }

//    techInformationSize += 2;       //2B: size of data ==> размер сообщения указывается ТОЛЬКО в первом фрагменте
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
        case eCRC_8:    utils::CheckCrc8(innerData);    break;
        case eCRC_16:   utils::CheckCrc16(innerData);   break;
        case eCRC_32:   utils::CheckCrc32(innerData);   break;
        default:        break;
        }
        //применяем шифрование на данный пакет
        if(m_settings.isChipheringEnabled())
            chiphering(innerData);
    }
    //==========================================================================

    //подготавливаем пакеты к отправке
    Packet      fragment;
    size_t      currentPos = 0;
    uint16_t    currentFragmentSize;

    bool        isFirstEECsaved = false;
    EECounter   firstSn(255);
    EECounter   lastSn(255);

    std::vector<PacketMessage> fragments;
    bool isStart    = true;
    bool isFinish   = false;
    while(innerData.size() - currentPos > 0) {
        uint16_t availableSize = m_settings.getMaxLength();
        if(availableSize < techInformationSize + 1) {
            availableSize = techInformationSize + 1;
        }

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
                ph.isChip   = m_settings.isChipheringEnabled();
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
            pm.m_ip_port      = remote_ip_port;
            pm.m_packet       = buf;
            pm.m_sn           = saved; //для записи НЕ инкрементированного значения
            pm.m_header.type  = type;
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
    //дальнейшая обработка пакета происходит в функции tick()

    PacketMessage pm;
    {
        pm.m_ip_port      = remote_ip_port;
        pm.m_packet       = packet;
        pm.m_header.type  = type;
        //границы глобального пакета
        pm.m_range.start  = firstSn;
        pm.m_range.finish = lastSn;
    }
    if(need_ack) {
        m_sent_global_packets.push_back(pm);

        JsonMessage jm = pm;
        std::string appendString = "";
        appendString = "to map_global_packets, size: " + std::to_string(m_sent_global_packets.size());
        log(logs::eDEBUG,
            "append ["
                + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(pm.m_packet)
                                     : "Json:" + jm.m_json.toString())
                + "] " + appendString);//,
            logs::to_color_string(GLOBAL_APPEND_MSG_COLOR, "append ["
                + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(pm.m_packet)
                                     : "Json:" + jm.m_json.toString())
                + "] " + appendString);
    }
    m_output_threads_mutex.unlock();
}

void UDPSocket::tick() noexcept {
    std::map<IpPort, Config> acknowledgeMap;
    checkConnections();

    //отправка фрагментов
    sendAutoMsg();

    //приём фрагментов
    Config jRecv = recvAutoMsg(1);

    //сборка пакетов для всех активных соединений
    for(auto it = m_map_connections.begin(); it != m_map_connections.end(); it++) {
        PacketMessage b_pm = buildPacket(it);

        Config jProc = processingBuiltPacket(b_pm);
        jProc.append(jRecv);

        auto it_json = acknowledgeMap.find(it->first);
        if(it_json != acknowledgeMap.end())
            jProc.append(it_json->second);

        if(!jProc.isEmpty()) {
            log(logs::eDEBUG2, "send ack: " + jProc.toString() + b_pm.m_ip_port.to_string("to"));
            sendFragments(it->first, eControlType, ConvertToPacket(jProc.toString()), false);
        }
    }

    //выдача собранных пакетов пользователю API
    PacketMessage pm = getOutPacket();
    if(!pm.m_packet.empty() && m_settings.getPacketCallback())
        m_settings.getPacketCallback()(pm);
    JsonMessage jm = getOutJson();
    if(!jm.m_json.isEmpty() && m_settings.getJsonCallback())
        m_settings.getJsonCallback()(jm);
}

void UDPSocket::checkConnections() noexcept {
    log(logs::eDEBUG2, "checkConnections()");

    Config jPing(ValueType::eJson, "ping", getLocalIpPort().to_string());

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
            sendFragments(it->first, eControlType, ConvertToPacket(jPing.toString()), false);
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

            //сигнализировать о разрыве соединения
            if(m_settings.getConnectionResetCallback())
                m_settings.getConnectionResetCallback()(_currentIpPort);

            //удаление всех фрагментов, которые находятся в очереди отправки, с совпадающим адресатом
            log(logs::eDEBUG3, "checkConnections(), removing fragments");
            for(auto it2 = m_map_auto_sent_packets.begin(); it2 != m_map_auto_sent_packets.end(); it2++) {
                if(it2->second.m_ip_port == it->first)
                    it2 = m_map_auto_sent_packets.erase(it2);

                //если дошли до конца диапазона
                if(it2 == m_map_auto_sent_packets.end()) break;
            }

            //перепосылка недоставленных глобальных пакетов ==================================
            log(logs::eDEBUG3, "checkConnections(), prepare to resend global packets");
            m_output_threads_mutex.lock();
            for(auto it_global_packet = m_sent_global_packets.begin();
                 it_global_packet != m_sent_global_packets.end(); it_global_packet++
                 ) {
                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                IpPort ipPort = it_global_packet->m_ip_port;
                if(ipPort == _currentIpPort) {
                    packetsForSend.push_back({it_global_packet->m_packet, it_global_packet->m_header.type, ipPort});

                    it_global_packet = m_sent_global_packets.erase(it_global_packet);

                    if(it_global_packet == m_sent_global_packets.end()) break;
                }
            }
            m_output_threads_mutex.unlock();

            //если дошли до конца диапазона
            if(it == m_map_connections.end()) break;
        }

    }

    //перепосылка недоставленных глобальных пакетов ==================================
    log(logs::eDEBUG3, "checkConnections(), send found prepared packets");
    for(const prepPacket& current : packetsForSend) {
        if(!current.packet.empty() && current.type != eControlType) {
            log(logs::eDEBUG, "New try to send global packet " + ToString(current.type)
                                  + " [0x" + utils::ToHexString(current.packet) + "]");
            sendFragments(current.ipPort, current.type, current.packet); //переотправка
        }
    }
    //================================================================================
}

void UDPSocket::sendAutoMsg() noexcept {
    log(logs::eDEBUG2, "sendAutoMsg()");

    int counter = 0; //общий счётчик за проход функции

    //перепосылка недоставленных пакетов =============================================
    m_output_threads_mutex.lock();
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
            log(logs::eDEBUG, "New try to send [" + std::to_string(it->second.m_sn.get()) + "] fragment");

//            counter++; скорее всего не нужно
            if(it == m_map_auto_sent_packets.end()) break;
        }
    }
    m_output_threads_mutex.unlock();
    //================================================================================

    //отправка шифрованных пакетов если есть ключ=====================================
    m_output_threads_chip_mutex.lock();
    for(auto it = m_packets_wait_chip_key.begin();
         it != m_packets_wait_chip_key.end();
         it++
         ) {
        auto connection_it = findOrCreateConnection(it->m_ip_port);

        if(m_settings.isChipheringEnabled()
            && !connection_it->second.m_chip_key.key.empty()
            ) {
            log(logs::eDEBUG, "send chiphering message...");
            sendFragments(it->m_ip_port, eDataType, it->m_packet);   //внутри используется мьютекс
//            log(logs::eDEBUG, "chiphering message sent.");      //TEST

            //запоминание отправленных шифрованных пакетов

            //удаление текущего пакета из списка
            it = m_packets_wait_chip_key.erase(it);
        }

        if(it == m_packets_wait_chip_key.end()) break;
    }
    m_output_threads_chip_mutex.unlock();
    //================================================================================

    //постепенная отправка пакетов в сокет ===========================================
    m_output_threads_mutex.lock();
    while(!m_send_packets_buffer.empty()
           && ((counter < m_settings.getMaxMsgsSentOnTick()) || (m_settings.getMaxMsgsSentOnTick() < 0))
           ) {
        PacketMessage pm = m_send_packets_buffer.front();
        m_send_packets_buffer.pop_front();
        log(logs::eDEBUG,
            "Sending sn[" + std::to_string(pm.m_sn.get()) + "] fragment, data:[0x"
                + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.to_string("to"),
            logs::to_color_string(OUTPUT_MSG_COLOR,"Sending")
                + " sn[" + std::to_string(pm.m_sn.get()) + "] fragment, data:[0x"
                + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.to_string("to"));

        Socket::sendRawMsg(pm); //отправили
        updateLastOutputActivityTime(pm.m_ip_port);

        if(pm.m_header.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
            //запоминаем для ожидания ответа или досылки
            time_point_default current_time = std::chrono::system_clock::now();
            m_map_auto_sent_packets.insert(std::make_pair(current_time, pm));
        }

        counter++;
    }
    m_output_threads_mutex.unlock();
    //================================================================================

}

Config UDPSocket::recvAutoMsg(int timeout) noexcept {
    log(logs::eDEBUG2, "recvAutoMsg()");

    Config outputJson;
    PacketMessage pm = recvRawMsg(1);
    if(pm.m_packet.empty()) return outputJson;

    pm.m_header = unpackHeader(pm.m_packet[0]);

    if(pm.m_packet.size() < 3) return outputJson;
    uint8_t glob_sn = pm.m_packet[1];
    uint8_t sn      = pm.m_packet[2];
    pm.m_sn = EECounter(255);
    pm.m_sn.set_glob_pos(glob_sn);
    pm.m_sn.set_pos(sn);
    pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 3); //удалить первые три байта

    log(logs::eDEBUG2,
        "Received [" + std::to_string(sn) + "] sn fragment of type "
            + ToString(pm.m_header.type)
            + ", data:[0x"
            + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.to_string("from"),
        logs::to_color_string(INPUT_MSG_COLOR, "Received")
            + " [" + std::to_string(sn) + "] sn fragment of type "
            + ToString(pm.m_header.type)
            + ", data:[0x"
            + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.to_string("from"));

    if(pm.m_packet.empty()) return outputJson;
    auto it = m_map_connections.find(pm.m_ip_port);
    if(it == m_map_connections.end() && pm.m_header.type == eControlType) {
        //если первый пакет от адресата является контрольным и НЕ требует отчёта о доставке
        log(logs::eDEBUG,
            "Send initial ping for message sn=" + std::to_string(pm.m_sn.get())
                + " " + pm.m_ip_port.to_string(),
            "Send initial ping for message sn=" + std::to_string(pm.m_sn.get())
                + " " + logs::to_color_string(logs::eRED_BG, pm.m_ip_port.to_string()));
        outputJson.push_at("ping", getLocalIpPort().to_string());
    }

    if(pm.m_header.type != eControlType)
        outputJson.push_at("ack_sn", (double)pm.m_sn.get());

    appendNewFragment(pm);

    return outputJson;
}

Config UDPSocket::processingBuiltPacket(const PacketMessage &pm) noexcept {
    JsonMessage jm = pm;

    if(!pm.m_packet.empty()) {
        log(pm.m_header.type != eControlType ? logs::eINFO : logs::eDEBUG,
            "Built packet: "+ ToString(pm.m_header.type) + " ["
                + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(pm.m_packet)
                                     : "Json:" + jm.m_json.toString())
                + "] " + pm.m_ip_port.to_string("from"),
            "Built packet: "+ ToString(pm.m_header.type) + " ["
                + (jm.m_json.isEmpty() ? "Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::ToHexString(pm.m_packet))
                                     : "Json:" + logs::to_color_string(FULL_MSG_COLOR, jm.m_json.toString()))
                + "] " + pm.m_ip_port.to_string("from"));

        //обработка собранного пакета (1 за проход)
        log(logs::eDEBUG, "pm.m_header.type: " + ToString(pm.m_header.type));
        if(pm.m_header.type == eControlType) {
            if(jm.m_json.containsKey("ack_sn")) {
                uint8_t sn = jm.m_json["ack_sn"].getNumber();
                log(logs::eDEBUG,
                    "erasing sn[" + jm.m_json["ack_sn"].toString() + "]",
                    logs::to_color_string(logs::COLOR::eBRIGHT_MAGENTA_BG,
                                          "erasing sn[" + jm.m_json["ack_sn"].toString() + "]"));
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.m_sn.get() == sn) {
                        m_map_auto_sent_packets.erase(it->first);
                        break;
                    }
                }
            }
            if(jm.m_json.containsKey("ack_all_packet")) {
                uint8_t first_sn = jm.m_json["ack_all_packet"].getNumber(); //номер первого фрагмента сообщения

                m_output_threads_mutex.lock();
                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->m_range.start.get() == first_sn) {
                        PacketMessage tempPM;
                        tempPM.m_packet = it->m_packet;
                        jm.clear();
                        jm = tempPM;
                        std::string appendString = "";
                        appendString = ", map_global_packets size: " + std::to_string(m_sent_global_packets.size());
                        log(it->m_header.type != eControlType ? logs::eINFO : logs::eDEBUG,
                            "Message delivered ["
                                + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(tempPM.m_packet)
                                                     : "Json:" + jm.m_json.toString())
                                + "]" + appendString,
                            "Message delivered ["
                                + (jm.m_json.isEmpty() ? "Data:" + logs::to_color_string(FULL_MSG_COLOR, "0x" + utils::ToHexString(tempPM.m_packet))
                                                       : "Json:" + logs::to_color_string(FULL_MSG_COLOR, jm.m_json.toString()))
                                + "]" + appendString);
                        it = m_sent_global_packets.erase(it);
                        break;
                    }
                }
                m_output_threads_mutex.unlock();
            }
            if(jm.m_json.containsKey("packet_error_last_sn")) {
                uint8_t last_err_sn = jm.m_json["packet_error_last_sn"].getNumber(); //номер первого фрагмента сообщения

                //удалить все упоминания фрагментов пакета из очереди переотправок
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.m_sn <= last_err_sn) {
                        it = m_map_auto_sent_packets.erase(it);
                        if(it == m_map_auto_sent_packets.end()) break;
                    }
                }

                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                Packet packet;
                IpPort ipPort;
                PacketType type = eControlType; //заглушка от warning
                m_output_threads_mutex.lock();
                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->m_range.finish.get() == last_err_sn) {
                        packet  = it->m_packet;
                        type    = it->m_header.type;
                        ipPort  = it->m_ip_port;
                        it = m_sent_global_packets.erase(it);
                        break;
                    }
                }
                m_output_threads_mutex.unlock();

                if(!packet.empty() && type != eControlType) {
                    log(logs::eDEBUG, "New try to send packet fragment " + ToString(type)
                                          + " [0x" + utils::ToHexString(packet) + "]");
                    sendFragments(ipPort, type, packet); //переотправка
                }
            }
            if(jm.m_json.containsKey("get")) {
                //get = element of Config-Array
                Config requests = jm.m_json["get"];
                for(const auto& it_req : requests.getNamedRange()) {
                    switch(it_req.second->getType()) {
                    case ValueType::eString: {
                        if(it_req.second->getString() == "chip_key") {
                            log(logs::eDEBUG, "append chiphering key");
                            Config jChipKey;
                            jChipKey.push_at("key", "abcdefgjiklmnopqrstuvwxyz0123456789");
                            sendFragments(pm.m_ip_port, eControlType, ConvertToPacket(jChipKey.toString()));
                        }
                        break;
                    }
                    case ValueType::eNumber:
                    case ValueType::eBool:
                    case ValueType::eJson:
                    case ValueType::eArray:
                    case ValueType::eNull:
                        break;
                    }
                }
            }
            if(jm.m_json.containsKey("key")) {
                auto connection_it = findOrCreateConnection(pm.m_ip_port);
                connection_it->second.m_chip_key.key = jm.m_json["key"].getString();
            }
        } else {
            m_input_threads_mutex.lock();
            if(jm.m_json.isEmpty()) {
                log(logs::eDEBUG2,
                    "insert packet " + ToString(pm.m_header.type) + " to storage",
                    logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "insert packet " + ToString(pm.m_header.type) + " to storage"));
                m_map_recv_packets_buffer.push_back(pm);
            } else {
                log(logs::eDEBUG2,
                    "insert packet " + ToString(pm.m_header.type) + " to storage",
                    logs::to_color_string({logs::eGRAY_BG, logs::eWHITE_FG}, "insert packet " + ToString(pm.m_header.type) + " to storage"));
                m_map_recv_jsons_buffer.push_back(jm);
            }
            m_input_threads_mutex.unlock();
        }
    }

    //=========================================================================================
    Config controlAcknowledgement;
    if(pm.m_header.type != eControlType) {
        if(pm.m_is_built_complete)
            controlAcknowledgement.push_at("ack_all_packet", pm.m_sn.get());
        if(pm.m_is_error)
            controlAcknowledgement.push_at("packet_error_last_sn", pm.m_error.sn_finish.get());
    }
    //=========================================================================================
    return controlAcknowledgement;
}

UDPSocket::UDPSocket(const IpPort &local_ip_port, const SocketSettings& settings) noexcept {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_ip_port.port, local_ip_port.ip);
}

UDPSocket::UDPSocket(const uint16_t local_port, const std::string& local_ip,
                     const SocketSettings& settings) noexcept {
    m_socket_type = SocketType::eUDP;
    m_settings = settings;

    open(local_port, local_ip);
}

bool UDPSocket::sendRawMsg(const std::string &remote_ip, const uint16_t remote_port,
                           const Packet &packet) noexcept {
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

PacketMessage UDPSocket::recvRawMsg(int timeout) noexcept {
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
        char rpacket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket_ip, INET_ADDRSTRLEN);

        PacketMessage rpacket;
        rpacket.m_ip_port.ip    = rpacket_ip;
        rpacket.m_ip_port.port  = ntohs(sock.sin_port);
        rpacket.m_packet        = Packet(buf, buf + recv_num);

        log(logs::eDEBUG2, std::string("recvRaw ")
                              + "(" + std::to_string(rpacket.m_packet.size()) + ")"
                              + "[0x" + utils::ToHexString(rpacket.m_packet) + "] "
                              + rpacket.m_ip_port.to_string("from"));

        return rpacket;
    }

    return {};
}

void UDPSocket::startServer() noexcept {
    if(!isServerActive())
        open(m_local_port, m_local_ip);
}

void UDPSocket::stopServer() noexcept {
    if(isServerActive())
        close();
}

void UDPSocket::open(const uint16_t local_port, const std::string& local_ip) noexcept {
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

bool UDPSocket::isConnected(const IpPort &remote_ip_port) noexcept {
    auto it = m_map_connections.find(remote_ip_port);
    if(it != m_map_connections.end())   return true;
    else                                return false;
}

void UDPSocket::sendMsg(const IpPort& remote_ip_port, const Packet& packet) {
    if(!checkCorrectIp(remote_ip_port.ip))
        throw std::invalid_argument("incorrect destination IP");

    if(!m_settings.isChipheringEnabled())
        sendFragments(remote_ip_port, eDataType, packet); //отправляем сразу
    else {
        //создать подключение
        auto connection_it = findOrCreateConnection(remote_ip_port);

        //отправить запрос ключа
        Config jRequest;
        jRequest.push_at("get", "chip_key");
        sendFragments(remote_ip_port, eControlType, ConvertToPacket(jRequest.toString()));

        //положить текущее сообщение в очередь шифрованных сообщений на отправку
        m_output_threads_chip_mutex.lock();
        PacketMessage pm;
        pm.m_packet = std::move(packet);
        pm.m_ip_port = std::move(remote_ip_port);
        m_packets_wait_chip_key.push_back(pm);
        m_output_threads_chip_mutex.unlock();
        //продолжение алогоритма в sendAutoMsg...
    }
}

void UDPSocket::sendMsg(const IpPort& remote_ip_port, const Config& json) {
    sendMsg(remote_ip_port, ConvertToPacket(json.toString()));
}

PacketMessage UDPSocket::getOutPacket() noexcept {
    m_input_threads_mutex.lock();
    PacketMessage pm;
    if(m_map_recv_packets_buffer.size() > 0) {
        pm = m_map_recv_packets_buffer.front();
        m_map_recv_packets_buffer.pop_front();
    }
    m_input_threads_mutex.unlock();

    return pm;
}

JsonMessage UDPSocket::getOutJson() noexcept {
    m_input_threads_mutex.lock();
    JsonMessage jm;
    if(m_map_recv_jsons_buffer.size() > 0) {
        jm = m_map_recv_jsons_buffer.front();
        m_map_recv_jsons_buffer.pop_front();
    }
    m_input_threads_mutex.unlock();

    return jm;
}

std::string to_string(SocketType type) noexcept {
    switch(type){
    case eUDP:      return "UDP";
    case eTCP:      return "TCP";
    default:        return "SOCKET";
    }
}


