#ifndef SOCKET_H
#define SOCKET_H

#include "EECounter.h"
#include "IpPort.h"
#include "Json.h"
#include "SocketSettings.h"

#include <deque>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <set>

#include <arpa/inet.h>
#include <sys/socket.h>


#define MAX_PACKET_LENGTH 65535

using time_point_default = std::chrono::system_clock::time_point;


enum SocketType {
    eUDP,
    eTCP
};
std::string to_string(SocketType type);


class Connection {
public:
    time_point_default  m_last_ping_time;
    time_point_default  m_last_activity;
    EECounter           m_out_sn;
    EECounter           m_in_sn_last_recv; //влияет на границу окна ожидания фрагментов
    EECounter           m_in_next_sn;

    std::map<EECounter, PacketMessage> m_map_recv_fragments;        //фрагменты сообщений (в беспорядке)
    std::map<EECounter, PacketMessage> m_map_recv_builded_messages;  //собранные по очереди фрагменты сообщений

    Connection() :
        m_last_ping_time(std::chrono::system_clock::now()),
        m_last_activity(std::chrono::system_clock::now()),
        m_out_sn(255),
        m_in_sn_last_recv(255),
        m_in_next_sn(255)
    {}
};

class Socket {
protected:
    using MapConnectionsIterator = std::map<IpPort, Connection>::iterator;

    int         m_socket_fd;
    SocketType  m_socket_type;
    std::string m_local_ip;
    uint16_t    m_local_port;
    std::map<IpPort, Connection> m_map_connections; //счётчики сообщений на отправку

public:
    SocketSettings  m_settings;

protected:
    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    //для доступа извне------------------------
    std::mutex                  m_output_threads_mutex;
    std::deque<PacketMessage>   m_send_packets_buffer;      //sendFragments(), sendAutoMsg()
    std::vector<PacketMessage>  m_sent_global_packets;      //запоминаем до тех пор, пока не придёт подтверждение о передаче всех фрагментов

    std::mutex                  m_input_threads_mutex;
    //собранные пакеты
    std::deque<PacketMessage>   m_map_recv_packets_buffer;  //buildPackets(), getOutPacket()
    std::deque<JsonMessage>     m_map_recv_jsons_buffer;    //buildPackets(), getOutJson()
    //-----------------------------------------

    bool            checkCorrectIp(const std::string& ip_string);

    uint8_t         packHeader(const PacketHeader& ph);
    PacketHeader    unpackHeader(const uint8_t header);
    EECounter&      getOutSeqNumber(const IpPort& ip_port);
    void            appendNewFragment(const PacketMessage& received_pm);
    PacketMessage   buildPacket(MapConnectionsIterator& it);
    void            updateLastOutputActivityTime(const IpPort& remote_ip_port);

    void            log(const logs::LEVEL level, const std::string log_message, const std::string color_log_message = "");
    void            setSettings(const SocketSettings settings = SocketSettings())
                    { m_settings = settings; }
    SocketSettings  getSettings()                       { return m_settings; }

    void            sendFragments(const std::string& remote_ip, const uint16_t remote_port,
                                  const PacketType type, const Packet& packet, const bool need_ack = true)
                    { sendFragments(IpPort{remote_ip, remote_port}, type, packet, need_ack); }
    virtual void    sendFragments(const IpPort& remote_ip_port, const PacketType type,
                                  const Packet& packet, const bool need_ack = true) = 0;

    virtual void    tick() = 0;
    virtual void    sendAutoMsg() = 0;
    virtual Json    recvAutoMsg(int timeout) = 0;
    virtual Json    processingBuiltPacket(const PacketMessage& pm) = 0;
    //=====================================
    friend class SocketThread; //для функции tick()

public:
                    Socket() :
                        m_socket_fd(-1),
                        m_settings(SocketSettings())    {}
    virtual         ~Socket()                           {}

    //-----------------------------------------
    void            setLogLevel(logs::LEVEL log_level)  { m_settings.setLogLevel(log_level); }
    IpPort          getLocalIpPort()                    { return IpPort{m_local_ip, m_local_port}; }
    bool            isChiphering()                      { /*TODO: isChiphering()*/ return false; }
    virtual bool    isConnected(const IpPort& remote_ip_port) = 0;
    bool            isServerActive()                    { return m_socket_fd > 0; }
    //-----------------------------------------
    void            chiphering(Packet& packet)          {}
    void            dechiphering(Packet& packet)        {}
    virtual void    startServer() = 0;
    virtual void    stopServer() = 0;
    void            close();
    //-----------------------------------------
    bool            sendRawMsg(const PacketMessage &packet_message);
    virtual bool    sendRawMsg(const std::string& remote_ip, const uint16_t remote_port, const Packet& packet) = 0;
    virtual PacketMessage recvRawMsg(int timeout) = 0;
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    bool            sendMsg(const std::string& remote_ip, const uint16_t remote_port, const Packet& packet);
    bool            sendMsg(const std::string& remote_ip, const uint16_t remote_port, const Json& json);
    virtual bool    sendMsg(const IpPort& remote_ip_port, const Packet& packet) = 0;
    virtual bool    sendMsg(const IpPort& remote_ip_port, const Json& json) = 0;
    //-----------------------------------------
    //Эти функции работают в связке с tick()
    virtual PacketMessage   getOutPacket() = 0; //выдаст пустой пакет, если очередь пуста
    virtual JsonMessage     getOutJson() = 0;   //выдаст пустой json, если очередь пуста
    //=====================================
};

class UDPSocket : public Socket {
    //работа через tick()
    std::map<time_point_default, PacketMessage> m_map_auto_sent_packets; //только UDP, уже отправленные фрагменты

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    /* принятый пакет делится на части, пришиваются необходимые заголовки
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void            sendFragments(const IpPort& remote_ip_port, const PacketType type,
                                  const Packet& packet, const bool need_ack = true);

    void            tick();
    void            checkConnections();//только UDP, проверка коннекта
    void            sendAutoMsg();
    Json            recvAutoMsg(int timeout);
    Json            processingBuiltPacket(const PacketMessage& pm);
    //=====================================

public:
    UDPSocket(const IpPort& local_ip_port, const SocketSettings& settings = SocketSettings());
    UDPSocket(const uint16_t local_port, const std::string& local_ip = "", const SocketSettings& settings = SocketSettings());
    ~UDPSocket()    { close(); }
    void            open(const uint16_t local_port, const std::string& local_ip = "");

    //-----------------------------------------
    bool            isConnected(const IpPort& remote_ip_port);
    //-----------------------------------------
    void            startServer();
    void            stopServer();
    //-----------------------------------------
    bool            sendRawMsg(const std::string& remote_ip, const uint16_t remote_port, const Packet& packet);
    PacketMessage   recvRawMsg(int timeout = -1);
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    void            setDeliveryNeed(bool enabled = true); //только UDP

    bool            sendMsg(const IpPort& remote_ip_port, const Packet& packet);
    bool            sendMsg(const IpPort& remote_ip_port, const Json& json);

    PacketMessage   getOutPacket();
    JsonMessage     getOutJson();
    //=====================================
};

//class TCPSocket : public Socket
//{
//public:
//    //crc, localIP, localPort, remoteIP, remotePort
//    TCPSocket(CRC crc = eCRC_OFF);
//    ~TCPSocket(){};
//};

//class TCPServer : public TCPSocket
//{
//public:
//    ~TCPServer(){};
//};

#endif // SOCKET_H
