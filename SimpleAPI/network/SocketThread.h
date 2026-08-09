#pragma once

#include <set>
#include <thread>
#include "Socket.h"
#include <mutex>


namespace simpleapi {

//TODO: закрыть доступ до фактических сокетов от пространства пользователя - всё через обёртку
class SocketThread {
    std::mutex      m_sockets_mutex;

    std::thread     m_thread;
    bool            m_active;
    SocketSettings  m_common_socket_settings;
    std::map<IpPort, std::shared_ptr<Socket>> m_sockets;
public:
    LoggerSettings  m_settings;

private:
    void run()                                                                          noexcept;
    void log(const logs::LEVEL level, std::string log_message)                          noexcept;

public:
    SocketThread()                                                                      noexcept
        : m_active(false)
        , m_settings(SocketSettings())
        , m_common_socket_settings(SocketSettings())
    {}

    SocketThread(const SocketType type,
                 const uint16_t local_port,
                 const std::string& local_ip = "",
                 const SocketSettings settings = SocketSettings())                      noexcept
        : m_active(false)
    { addSocket(type, local_port, local_ip, settings); }

    SocketThread(const SocketType type, const IpPort& local_ip_port,
                 const SocketSettings settings = SocketSettings())                      noexcept
        : m_active(false)
    { addSocket(type, local_ip_port, settings); }

    ~SocketThread()                                                                     noexcept { stopThread(); }

    bool addSocket(const SocketType type, const uint16_t local_port,
                   const std::string& local_ip = "",
                   const SocketSettings settings = SocketSettings())                    noexcept;
    bool addSocket(const SocketType type, const IpPort& local_ip_port,
                   const SocketSettings settings = SocketSettings())                    noexcept;
    bool addSocket(const SocketType type, const uint16_t local_port,
                   const std::string local_ip = "",
                   const bool common_settings = false)                                  noexcept;
    bool addSocket(const SocketType type, const IpPort& local_ip_port,
                   bool common_settings = false)                                        noexcept;

    void closeSocket(const IpPort& local_ip_port)                                       noexcept;
    void closeAllSockets()                                                              noexcept;

    void startSocket(const IpPort& local_ip_port)                                       noexcept;
    void stopSocket(const IpPort& local_ip_port)                                        noexcept;

    void send(const IpPort& source, const IpPort& destination, const Packet& packet)    noexcept;
    void send(const IpPort& source, const IpPort& destination, const Config& json)      noexcept;

    // методы для отправки в сокет данных без упаковки в протокол SimpleAPI
//TODO:    void sendRaw(const IpPort& source, const IpPort& destination, const Packet& packet)    noexcept;
//TODO:    void sendRaw(const IpPort& source, const IpPort& destination, const Config& json)      noexcept;
//TODO: на стороне приёма тоже должны быть такие нюансы

    bool isActive()                                                                     noexcept { return m_active; }
    void startThread()                                                                  noexcept;
    //вызывается в деструкторе, останавливает поток
    void stopThread()                                                                   noexcept;

private:
    //NOTE: nullptr если не найден или создать новый сервер во избежание ошибок?
    std::shared_ptr<Socket> findSocket(const IpPort& local_ip_port)                     noexcept;
public:

    //общие настройки для всех сокетов
    void setAllSocketsSettings(const SocketSettings settings = SocketSettings())        noexcept;
    //индивидуальные настройки для каждого сокета отдельно
    void setSocketsSettings(const IpPort& local_ip_port,
                            const SocketSettings settings = SocketSettings())           noexcept;
};

} // namespace simpleapi
