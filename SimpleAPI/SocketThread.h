#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread* t;
    bool        active;
    std::set<std::shared_ptr<Socket*>> p_sockets;

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

    std::set<std::shared_ptr<Socket*>>::iterator find(const SocketType type, const std::string& localIp, const uint16_t localPort);
    std::set<std::shared_ptr<Socket*>>::iterator find(const SocketType type, const IpPort& localIpPort);

    void closeSocket(const std::set<std::shared_ptr<Socket*>>::iterator it);
    void closeAllSockets();

    void startSocket(const std::set<std::shared_ptr<Socket*>>::iterator it);
    void stopSocket(const std::set<std::shared_ptr<Socket*>>::iterator it);

    void send(const std::set<std::shared_ptr<Socket*>>::iterator it,
              const std::string& remoteIp, const uint16_t remotePort, const Packet& packet);
    void send(const std::set<std::shared_ptr<Socket*>>::iterator it,
              const std::string& remoteIp, const uint16_t remotePort, const Json& json);
    void send(const std::set<std::shared_ptr<Socket*>>::iterator it,
              const IpPort& remoteIpPort, const Packet& packet);
    void send(const std::set<std::shared_ptr<Socket*>>::iterator it,
              const IpPort& remoteIpPort, const Json& json);

    bool isActive();
    void startThread(); //вызывается в конструкторе, запускает поток
    void stopThread();  //вызывается в деструкторе, останавливает поток

    void setCallbackSocketReadRawData(const Socket& s, void (*callback)(PacketMessage pm));
    void setCallbackSocketReadJsonData(const Socket& s, void (*callback)(JsonMessage jm));
};

#endif // SOCKET_THREAD_H
