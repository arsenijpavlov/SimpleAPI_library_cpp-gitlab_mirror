#pragma once

#include "../utils/EECounter.h"
#include "../config/Config.h"
#include "IpPort.h"
#include "SocketSettings.h"

#include <deque>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <set>

#include <arpa/inet.h>
#include <sys/socket.h>


namespace simpleapi {

#define SIMPLEAPI_NETWORK_FULL_MSG_COLOR            { COLOR::eYELLOW_BG,         COLOR::eBLACK_FG, COLOR::eBOLD_TEXT }
#define SIMPLEAPI_NETWORK_GLOBAL_APPEND_MSG_COLOR   { COLOR::eBRIGHT_GRAY_BG,    COLOR::eBRIGHT_RED_FG }
#define SIMPLEAPI_NETWORK_CRITICAL_MSG_COLOR        { COLOR::eRED_BG,            COLOR::eWHITE_FG }
#define SIMPLEAPI_NETWORK_OUTPUT_MSG_COLOR          { COLOR::eBLUE_BG,           COLOR::eWHITE_FG }
#define SIMPLEAPI_NETWORK_INPUT_MSG_COLOR           { COLOR::eCYAN_BG,           COLOR::eWHITE_FG }
#define SIMPLEAPI_NETWORK_OUTPUT_FRAGMENT_COLOR     { COLOR::eBLUE_FG,           COLOR::eBRIGHT_GRAY_BG }
#define SIMPLEAPI_NETWORK_INPUT_FRAGMENT_COLOR      { COLOR::eBLUE_FG,           COLOR::eBRIGHT_GRAY_BG }

#define SIMPLEAPI_NETWORK_MAX_PACKET_LENGTH         65535

using time_point_default = std::chrono::system_clock::time_point;


enum SocketType {
    eUDP,
    eTCP
};
std::string ToString(SocketType type) noexcept;

struct ChipheringKey {
    std::string key;
};


/* TODO: нужно добавить отслеживание id соединения внутри пакетов
 *  - может возникнуть ситуация, когда одна из точек выйдет из строя и при восстановлении
 *    соединения отслеживание доставленных фрагментов сломается
 */
class Connection {
public:
    time_point_default  m_last_output_activity;
    time_point_default  m_last_input_activity;
    EECounter           m_out_sn;
    EECounter           m_in_sn_last_recv; //влияет на границу окна ожидания фрагментов
    EECounter           m_in_next_sn;

    ChipheringKey       m_chip_key;

    std::map<EECounter, PacketMessage> m_map_recv_fragments;        //фрагменты сообщений (в беспорядке)
    std::map<EECounter, PacketMessage> m_map_recv_builded_messages; //собранные по очереди фрагменты сообщений

    // храним список отправленных индексов NACK, чтобы не было дублей
    // NOTE: если прошло больше 1 секунды, то NACK дублируется
    std::map<time_point_default, uint8_t> m_sended_nacks;

    Connection() noexcept :
        m_last_output_activity(std::chrono::system_clock::now()),
        m_last_input_activity(std::chrono::system_clock::now()),
        m_out_sn(255),
        m_in_sn_last_recv(255),
        m_in_next_sn(255),
        m_chip_key({})
    {}
};

// абстрактный базовый класс
class Socket {
protected:
    using MapConnectionsIterator = std::map<IpPort, Connection>::iterator;

    int         m_socket_fd;
    SocketType  m_socket_type;
    std::string m_local_ip;
    uint16_t    m_local_port;
    std::map<IpPort, Connection> m_map_connections; //счётчики сообщений на отправку

    //цветной вывод времени (для визуального различия)
    std::string m_last_time_string;
    bool        m_time_color_flag;

public:
    SocketSettings  m_settings;

protected:
    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    //для доступа извне------------------------
    std::deque<PacketMessage>   m_send_packets_buffer;  //sendFragments(), sendAutoMsg()
    std::vector<PacketMessage>  m_sent_global_packets;  //запоминаем до тех пор, пока не придёт подтверждение о передаче всех фрагментов

    std::vector<PacketMessage>  m_packets_wait_chip_key;//запоминаем до тех пор, пока не придёт ключ для шифрования сообщения

    //собранные пакеты
    std::deque<PacketMessage>   m_map_recv_packets_buffer;  //buildPackets(), getOutPacket()
    std::deque<JsonMessage>     m_map_recv_jsons_buffer;    //buildPackets(), getOutJson()
    //-----------------------------------------

    bool            checkCorrectIp(const std::string& ip_string)                noexcept;

    uint8_t         packHeader(const PacketHeader& ph)                          noexcept;
    PacketHeader    unpackHeader(const uint8_t header)                          noexcept;
    EECounter&      getOutSeqNumber(const IpPort& ip_port)                      noexcept;
    void            appendNewFragment(const PacketMessage& received_pm, Config& output_message) noexcept;
    PacketMessage   buildPacket(MapConnectionsIterator& it)                     noexcept;
    MapConnectionsIterator
                    findOrCreateConnection(const IpPort& remote_ip_port)        noexcept;
    void            updateLastOutputActivityTime(const IpPort& remote_ip_port)  noexcept;

    void            log(const logs::LEVEL level, std::string log_message = "")  noexcept;
    void            setSettings(const SocketSettings settings = SocketSettings())
                                                                noexcept        { m_settings = settings; }
    SocketSettings  getSettings()                               noexcept        { return m_settings; }

    void            sendFragments(const std::string& remote_ip, const uint16_t remote_port,
                                  const PacketType type, const Packet& packet, const bool need_ack = true) noexcept
                                                        { sendFragments(IpPort{remote_ip, remote_port}, type, packet, need_ack); }
    virtual void    sendFragments(const IpPort& remote_ip_port, const PacketType type,
                                  const Packet& packet, const bool need_ack = true) noexcept = 0;

    virtual void    tick()                                      noexcept        = 0;
    virtual void    sendAutoMsg()                               noexcept        = 0;
    virtual Config  recvAutoMsg(int timeout)                    noexcept        = 0;
    virtual Config  processingBuiltPacket(const PacketMessage& pm) noexcept     = 0;
    //=====================================
    friend class SocketThread; //для функции tick()

public:
                    Socket() noexcept :
                        m_socket_fd(-1),
                        m_settings(SocketSettings())                            {}
    virtual         ~Socket()                                   noexcept        {}

    //-----------------------------------------
    void            setLogLevel(logs::LEVEL log_level)          noexcept        { m_settings.setLogLevel(log_level); }
    IpPort          getLocalIpPort()                            noexcept        { return IpPort{m_local_ip, m_local_port}; }
    virtual bool    isConnected(const IpPort& remote_ip_port)   noexcept        = 0;
    bool            isServerActive()                            noexcept        { return m_socket_fd > 0; }
    //-----------------------------------------
    void            chiphering(Packet& packet)                  noexcept        {}
    void            dechiphering(Packet& packet)                noexcept        {}
    virtual void    startServer()                               noexcept        = 0;
    virtual void    stopServer()                                noexcept        = 0;
    void            close()                                     noexcept;
    //-----------------------------------------
    bool            sendRawMsg(const PacketMessage &packet_message) noexcept;
    virtual bool    sendRawMsg(const std::string& remote_ip, const uint16_t remote_port,
                            const Packet& packet)               noexcept        = 0;
    virtual PacketMessage recvRawMsg(int timeout)               noexcept        = 0;
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    void            sendMsg(const std::string& remote_ip, const uint16_t remote_port,
                            const Packet& packet)               noexcept        { sendMsg(IpPort{remote_ip, remote_port}, packet); }
    void            sendMsg(const std::string& remote_ip, const uint16_t remote_port,
                            const Config& json)                 noexcept        { sendMsg(IpPort{remote_ip, remote_port}, json); }
    virtual void    sendMsg(const IpPort& remote_ip_port, const Packet& packet) = 0;
    virtual void    sendMsg(const IpPort& remote_ip_port, const Config& json)   = 0;
    //-----------------------------------------
    //Эти функции работают в связке с tick():
    //выдаст пустой пакет, если очередь пуста
    virtual PacketMessage   getOutPacket()                      noexcept        = 0;
    //выдаст пустой json, если очередь пуста
    virtual JsonMessage     getOutJson()                        noexcept        = 0;
    //=====================================
};

} // namespace simpleapi
