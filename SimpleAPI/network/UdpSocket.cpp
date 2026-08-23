#include "UdpSocket.h"


namespace simpleapi {


void UDPSocket::sendFragments(const IpPort &remote_ip_port, const PacketType type,
                              const Packet &packet, const bool need_ack) noexcept {
    using namespace logs;

    log(type != eControlType ? LEVEL::eINFO : LEVEL::eDEBUG,
        "Send: " + ToString(type)
            + " [Data:" + to_color_string(SIMPLEAPI_NETWORK_FULL_MSG_COLOR, "0x" + utils::ToHexString(packet)) + "] "
            + remote_ip_port.toString("to"));

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
        Packet buf;
        /* упаковываем фрагмент */ {
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
        log(LEVEL::eDEBUG,
            "append ["
                + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(pm.m_packet)
                                       : "Json:" + jm.m_json.toString())
                + "] " + appendString);//,
        to_color_string(SIMPLEAPI_NETWORK_GLOBAL_APPEND_MSG_COLOR, "append ["
                                                     + (jm.m_json.isEmpty() ? "Data:0x" + utils::ToHexString(pm.m_packet)
                                                                            : "Json:" + jm.m_json.toString())
                                                     + "] " + appendString);
    }
}

void UDPSocket::tick() noexcept {
    using namespace logs;

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
            log(LEVEL::eDEBUG_2, "send ack: " + jProc.toString() + it->first.toString("to"));
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
    using namespace logs;

    log(LEVEL::eDEBUG_3, "checkConnections()");

    Config jPing(ValueType::eJson, "ping", getLocalIpPort().toString());

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
        auto _one_third_inactivity = std::chrono::milliseconds(m_settings.getInactivityTimer() / 3);

        // если не было сообщений ОТ адреса дольше this->inactivityTimer/3, то отправить пинг
        // либо последняя отправка ДО адреса была дольше этого времени
        log(LEVEL::eDEBUG_3, "checkConnections(), pings");
        if(it->second.m_last_output_activity + _one_third_inactivity < _now
            && it->second.m_last_input_activity + _one_third_inactivity < _now
            )
        {
            log(LEVEL::eDEBUG_2, "Send ping to " + it->first.toString());
            log(LEVEL::eDEBUG_3, "Expected time: " + get_time_string(it->second.m_last_output_activity + _one_third_inactivity));
            sendFragments(it->first, eControlType, ConvertToPacket(jPing.toString()), false);
            it->second.m_last_output_activity = std::chrono::system_clock::now();
            continue;
        }

        IpPort _currentIpPort = it->first;
        //если долгое время не было сообщений от абонента, удалить все сообщения до него
        log(LEVEL::eDEBUG_3, "checkConnections(), bad connection");
        if(it->second.m_last_input_activity + _inactivity < _now)
        {
            log(LEVEL::eWARNING,
                to_color_string(SIMPLEAPI_NETWORK_CRITICAL_MSG_COLOR, "Connection " + _currentIpPort.toString()
                                                        + " removed, last activity at "
                                                        + get_time_string(it->second.m_last_input_activity)));
            it = m_map_connections.erase(it);

            //сигнализировать о разрыве соединения
            if(m_settings.getConnectionResetCallback())
                m_settings.getConnectionResetCallback()(_currentIpPort);

            //удаление всех фрагментов, которые находятся в очереди отправки, с совпадающим адресатом
            log(LEVEL::eDEBUG_3, "checkConnections(), removing fragments");
            for(auto it2 = m_map_auto_sent_packets.begin(); it2 != m_map_auto_sent_packets.end(); it2++) {
                if(it2->second.m_ip_port == it->first)
                    it2 = m_map_auto_sent_packets.erase(it2);

                //если дошли до конца диапазона
                if(it2 == m_map_auto_sent_packets.end()) break;
            }

            //перепосылка недоставленных глобальных пакетов ==================================
            log(LEVEL::eDEBUG_3, "checkConnections(), prepare to resend global packets");
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

            //если дошли до конца диапазона
            if(it == m_map_connections.end()) break;
        }

    }

    //перепосылка недоставленных глобальных пакетов ==================================
    log(LEVEL::eDEBUG_3, "checkConnections(), send found prepared packets");
    for(const prepPacket& current : packetsForSend) {
        if(!current.packet.empty() && current.type != eControlType) {
            log(LEVEL::eDEBUG, "New try to send global packet " + ToString(current.type)
                                   + " [0x" + utils::ToHexString(current.packet) + "]");
            sendFragments(current.ipPort, current.type, current.packet); //переотправка
        }
    }
    //================================================================================
}

void UDPSocket::sendAutoMsg() noexcept {
    using namespace logs;

    log(LEVEL::eDEBUG_3, "sendAutoMsg()");

    int counter = 0; //общий счётчик за проход функции

    //перепосылка недоставленных пакетов =============================================
    for(auto it = m_map_auto_sent_packets.begin();
         it != m_map_auto_sent_packets.end() && ((counter < m_settings.getMaxMsgsSentOnTick())
                                                 || (m_settings.getMaxMsgsSentOnTick() < 0));
         it++) {
        time_point_default tp = it->first;
        tp += std::chrono::seconds(1); // одной секунды более чем достаточно для отправки сообщения и получения ответа
        PacketMessage pm = it->second;
        if(tp < std::chrono::system_clock::now()) { //нужно переотправить
            it = m_map_auto_sent_packets.erase(it);
            m_send_packets_buffer.push_front(pm);
            log(LEVEL::eDEBUG, "Sending timeout, new try to send [" + std::to_string(pm.m_sn.get()) + "] fragment");

            //            counter++; скорее всего не нужно
            if(it == m_map_auto_sent_packets.end()) break;
        }
    }
    //================================================================================

    //отправка шифрованных пакетов если есть ключ=====================================
    for(auto it = m_packets_wait_chip_key.begin();
         it != m_packets_wait_chip_key.end();
         it++
         ) {
        auto connection_it = findOrCreateConnection(it->m_ip_port);

        if(m_settings.isChipheringEnabled()
            && !connection_it->second.m_chip_key.key.empty()
            ) {
            log(LEVEL::eDEBUG, "send chiphering message...");
            sendFragments(it->m_ip_port, eDataType, it->m_packet);   //внутри используется мьютекс
            //            log(LEVEL::eDEBUG, "chiphering message sent.");      //TEST

            //запоминание отправленных шифрованных пакетов

            //удаление текущего пакета из списка
            it = m_packets_wait_chip_key.erase(it);
        }

        if(it == m_packets_wait_chip_key.end()) break;
    }
    //================================================================================

    //постепенная отправка пакетов в сокет ===========================================
    while(!m_send_packets_buffer.empty()
           && ((counter < m_settings.getMaxMsgsSentOnTick()) || (m_settings.getMaxMsgsSentOnTick() < 0))
           ) {
        PacketMessage pm = m_send_packets_buffer.front();
        m_send_packets_buffer.pop_front();
        log(LEVEL::eDEBUG,
            to_color_string(SIMPLEAPI_NETWORK_OUTPUT_FRAGMENT_COLOR,"sending")
                + " sn [" + std::to_string(pm.m_sn.get()) + "] fragment, data:[0x"
                + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.toString("to"));

        Socket::sendRawMsg(pm); //отправили
        updateLastOutputActivityTime(pm.m_ip_port);

        if(pm.m_header.type != eControlType) { //контрольные пакеты не перепосылаются, поэтому хранить их не нужно
            //запоминаем для ожидания ответа или досылки
            time_point_default current_time = std::chrono::system_clock::now();
            m_map_auto_sent_packets.insert(std::make_pair(current_time, pm));
        }

        counter++;
    }
    //================================================================================

}

Config UDPSocket::recvAutoMsg(int timeout) noexcept {
    using namespace logs;

    log(LEVEL::eDEBUG_3, "recvAutoMsg()");

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

    log(LEVEL::eDEBUG_2,
        to_color_string(SIMPLEAPI_NETWORK_INPUT_FRAGMENT_COLOR, "received")
            + " [" + std::to_string(sn) + "] sn fragment of type "
            + ToString(pm.m_header.type)
            + ", data:[0x"
            + utils::ToHexString(pm.m_packet) + "] " + pm.m_ip_port.toString("from"));

    if(pm.m_packet.empty()) return outputJson;
    auto it = m_map_connections.find(pm.m_ip_port);
    if(it == m_map_connections.end() && pm.m_header.type == eControlType) {
        //если первый пакет от адресата является контрольным и НЕ требует отчёта о доставке
        log(LEVEL::eDEBUG,
            "Send initial ping for message sn [" + std::to_string(pm.m_sn.get()) + "] "
                + to_color_string(COLOR::eRED_BG, pm.m_ip_port.toString()));
        outputJson.push_at("ping", getLocalIpPort().toString());
    }

    if(pm.m_header.type != eControlType)
        outputJson.push_at("ack_sn", Config(ValueType::eArray, (double)pm.m_sn.get()));

    appendNewFragment(pm, outputJson);

    return outputJson;
}

// метод выполнется на стороне приёма пакетов
Config UDPSocket::processingBuiltPacket(const PacketMessage &pm) noexcept {
    using namespace logs;
    JsonMessage jm = pm;

    if(!pm.m_packet.empty()) {
        log(pm.m_header.type != eControlType ? LEVEL::eINFO : LEVEL::eDEBUG,
            "Built packet: "+ ToString(pm.m_header.type) + " ["
                + (jm.m_json.isEmpty() ? "Data:" + to_color_string(SIMPLEAPI_NETWORK_FULL_MSG_COLOR, "0x" + utils::ToHexString(pm.m_packet))
                                       : "Json:" + to_color_string(SIMPLEAPI_NETWORK_FULL_MSG_COLOR, jm.m_json.toString()))
                + "] " + pm.m_ip_port.toString("from"));

        //обработка собранного пакета (1 за проход)
        log(LEVEL::eDEBUG, "pm.m_header.type: " + ToString(pm.m_header.type));
        if(pm.m_header.type == eControlType) {
            // отчёт о доставке фрагментов
            if(jm.m_json.containsKey("ack_sn") && jm.m_json["ack_sn"].isArray()) {
                std::vector<Color> colors = {COLOR::eBRIGHT_GRAY_BG};

                // ack_sn - массив чисел
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++)
                {
                    uint8_t sn = it->second.m_sn.get();
                    if(jm.m_json["ack_sn"].containsValue(sn))
                    {
                        log(LEVEL::eDEBUG, to_color_string(colors, "Received ACK, erasing sn [" + std::to_string(sn) + "]"));
                        it = m_map_auto_sent_packets.erase(it);
                    }

                    // защита от пропуска конца массива при удалении итератора
                    if(it == m_map_auto_sent_packets.end()) break;
                }
            }

            // все фрагменты пакета доставлены, нужно очистить буфер
            if(jm.m_json.containsKey("ack_all_packet")) {
                uint8_t first_sn = jm.m_json["ack_all_packet"].getNumber(); //номер первого фрагмента сообщения

                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->m_range.start.get() == first_sn) {
                        PacketMessage tempPM;
                        tempPM.m_packet = it->m_packet;
                        jm.clear();
                        jm = tempPM;
                        std::string appendString = "";
                        appendString = ", map_global_packets size: " + std::to_string(m_sent_global_packets.size());
                        log(it->m_header.type != eControlType ? LEVEL::eINFO : LEVEL::eDEBUG,
                            "Message delivered ["
                                + (jm.m_json.isEmpty() ? "Data:" + to_color_string(SIMPLEAPI_NETWORK_FULL_MSG_COLOR, "0x" + utils::ToHexString(tempPM.m_packet))
                                                       : "Json:" + to_color_string(SIMPLEAPI_NETWORK_FULL_MSG_COLOR, jm.m_json.toString()))
                                + "]" + appendString);
                        it = m_sent_global_packets.erase(it);
                        break;
                    }
                }
            }

            // перепосылка недоставленных фрагментов
            if(jm.m_json.containsKey("nack_sn") && jm.m_json["nack_sn"].isArray()) {
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++)
                {
                    PacketMessage pm = it->second;
                    uint8_t sn       = pm.m_sn.get();
                    if(jm.m_json["nack_sn"].containsValue(sn)) { //нужно переотправить
                        it = m_map_auto_sent_packets.erase(it); // удаляем фрагмент из таблицы ожидания для переотправки
                        m_send_packets_buffer.push_front(pm);
                        log(LEVEL::eDEBUG, "Received NACK, new try to send [" + std::to_string(sn) + "] fragment");
                    }

                    // защита от пропуска конца массива при удалении итератора
                    if(it == m_map_auto_sent_packets.end()) break;
                }
            }

            // возникла ошибка при декодировании собранного сообщения (все фрагменты), нужно переупаковать и отправить фрагменты сообщения заново
            if(jm.m_json.containsKey("packet_error_last_sn")) {
                uint8_t last_err_sn = jm.m_json["packet_error_last_sn"].getNumber(); //номер первого фрагмента сообщения

                //удалить все упоминания фрагментов пакета из очереди переотправок
                for(auto it = m_map_auto_sent_packets.begin(); it != m_map_auto_sent_packets.end(); it++) {
                    if(it->second.m_sn <= last_err_sn) {
                        it = m_map_auto_sent_packets.erase(it);

                        // защита от пропуска конца массива при удалении итератора
                        if(it == m_map_auto_sent_packets.end()) break;
                    }
                }

                //удалить из глобального списка (потому что станет дубликатом) и отправить заново
                Packet packet;
                IpPort ipPort;
                PacketType type = eControlType; //заглушка от warning
                for(auto it = m_sent_global_packets.begin(); it != m_sent_global_packets.end(); it++) {
                    if(it->m_range.finish.get() == last_err_sn) {
                        packet  = it->m_packet;
                        type    = it->m_header.type;
                        ipPort  = it->m_ip_port;
                        it      = m_sent_global_packets.erase(it);
                        break;
                    }
                }

                if(!packet.empty() && type != eControlType) {
                    log(LEVEL::eDEBUG, "New try to send packet fragment " + ToString(type)
                                           + " [0x" + utils::ToHexString(packet) + "]");
                    sendFragments(ipPort, type, packet); //переотправка
                }
            }

            // другая сторона запрашивает какой-либо из параметров
            if(jm.m_json.containsKey("get")) {
                //get = element of Config-Array
                Config requests = jm.m_json["get"];
                for(const auto& it_req : requests.getNamedRange()) {
                    switch(it_req.second->getType()) {
                    case ValueType::eString: {
                        if(it_req.second->getString() == "chip_key") {
                            log(LEVEL::eDEBUG, "append chiphering key");
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

            // другая сторона отправила открытую часть ключа шифрования
            if(jm.m_json.containsKey("key")) {
                auto connection_it = findOrCreateConnection(pm.m_ip_port);
                connection_it->second.m_chip_key.key = jm.m_json["key"].getString();
            }
        } else {
            // обработка пользовательского пакета
            if(jm.m_json.isEmpty()) {
                log(LEVEL::eDEBUG_2,
                    to_color_string({COLOR::eGRAY_BG, COLOR::eWHITE_FG},
                                    "insert packet " + ToString(pm.m_header.type) + " to storage"));
                m_map_recv_packets_buffer.push_back(pm);
            } else {
                log(LEVEL::eDEBUG_2,
                    to_color_string({COLOR::eGRAY_BG, COLOR::eWHITE_FG},
                                    "insert packet " + ToString(pm.m_header.type) + " to storage"));
                m_map_recv_jsons_buffer.push_back(jm);
            }
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
    using namespace logs;

    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(remote_port);
    if(!inet_pton(AF_INET, remote_ip.c_str(), &sock.sin_addr.s_addr))
        log(LEVEL::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
    int res = sendto(m_socket_fd, (char*)packet.data(), packet.size(), 0,
                     (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
    if(res < 0) {
        log(LEVEL::eERROR, "sendTo() failed, error(" + std::to_string(errno) + ")");
        return false;
    }
    return res > 0;
}

PacketMessage UDPSocket::recvRawMsg(int timeout) noexcept {
    using namespace logs;

    if(!this->isServerActive()) return {};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket_fd, &fds);
    struct timeval t;
    if(timeout > 0) {
        t.tv_sec    = timeout / 1000;
        t.tv_usec   = (timeout % 1000) * 1000;
    }
    char buf[SIMPLEAPI_NETWORK_MAX_PACKET_LENGTH];
    int recv_num;

    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    socklen_t socklen = sizeof(sock);
    recv_num = select(m_socket_fd + 1, &fds, NULL, NULL, (timeout > 0 ? &t : NULL));
    if(recv_num < 0) {
        if(errno != EINTR) /* Interrupted system call */
            log(LEVEL::eERROR, "select() failed, error(" + std::to_string(errno) + ")");
        return PacketMessage();
    }
    if(recv_num > 0) {
        recv_num = recvfrom(m_socket_fd, buf,
                            SIMPLEAPI_NETWORK_MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
    }

    if(recv_num < 0) {
        log(LEVEL::eERROR, "recvfrom() failed, error(" + std::to_string(errno) + ")");
    } else if(recv_num > 0) {
        char rpacket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)rpacket_ip, INET_ADDRSTRLEN);

        PacketMessage rpacket;
        rpacket.m_ip_port.ip    = rpacket_ip;
        rpacket.m_ip_port.port  = ntohs(sock.sin_port);
        rpacket.m_packet        = Packet(buf, buf + recv_num);

        log(LEVEL::eDEBUG_2, std::string("recvRaw ")
                                + "(" + std::to_string(rpacket.m_packet.size()) + ")"
                                + "[0x" + utils::ToHexString(rpacket.m_packet) + "] "
                                + rpacket.m_ip_port.toString("from"));

        return rpacket;
    }

    return {};
}

bool UDPSocket::startServer() noexcept {
    if(!isServerActive())
        return open(m_local_port, m_local_ip);
    return false;
}

void UDPSocket::stopServer() noexcept {
    if(isServerActive())
        close();
}

bool UDPSocket::open(const uint16_t local_port, const std::string& local_ip) noexcept {
    using namespace logs;

    m_local_ip      = local_ip;
    m_local_port    = local_port;

    // create
    m_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket_fd < 0) {
        log(LEVEL::eERROR, "socket() failed, error(" + std::to_string(errno) + ")");
        return false;
    }

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(local_port);
    if(local_ip.empty())
        sock.sin_addr.s_addr = INADDR_ANY;
    else {
        if(!inet_pton(AF_INET, local_ip.c_str(), &sock.sin_addr.s_addr)) {
            log(LEVEL::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
            return false;
        }
    }
    int res = bind(m_socket_fd, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0) {
        log(LEVEL::eERROR, "bind() failed with localIP(" + local_ip + ")"
                               + ", port(" + std::to_string(local_port) + "), error("
                               + std::to_string(errno) + ")");
        close();
        return false;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr.s_addr), str, INET_ADDRSTRLEN);
    log(LEVEL::eWARNING, "Socket binded at " + IpPort{str, local_port}.toString());
    return true;
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
        PacketMessage pm;
        pm.m_packet = std::move(packet);
        pm.m_ip_port = std::move(remote_ip_port);
        m_packets_wait_chip_key.push_back(pm);
        //продолжение алогоритма в sendAutoMsg...
    }
}

void UDPSocket::sendMsg(const IpPort& remote_ip_port, const Config& json) {
    sendMsg(remote_ip_port, ConvertToPacket(json.toString()));
}

PacketMessage UDPSocket::getOutPacket() noexcept {
    PacketMessage pm;
    if(m_map_recv_packets_buffer.size() > 0) {
        pm = m_map_recv_packets_buffer.front();
        m_map_recv_packets_buffer.pop_front();
    }

    return pm;
}

JsonMessage UDPSocket::getOutJson() noexcept {
    JsonMessage jm;
    if(m_map_recv_jsons_buffer.size() > 0) {
        jm = m_map_recv_jsons_buffer.front();
        m_map_recv_jsons_buffer.pop_front();
    }

    return jm;
}


} // namespace simpleapi
