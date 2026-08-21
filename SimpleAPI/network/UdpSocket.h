#pragma once

#include "Socket.h"


namespace simpleapi {

class UDPSocket : public Socket {
    //работа через tick()
    std::map<time_point_default, PacketMessage> m_map_auto_sent_packets; //только UDP, уже отправленные фрагменты

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    /* принятый пакет делится на части, к ним пришиваются необходимые заголовки
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void            sendFragments(const IpPort& remote_ip_port, const PacketType type,
                       const Packet& packet, const bool need_ack = true) noexcept;

    void            tick()                                                          noexcept;
        //только UDP
    void            checkConnections()                                              noexcept;
    void            sendAutoMsg()                                                   noexcept;
    Config          recvAutoMsg(int timeout)                                        noexcept;
    Config          processingBuiltPacket(const PacketMessage& pm)                  noexcept;
    //=====================================

public:
    UDPSocket(const IpPort& local_ip_port,
              const SocketSettings& settings = SocketSettings())    noexcept;
    UDPSocket(const uint16_t local_port,
              const std::string& local_ip = "",
              const SocketSettings& settings = SocketSettings())    noexcept;
    ~UDPSocket()                                                    noexcept    { close(); }
    bool            open(const uint16_t local_port,
                         const std::string& local_ip = "")                          noexcept;

    //-----------------------------------------
    bool            isConnected(const IpPort& remote_ip_port)                       noexcept;
    //-----------------------------------------
    bool            startServer()                                                   noexcept;
    void            stopServer()                                                    noexcept;
    //-----------------------------------------
    bool            sendRawMsg(const std::string& remote_ip, const uint16_t remote_port,
                    const Packet& packet)                                noexcept;
    PacketMessage   recvRawMsg(int timeout = -1)                                    noexcept;
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    //только UDP
    void            setDeliveryNeed(bool enabled = true)                            noexcept;

    void            sendMsg(const IpPort& remote_ip_port, const Packet& packet);
    void            sendMsg(const IpPort& remote_ip_port, const Config& json);

    PacketMessage   getOutPacket()                                                  noexcept;
    JsonMessage     getOutJson()                                                    noexcept;
    //=====================================
};

} // namespace simpleapi
