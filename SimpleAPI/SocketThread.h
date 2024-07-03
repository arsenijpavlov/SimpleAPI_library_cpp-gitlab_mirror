#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread t;
    bool        active;
    std::map<IpPort, std::shared_ptr<Socket>> p_sockets;

    void (*packetCallback)(PacketMessage pm);
    void (*jsonCallback)(JsonMessage jm);

    void run();

public:
    SocketThread();
    SocketThread(const SocketType type, const std::string& localIP, uint16_t localPort);
    SocketThread(const SocketType type, const IpPort& localIpPort);
    ~SocketThread();

    bool addSocket(const SocketType type, const std::string& localIP, const uint16_t localPort);
    bool addSocket(const SocketType type, const IpPort& localIpPort);

    void closeSocket(const IpPort& localIpPort);
    void closeAllSockets();

    void startSocket(const IpPort& localIpPort);
    void stopSocket(const IpPort& localIpPort);

    bool send(const IpPort& source, const IpPort& destination, const Packet& packet);
    bool send(const IpPort& source, const IpPort& destination, const Json& json);

    bool isActive();
    void startThread(); //вызывается в конструкторе, запускает поток
    void stopThread();  //вызывается в деструкторе, останавливает поток

    //NOTE: nullptr если не найден или создать новый сервер во избежание ошибок?
    std::shared_ptr<Socket> findSocket(const IpPort& localIpPort);

    void setCallbackSocketReadRawData(const IpPort& ipPort, void (*callback)(PacketMessage pm));
    void setCallbackSocketReadJsonData(const IpPort& ipPort, void (*callback)(JsonMessage jm));
};

#endif // SOCKET_THREAD_H
