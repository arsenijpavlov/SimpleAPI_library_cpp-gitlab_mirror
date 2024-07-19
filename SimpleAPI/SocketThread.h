#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread     m_thread;
    bool            m_active;
    SocketSettings  m_common_settings;
    std::map<IpPort, std::shared_ptr<Socket>> m_sockets;

    void run();

public:
    SocketThread();
    SocketThread(const SocketType type, const uint16_t local_port,
                 const std::string& local_ip = "",
                 const SocketSettings settings = SocketSettings());
    SocketThread(const SocketType type, const IpPort& local_ip_port,
                 const SocketSettings settings = SocketSettings());
    ~SocketThread();

    bool addSocket(const SocketType type, const uint16_t local_port, const std::string& local_ip = "",
                   const SocketSettings settings = SocketSettings());
    bool addSocket(const SocketType type, const IpPort& local_ip_port,
                   const SocketSettings settings = SocketSettings());
    bool addSocket(const SocketType type, const uint16_t local_port, const std::string local_ip = "",
                   const bool common_settings = false);
    bool addSocket(const SocketType type, const IpPort& local_ip_port, bool common_settings = false);

    void closeSocket(const IpPort& local_ip_port);
    void closeAllSockets();

    void startSocket(const IpPort& local_ip_port);
    void stopSocket(const IpPort& local_ip_port);

    bool send(const IpPort& source, const IpPort& destination, const Packet& packet);
    bool send(const IpPort& source, const IpPort& destination, const Json& json);

    bool isActive();
    void startThread();
    void stopThread();  //вызывается в деструкторе, останавливает поток

    //NOTE: nullptr если не найден или создать новый сервер во избежание ошибок?
    std::shared_ptr<Socket> findSocket(const IpPort& local_ip_port);

    //общие настройки для всех сокетов
    void setAllSocketsSettings(const SocketSettings settings = SocketSettings());
    //индивидуальное для каждого сокета отдельно
    void setSocketsSettings(const IpPort& local_ip_port, const SocketSettings settings = SocketSettings());
};

#endif // SOCKET_THREAD_H
