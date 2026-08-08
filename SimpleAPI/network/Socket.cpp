#include "Socket.h"
#include <iostream>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <regex>
#include "../utils/Utils.h"


namespace simpleapi {


bool Socket::checkCorrectIp(const std::string& ip_string) noexcept {
    using namespace logs;

    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    if(!inet_pton(AF_INET, ip_string.c_str(), &sock.sin_addr.s_addr)) {
        log(LEVEL::eERROR, "inet_pton() failed, error(" + std::to_string(errno) + ")");
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

void Socket::appendNewFragment(const PacketMessage& received_pm, Config& output_message) noexcept
{
    using namespace logs;

    auto it = findOrCreateConnection(received_pm.m_ip_port);

    it->second.m_last_input_activity = std::chrono::system_clock::now();

    log(LEVEL::eDEBUG3, "appendNewFragment(), mapConnection size: " + std::to_string(m_map_connections.size()));

    if(received_pm.m_packet.empty()) {
        log(LEVEL::eDEBUG3, "~appendNewFragment(), packet empty");
        return;
    }

    log(LEVEL::eDEBUG, to_color_string(SIMPLEAPI_NETWORK_INPUT_FRAGMENT_COLOR,
                                       "received sn [" + std::to_string(received_pm.m_sn.get()) + "]"
                                           + ", expected_sn [" + std::to_string(it->second.m_in_next_sn.get()) + "]"));

    if(it->second.m_in_sn_last_recv < received_pm.m_sn)
        it->second.m_in_sn_last_recv = received_pm.m_sn;
    //всё, что пришло до этого - удалится
    EECounter rmSn = it->second.m_in_sn_last_recv - (it->second.m_in_sn_last_recv.size() / 2); //размер окна - половина диапазонаe

    // если пришедший счётчик совпал с ожидаемым
    if(received_pm.m_sn == it->second.m_in_next_sn) {
        //NOTE: игнор уже пришедших фрагментов произойдёт здесь же
        it->second.m_map_recv_builded_messages.insert(std::make_pair(received_pm.m_sn, received_pm));
        it->second.m_in_next_sn++;
        log(LEVEL::eDEBUG, "processing build...");
    } else if (received_pm.m_sn < it->second.m_in_next_sn) { // пришедший счётчик уже был, игнорирование
        log(LEVEL::eDEBUG, to_color_string({COLOR::eGRAY_BG, COLOR::eWHITE_FG}, "IGNORING")
                               + ", fragment has already been received!");

        //если имеющийся пакет с таким SN отличается по содержанию, то необходимо обновить мапу
        auto it_fragment = it->second.m_map_recv_fragments.find(received_pm.m_sn);
        if(it_fragment != it->second.m_map_recv_fragments.end()) {
            if(it_fragment->second.m_packet != received_pm.m_packet) {
                log(LEVEL::eDEBUG, to_color_string(COLOR::eBRIGHT_YELLOW_BG,
                                                   "This fragment is different from the existing one, updating map..."));
                it->second.m_map_recv_fragments.erase(it_fragment);
                it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.m_sn, received_pm));
            }
        }
    } else { // пришедший счётчик больше ожидаемого, сборка пакета откладывается; посылается NACK
        // предварительная очистка списка отправленных индексов NACK
        {
            auto time_now = std::chrono::system_clock::now();

            // удаляются все SN до текущего ожидаемого
            for(auto it_nack = it->second.m_sended_nacks.begin(); it_nack != it->second.m_sended_nacks.end(); it_nack++)
            {
                if(it_nack->second != it->second.m_in_next_sn.get())
                    it_nack = it->second.m_sended_nacks.erase(it_nack);

                if(it_nack->second == it->second.m_in_next_sn.get())
                    break; //текущий ожидаемый SN удаляется только при превышении таймера

                // защита от пропуска конца массива при удалении итератора
                if(it_nack == it->second.m_sended_nacks.end()) break;
            }

            // удаляются все записи с превышением таймера на 1 секунду (ошибка доставки)
            // NOTE: вынесено во второй цикл во избежание ненужных коллизий
            for(auto it_nack = it->second.m_sended_nacks.begin(); it_nack != it->second.m_sended_nacks.end(); it_nack++)
            {
                if(time_now - it_nack->first > std::chrono::seconds(1))
                    it_nack = it->second.m_sended_nacks.erase(it_nack);

                // защита от пропуска конца массива при удалении итератора
                if(it_nack == it->second.m_sended_nacks.end()) break;
            }
        }

        log(LEVEL::eDEBUG, "fragment received, but will be processed later");
        it->second.m_map_recv_fragments.insert(std::make_pair(received_pm.m_sn, received_pm));

        // подготовка к отправке NACK
        if(!output_message.containsKey("nack_sn"))
            output_message.push_back("nack_sn", Config(ValueType::eArray));

        // нужно собрать NACK на каждый из неполученных фрагментов
        /* счётчик добавляется:
         * - только если он отсутствует в it->second.m_sended_nacks
         * - строго в диапазоне it->second.m_in_next_sn <= X < received_pm.m_sn
        */
        for(EECounter temp_counter = it->second.m_in_next_sn;
             temp_counter.get() != received_pm.m_sn.get();
             temp_counter.add())
        {
            bool already_in = false;
            // первичный ключ - временнАя метка, поэтому поиск ручной через цикл for
            for(auto& it_nack : it->second.m_sended_nacks)
            {
                if(it_nack.second == temp_counter.get())
                {
                    already_in = true;
                    break;
                }
            }
            if(!already_in)
            {
                output_message["nack_sn"].push_back(temp_counter.get());
                log(LEVEL::eDEBUG,
                    to_color_string(COLOR::eBRIGHT_YELLOW_BG,
                                    "appendNewFragment(), append NACK for sn ["
                                        + std::to_string(it->second.m_in_next_sn.get()) + "]"));
            }
        }
    }

    //пройтись по poolRecvMessages и добрать по порядку к mapRecvBuildedMessages
    auto it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn);
    while(it_pool != it->second.m_map_recv_fragments.end()) {
        it->second.m_map_recv_builded_messages.insert(std::make_pair(it_pool->second.m_sn, it_pool->second));
        it->second.m_in_next_sn++;

        it_pool = it->second.m_map_recv_fragments.erase(it_pool);
        log(LEVEL::eDEBUG, "appendNewFragment(), new expected_sn:" + std::to_string(it->second.m_in_next_sn.get()));
        it_pool = it->second.m_map_recv_fragments.find(it->second.m_in_next_sn); //ищем следующий фрагмент очереди
    }

    //удалить всё, что теперь вне окна ожидания
    if(it->second.m_in_sn_last_recv.get() > (it->second.m_in_sn_last_recv.size() / 2)
        || it->second.m_in_sn_last_recv.get_glob() > 0
        ) {
        log(LEVEL::eDEBUG,
            to_color_string(COLOR::eBRIGHT_YELLOW_BG,
                            "appendNewFragment(), (window) rm sn [" + std::to_string(rmSn.get()) + "]"));
        it_pool = it->second.m_map_recv_fragments.begin();
        while(it_pool != it->second.m_map_recv_fragments.end()) {
            if(it_pool->first < rmSn) {
                log(LEVEL::eDEBUG,
                    to_color_string(COLOR::eBRIGHT_YELLOW_BG,
                                    "appendNewFragment(), remove wait sn ["
                                        + std::to_string(it->second.m_in_next_sn.get()) + "]"));

                it_pool = it->second.m_map_recv_fragments.erase(it_pool);
                if(it_pool == it->second.m_map_recv_fragments.end()) break;
            }
            it_pool++;
        }
    }
}

PacketMessage Socket::buildPacket(MapConnectionsIterator& it) noexcept {
    using namespace logs;

    log(LEVEL::eDEBUG3, "buildPacket(), prepare to build");
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
                log(LEVEL::eDEBUG, "first sn: " + std::to_string(firstCounter.get()));
                isFirstCounterSet = true;
                isStarted = true;

                pm.m_ip_port = it_build->second.m_ip_port;
                pm.m_header = it_build->second.m_header;
            }

            if(it_build->second.m_header.isLastFragment) {
                lastCounter = it_build->first;
                log(LEVEL::eDEBUG, "last sn: " + std::to_string(lastCounter.get()));
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

            log(LEVEL::eDEBUG3, "~buildPacket(1), mapConnection size: " + std::to_string(m_map_connections.size()));
            return pm;
        }

        //пакет соберётся, копируем в выходной PM.packet
        if(isStarted && isFinished) {
            for(const auto& _it : it->second.m_map_recv_builded_messages) {
                log(LEVEL::eDEBUG2,
                    to_color_string(COLOR::eYELLOW_FG,
                                    "MAP: current fragment: [0x" + utils::ToHexString(_it.second.m_packet) + "]"));
            }

            pm.m_sn = firstCounter; //номер первого фрагмента для индикации доставки глобального сообщения
            log(LEVEL::eDEBUG, "built recv packet: sn [" + std::to_string(pm.m_sn.get()) + "], type " + ToString(pm.m_header.type));
            //скопировать и удалить задействованные фрагменты
            bool this_last_fragment = false;
            EECounter counter = firstCounter;
            for(auto it_build = it->second.m_map_recv_builded_messages.find(counter++);
                 it_build != it->second.m_map_recv_builded_messages.end();
                 it_build = it->second.m_map_recv_builded_messages.find(counter++)
                 ) {

                if(it_build->second.m_sn == lastCounter) {
                    this_last_fragment = true;
                    log(LEVEL::eDEBUG, "this fragment is last");
                }

                log(LEVEL::eDEBUG2, "current fragment(sn:" + std::to_string(it_build->first.get())
                                      + "): [0x" + utils::ToHexString(it_build->second.m_packet) + "]");
                std::copy(std::begin(it_build->second.m_packet),
                          std::end(it_build->second.m_packet),
                          std::back_insert_iterator<Packet>(pm.m_packet));

                it_build = it->second.m_map_recv_builded_messages.erase(it_build);

                if(this_last_fragment) break;
            }
            log(LEVEL::eDEBUG, "result packet: [0x" + utils::ToHexString(pm.m_packet) + "]");

            //дешифрация
            dechiphering(pm.m_packet);
            //проверка контрольной суммы
            log(LEVEL::eDEBUG2, "before CRC: " + utils::ToHexString(pm.m_packet));
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
            default:
                break;
            }
            log(LEVEL::eDEBUG2, "after CRC: " + utils::ToHexString(pm.m_packet));

            uint16_t size = (pm.m_packet[0] << 8) + pm.m_packet[1];
            pm.m_packet.erase(pm.m_packet.begin(), pm.m_packet.begin() + 2); //размер поля данных
            //проверка ошибки размера
            if(size != pm.m_packet.size()) {
                pm.m_is_error = true;
                pm.m_error.sn_finish = lastCounter;

                log(LEVEL::eERROR, "Bad result packet size: expected " + std::to_string(pm.m_packet.size())
                                      + ", but received " + std::to_string(size));
                return pm;
            }

            if(!pm.m_is_error)
                pm.m_is_built_complete = true;
        }
    }

    log(LEVEL::eDEBUG3, "~buildPacket(3), mapConnection size: " + std::to_string(m_map_connections.size()));
    return pm;
}

Socket::MapConnectionsIterator Socket::findOrCreateConnection(const IpPort &remote_ip_port) noexcept {
    auto it = m_map_connections.find(remote_ip_port);
    if(it != m_map_connections.end()) return it;

    using namespace logs;
    log(LEVEL::eDEBUG,
        to_color_string({COLOR::eBRIGHT_GRAY_BG, COLOR::eBRIGHT_RED_FG}, "add connection(1): " + remote_ip_port.toString())
            + ", " + "map_size:" + std::to_string(m_map_connections.size()));
    return m_map_connections.insert(std::make_pair(remote_ip_port, Connection())).first;
}

void Socket::updateLastOutputActivityTime(const IpPort& remote_ip_port) noexcept {
    using namespace logs;

    log(LEVEL::eDEBUG2, "updateLastOutputActivityTime " + remote_ip_port.toString());

    auto it = m_map_connections.find(remote_ip_port);
    if(it == m_map_connections.end()) {
        it = findOrCreateConnection(remote_ip_port);

        //сигнализировать о новом подключении
        if(m_settings.getNewConnectionCallback())
            m_settings.getNewConnectionCallback()(remote_ip_port);
    }

    it->second.m_last_output_activity = std::chrono::system_clock::now();
}

void Socket::log(const logs::LEVEL level, std::string log_message) noexcept {
    using namespace logs;

    if(level > m_settings.getLogLevel()) return;

    using namespace logs;
    LoggerSettings::LogCallback currentCallback         = nullptr;
    LoggerSettings::LogCallback currentColorCallback    = nullptr;
    std::string levelSubstring      = "";
    std::string timeString          = "";
    std::string coloredTimeString   = "";
    if(m_settings.isLogTimeEnabled()) {
        timeString = get_time_string() + " ";
        if(timeString != m_last_time_string) {
            m_time_color_flag = !m_time_color_flag;
            m_last_time_string = timeString;
        }

        std::vector<Color> timeColor = {};
        if(m_time_color_flag)   timeColor = {Color(COLOR::eBLACK_BG), Color(COLOR::eWHITE_FG)};
        else                    timeColor = {Color(COLOR::eBLACK_BG), Color(COLOR::eBRIGHT_GRAY_FG)};

        coloredTimeString = to_color_string(timeColor, get_time_string()) + " ";
    }

    switch(level) {
    case LEVEL::eWARNING:
        currentCallback         = m_settings.getLogCallback();
        currentColorCallback    = m_settings.getColorLogCallback();
        levelSubstring          = ".w";
        break;
    case LEVEL::eINFO:
        currentCallback         = m_settings.getLogCallback();
        currentColorCallback    = m_settings.getColorLogCallback();
        levelSubstring          = ".i";
        break;
    case LEVEL::eDEBUG:
    case LEVEL::eDEBUG2:
    case LEVEL::eDEBUG3:
        currentCallback         = m_settings.getLogCallback();
        currentColorCallback    = m_settings.getColorLogCallback();
        levelSubstring          = ".d";
        break;
    case LEVEL::eERROR:
        currentCallback         = m_settings.getLogErrorCallback();
        currentColorCallback    = m_settings.getColorLogErrorCallback();
        levelSubstring          = ".e";
        break;
    default:
        currentCallback         = m_settings.getLogErrorCallback();
        currentColorCallback    = m_settings.getColorLogErrorCallback();
        levelSubstring          = ".unknown";
        break;
    }

    // цветной вывод
    if(currentColorCallback)
    {
        currentColorCallback(
            coloredTimeString
            + columned(level, std::string("[")
                                  + ToString(m_socket_type)
                                  + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                  + "]",
                       m_settings.getNameColumnSize(),
                       m_settings.isNameColumnRightAlignEnabled())
            + " "
            + (log_message.empty() ? log_message : log_message)
            + "\n"
            );
    }

    // обычный вывод
    if(currentCallback) {
        /* перед выводом удаляются все команды форматирования */ {
            static std::regex reg("\\x1B\[[0-9;]*m");
            log_message = std::regex_replace(log_message, reg, "");
        }
        currentCallback(
            timeString
            + columned(std::string("[")
                           + ToString(m_socket_type)
                           + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                           + "]",
                       m_settings.getNameColumnSize(),
                       m_settings.isNameColumnRightAlignEnabled())
            + " "
            + log_message
            + "\n"
            );
    }
}

bool Socket::sendRawMsg(const PacketMessage &packet_message) noexcept {
    using namespace logs;

    log(LEVEL::eDEBUG2, std::string("sendRaw ")
                          + "(" + std::to_string(packet_message.m_packet.size()) + ")"
                          + "[0x" + utils::ToHexString(packet_message.m_packet) + "] "
                          + packet_message.m_ip_port.toString("to"));
    return sendRawMsg(packet_message.m_ip_port.ip, packet_message.m_ip_port.port, packet_message.m_packet);
}

void Socket::close() noexcept {
    using namespace logs;

    if(m_socket_fd) {
        ::close(m_socket_fd);
        log(LEVEL::eWARNING, "The socket " + IpPort{m_local_ip, m_local_port}.toString() + " has been freed");
        m_socket_fd = -1;
    }
}

std::string ToString(SocketType type) noexcept {
    switch(type){
    case eUDP:      return "UDP";
    case eTCP:      return "TCP";
    default:        return "SOCKET";
    }
}

} // namespace simpleapi

