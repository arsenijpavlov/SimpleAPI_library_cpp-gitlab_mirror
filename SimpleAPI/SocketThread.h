#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"
//TODO: add "noexcept"

class SocketThread {
    std::thread     m_thread;
    bool            m_active;
    SocketSettings  m_common_socket_settings;
    std::map<IpPort, std::shared_ptr<Socket>> m_sockets;
public:
    LoggerSettings  m_settings;

private:
    void run();
    void log(const logs::LEVEL level, const std::string log_message, const std::string color_log_message = "");

public:
    SocketThread() :
        m_active(false),
        m_settings(SocketSettings()),
        m_common_socket_settings(SocketSettings())  {}
    SocketThread(const SocketType type,
                 const uint16_t local_port,
                 const std::string& local_ip = "",
                 const SocketSettings settings = SocketSettings()) : m_active(false)
                                                    { addSocket(type, local_port, local_ip, settings); }
    SocketThread(const SocketType type, const IpPort& local_ip_port,
                 const SocketSettings settings = SocketSettings()) : m_active(false)
                                                    { addSocket(type, local_ip_port, settings); }
    ~SocketThread()                                 { stopThread(); }

    bool addSocket(const SocketType type, const uint16_t local_port,
                   const std::string& local_ip = "",
                   const SocketSettings settings = SocketSettings())
                                                    { return addSocket(type, IpPort{local_ip, local_port}, settings); }
    bool addSocket(const SocketType type, const IpPort& local_ip_port,
                   const SocketSettings settings = SocketSettings());
    bool addSocket(const SocketType type, const uint16_t local_port, const std::string local_ip = "",
                   const bool common_settings = false);
    bool addSocket(const SocketType type, const IpPort& local_ip_port, bool common_settings = false);

    void closeSocket(const IpPort& local_ip_port)   { m_sockets.erase(local_ip_port); }
    void closeAllSockets()                          { m_sockets.erase(m_sockets.begin(), m_sockets.cend()); }

    void startSocket(const IpPort& local_ip_port);
    void stopSocket(const IpPort& local_ip_port);

    void send(const IpPort& source, const IpPort& destination, const Packet& packet);
    void send(const IpPort& source, const IpPort& destination, const Json& json);

    bool isActive()                                 { return m_active; }
    void startThread();
    void stopThread();  //вызывается в деструкторе, останавливает поток

    //NOTE: nullptr если не найден или создать новый сервер во избежание ошибок?
    std::shared_ptr<Socket> findSocket(const IpPort& local_ip_port);

    //общие настройки для всех сокетов
    void setAllSocketsSettings(const SocketSettings settings = SocketSettings());
    //индивидуальные настройки для каждого сокета отдельно
    void setSocketsSettings(const IpPort& local_ip_port, const SocketSettings settings = SocketSettings());
};

#endif // SOCKET_THREAD_H
