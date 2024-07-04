#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread t;
    bool        active;
    std::map<IpPort, std::shared_ptr<Socket>> p_sockets;

//    void (*logOutputCallback)(std::string);//на будущее, может и понадобиться

    void run();

public:
    SocketThread();
    SocketThread(const SocketType type, const std::string& localIP, uint16_t localPort);
    SocketThread(const SocketType type, const IpPort& localIpPort);
    ~SocketThread();

    //TODO: общие callback функции при добавлении новых сокетов
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

    //общий логгер для всех сокетов
    void setCallbackAllSocketsReadRawData(void (*callback)(PacketMessage));
    void setCallbackAllSocketsReadJsonData(void (*callback)(JsonMessage));
    //индивидуальное для каждого сокета отдельно
    void setCallbackSocketReadRawData(const IpPort& localIpPort, void (*callback)(PacketMessage));
    void setCallbackSocketReadJsonData(const IpPort& localIpPort, void (*callback)(JsonMessage));

    //общий логгер для всех сокетов
    void setCallbackAllSocketsLogOutput(void (*callback)(std::string));
    void setCallbackAllSocketsLogErrorOutput(void (*callback)(std::string));
    //индивидуальный логгер для каждого сокета отдельно
    void setCallbackSocketLogOutput(const IpPort& localIpPort, void (*callback)(std::string));
    void setCallbackSocketLogErrOutput(const IpPort& localIpPort, void (*callback)(std::string));
};

#endif // SOCKET_THREAD_H
