#ifndef SOCKET_H
#define SOCKET_H

#include "EECounter.h"
#include "IpPort.h"
#include "Json.h"
#include "Message.h"

#include <deque>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <set>

#include <arpa/inet.h>
#include <sys/socket.h>



#define MAX_PACKET_LENGTH 65535

using time_point_default = std::chrono::time_point<
    std::chrono::system_clock,
    std::chrono::duration<long, std::ratio<1, 1000000000>>>;


bool checkCorrectIp(const std::string& ipString);

enum SocketType {
    eUDP,
    eTCP
};

struct ChannelSettings {
    CRC         crcLevel;
    uint16_t    maxLength;
    int         maxMsgsSentOnTick;
    ApiVersion  useApiVersion;
    uint32_t    inactivityTimer; //для проверки коннекта, перепосылки недоставленных сообщений и прочего
};

//TODO: (LOG) сделать флаг для возможности отключения/перенаправления сообщений от API
//TODO: (LOG) сделать внутреннюю функцию-логгер для API

struct Connection {
    EECounter outSn;
    EECounter inSnLastRecv; //влияет на границу окна ожидания фрагментов
    EECounter inNextSn;     //TODO: обнулить после обрыва соединения

    std::map<EECounter, PacketMessage> mapRecvFragments;        //фрагменты сообщений (в беспорядке)
    std::map<EECounter, PacketMessage> mapRecvBuildedMessages;  //собранные по очереди фрагменты сообщений
};

class Socket {
protected:
    int             mSocketFD;
    std::string     localIP;
    uint16_t        localPort;
    ChannelSettings settings;

    std::map<IpPort, Connection> mapConnections; //счётчики сообщений на отправкуa

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    //для доступа извне------------------------
    std::mutex                  outputThreadsMutex;
    std::deque<PacketMessage>   sendPacketsBuffer;  //sendFragments(), sendAutoMsg()
    std::vector<PacketMessage>  sentGlobalPackets;  //запоминаем до тех пор, пока не придёт подтверждение о передаче всех фрагментов

    std::mutex                  inputThreadsMutex;
    //собранные пакеты
    std::deque<PacketMessage>   mapRecvPacketsBuffer;   //buildPackets(), getOutPacket()
    std::deque<JsonMessage>     mapRecvJsonsBuffer;     //buildPackets(), getOutJson()
    //-----------------------------------------
//    std::deque<PacketMessage>   mapSendPackets;     //фрагменты на отправку
//    std::deque<PacketMessage>   mapRecvPackets;     //полученные фрагменты
//    PacketMessage               tmpRecvJsonPacket;  //не в map, потому что сборка Json произойдёт в recvAutoMsg()
    //-----------------------------------------

    uint8_t         packHeader(const PacketHeader& ph);
    PacketHeader    unpackHeader(uint8_t header);
    EECounter       getOutSeqNumber(const IpPort& ipPort);
    PacketMessage   buildPacket(PacketMessage receivedPM);

    void            sendFragments(const std::string& remoteIp, const uint16_t remotePort, const PacketType type, const Packet& packet);
    virtual void    sendFragments(const IpPort& remoteIpPort, const PacketType type, const Packet& packet) = 0;

    virtual void    tick() = 0;
    virtual void    sendAutoMsg() = 0;
    virtual void    recvAutoMsg(int timeout) = 0;
    //=====================================
    friend class SocketThread; //NOTE: для функции tick()

public:
            Socket();
    virtual ~Socket(){};

    //-----------------------------------------
    IpPort          getLocalIpPort() { return IpPort{localIP, localPort}; }
    bool            isChiphering() { /*TODO: isChiphering()*/ return false; }
//TODO:    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;
    bool            isServerActive();
    //-----------------------------------------
    void            chiphering(Packet& packet) {};
    void            dechiphering(Packet& packet) {};
    virtual void    startServer() = 0;
    virtual void    stopServer() = 0;
    void            close();
    //-----------------------------------------
    bool            sendRawMsg(const PacketMessage &packetMessage);
    virtual bool    sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) = 0;
    virtual PacketMessage recvRawMsg(int timeout) = 0;
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    void            setMaxLength(uint16_t newMaxSize);     //по умолчанию 1500 байт (установлено MTU)
    void            setUseApiVersion(ApiVersion version);  //не может быть больше актуальной
//    void            enableChip(/*ChiphgeringSettings*/);
    void            enableCRC(CRC crcLevel = eCRC_OFF);
    //-----------------------------------------
    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    bool            sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    bool            sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);
    virtual bool    sendMsg(const IpPort& remoteIpPort, const Packet& packet) = 0;
    virtual bool    sendMsg(const IpPort& remoteIpPort, const Json& json) = 0;
    //-----------------------------------------
    //Эти функции работают в связке с tick()
    virtual PacketMessage   getOutPacket() = 0; //выдаст пустой пакет, если очередь пуста
    virtual JsonMessage     getOutJson() = 0;   //выдаст пустой пакет, если очередь пуста
    //=====================================
};

class UDPSocket : public Socket {
    //работа через tick()
    std::map<IpPort, time_point_default>        mapLastActivity;    //только UDP, проверка коннекта
    std::map<time_point_default, PacketMessage> mapAutoSentPackets; //только UDP, уже отправленные фрагменты

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    /* принятый пакет делится на части, пришиваются необходимые заголовки
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void            sendFragments(const IpPort& remoteIpPort, const PacketType type, const Packet& packet);

    void            tick();
    void            sendAutoMsg();
    void            recvAutoMsg(int timeout);
    //=====================================

public:
    UDPSocket(const IpPort& ipPort);
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();
    void            open(const uint16_t localPort, const std::string& localIP = "");

    //-----------------------------------------
//TODO:    bool isConnected(std::string remoteIP, uint16_t remotePort); //TODO: постоянный пинг
    //-----------------------------------------
    void            startServer();
    void            stopServer();
    //-----------------------------------------
    bool            sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    PacketMessage   recvRawMsg(int timeout = -1);
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    void            setDeliveryNeed(bool enabled = true); //только UDP
    void            setInactivityTimer(int usec);
    void            setMaxMsgsSentOnTick(int count = -1); //-1 если все накопленные отправить разом

    bool            sendMsg(const IpPort& remoteIpPort, const Packet& packet);
    bool            sendMsg(const IpPort& remoteIpPort, const Json& json);

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
