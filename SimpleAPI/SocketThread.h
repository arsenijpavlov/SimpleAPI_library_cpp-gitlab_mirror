#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread t;
    bool        active;
    std::map<IpPort, std::shared_ptr<Socket>> p_sockets;

    void (*common_SocketsReadRawDataCallback)(PacketMessage);
    void (*common_SocketsReadJsonDataCallback)(JsonMessage);

    void (*common_LogOutputCallback)(std::string);
    void (*common_LogErrorOutputCallback)(std::string);
    logs::LEVEL common_logLevel;
    logs::LEVEL common_logErrorLevel;

    void run();

public:
    SocketThread();
    SocketThread(const SocketType type, const uint16_t localPort,
                 const std::string& localIP = "",
                 void (*callbackRecvPacket)(PacketMessage)  = nullptr,
                 void (*callbackRecvJson)(JsonMessage)      = nullptr,
                 void (*callbackLog)(std::string)           = nullptr,
                 void (*callbackLogError)(std::string)      = nullptr,
                 const logs::LEVEL logLevel                 = logs::eINFO,
                 const logs::LEVEL logErrorLevel            = logs::eERROR);
    SocketThread(const SocketType type, const IpPort& localIpPort,
                 void (*callbackRecvPacket)(PacketMessage)  = nullptr,
                 void (*callbackRecvJson)(JsonMessage)      = nullptr,
                 void (*callbackLog)(std::string)           = nullptr,
                 void (*callbackLogError)(std::string)      = nullptr,
                 const logs::LEVEL logLevel                 = logs::eINFO,
                 const logs::LEVEL logErrorLevel            = logs::eERROR);
    ~SocketThread();

    bool addSocket(const SocketType type, const uint16_t localPort,
                   const std::string& localIP = "",
                   void (*callbackRecvPacket)(PacketMessage)= nullptr,
                   void (*callbackRecvJson)(JsonMessage)    = nullptr,
                   void (*callbackLog)(std::string)         = nullptr,
                   void (*callbackLogError)(std::string)    = nullptr,
                   const logs::LEVEL logLevel               = logs::eINFO,
                   const logs::LEVEL logErrorLevel          = logs::eERROR);
    bool addSocket(const SocketType type, const IpPort& localIpPort,
                   void (*callbackRecvPacket)(PacketMessage)= nullptr,
                   void (*callbackRecvJson)(JsonMessage)    = nullptr,
                   void (*callbackLog)(std::string)         = nullptr,
                   void (*callbackLogError)(std::string)    = nullptr,
                   const logs::LEVEL logLevel               = logs::eINFO,
                   const logs::LEVEL logErrorLevel          = logs::eERROR);
    bool addSocket(const SocketType type, const uint16_t localPort,
                   const std::string localIP = "", bool commonSettings = false);
    bool addSocket(const SocketType type, const IpPort& localIpPort,
                   bool commonSettings = false);

    void closeSocket(const IpPort& localIpPort);
    void closeAllSockets();

    void startSocket(const IpPort& localIpPort);
    void stopSocket(const IpPort& localIpPort);

    bool send(const IpPort& source, const IpPort& destination, const Packet& packet);
    bool send(const IpPort& source, const IpPort& destination, const Json& json);

    bool isActive();
    void startThread();
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
    void setCallbackAllSocketsLogOutput(void (*callback)(std::string),
                                        const logs::LEVEL logLevel = logs::eINFO);
    void setCallbackAllSocketsLogErrorOutput(void (*callback)(std::string),
                                             const logs::LEVEL logLevel = logs::eWARNING);
    void setAllLogLevel(logs::LEVEL logLevel);      //INFO, DEBUG
    void setAllLogErrorLevel(logs::LEVEL logLevel); //ERROR, WARNING
    //индивидуальный логгер для каждого сокета отдельно
    void setCallbackSocketLogOutput(const IpPort& localIpPort, void (*callback)(std::string),
                                    const logs::LEVEL logLevel = logs::eINFO);
    void setCallbackSocketLogErrOutput(const IpPort& localIpPort, void (*callback)(std::string),
                                       const logs::LEVEL logLevel = logs::eWARNING);
    void setLogLevel(const IpPort& localIpPort, logs::LEVEL logLevel);
    void setLogErrorLevel(const IpPort& localIpPort, logs::LEVEL logLevel);
};

#endif // SOCKET_THREAD_H
