#ifndef SOCKET_H
#define SOCKET_H

#include "EECounter.h"
#include "Json.h"

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

/* ================================================================================================
 * API v.1
 * ================================================================================================
 * Packets structure:
 * [ C/D/J (2) | API version (2) | Start Data (1) | Chiphering enable (1) | CRC level (2) | ...
 *       ... | sequence number (8) | CRC (if enabaled, X bytes) | size (16) | data[size] ]
 * ________________________________________________________________________________________________
 *  C/D/J               - тип пакета:
 *                          * Control, необходимый для работы сокета
 *                          * Data, сырые данные
 *                          * Json, JSON-формат текстового сообщения
 *  API version         - версия библиотеки, обратная совместимость обязательна
 *  Start Data          - флаг начала сообщения, необходим для корректного приёма
 *  Chiphering enabled  - флаг шифрования, параметры шифрования д/б отправлены контрольным пакетом
 *  CRC level           - формат checksum, используемый для проверки целостности пакета
 *                      (в основном необходимо для сборки больших пакетов)
 *  sequence number     - порядковый номер пакета, нужен для сборки больших сообщений и проверки
 *                       корректности доставки
 *  CRC                 - checksum полного сообщения, не дублируется для последующих частей
 *  size                - размер полного сообщения, не дублируется для последующих частей
 *  data                - данные
 * ==============================================================================================*/

#define MAX_PACKET_LENGTH 65535

using Packet = std::vector<uint8_t>;
using SNumber = uint8_t;
Packet convert_to_packet(const std::string& str);
Packet convert_to_packet(const char* str);
std::string convert_from_packet(const Packet& packet);
std::string to_string(const Packet& packet);
bool checkCorrectIp(const std::string& ipString);

enum PacketType {
    eControlType    = 0,
    eDataType       = 1,
    eJsonType       = 2
};

enum ApiVersion {
    Version_1   = 1
};

enum CRC {
    eCRC_OFF    = 0,
    eCRC_8      = 1,
    eCRC_16     = 2,
    eCRC_32     = 3
};

enum SocketType {
    eUDP,
    eTCP
};

struct IpPort {
    std::string ip;
    uint16_t port;

    const bool operator==(const IpPort& other);
    const bool operator!=(const IpPort& other);
    const bool operator<(const IpPort& other) const;
    const bool operator>(const IpPort& other) const;

    std::string to_string() { return "[" + this->ip + ":" + std::to_string(this->port) + "]"; }
};

class PacketMessage {
public:
    std::string ip;
    uint16_t    port;
    Packet      packet;
    EECounter   sn;

    PacketMessage() : sn(0) { clear(); };

    void clear() { ip=""; port=0; packet={}; sn.reset(); }
    std::string to_string();
};
class JsonMessage {
public:
    std::string ip;
    uint16_t    port;
    Json        json;

    void clear() { ip=""; port=0; json={}; }
    std::string to_string(int arg = -1);
};

struct PacketHeader {
    PacketType  type;
    ApiVersion  version;
    bool        isFirstFragment;
    bool        isChip;
    CRC         crcLevel;
};

class Socket {
protected:
    int         mSocketFD;
    CRC         crcLevel;
    uint16_t    maxLength;
    ApiVersion  useApiVersion;
    std::map<IpPort, EECounter> mapActiveConnections;

    std::string localIP;
    uint16_t    localPort;

    int maxMsgsSentOnTick;

    uint8_t         packHeader(const PacketHeader& ph);
    void            unpackHeader(uint8_t header, PacketHeader& ph);
    EECounter       getSeqNumber(const IpPort& ipPort);

public:
                    Socket();
    virtual         ~Socket(){};

    bool            sendRawMsg(const PacketMessage &packetMessage);
    virtual bool    sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) = 0;
    virtual PacketMessage recvRawMsg(int timeout) = 0;

    virtual void    chiphering(Packet& packet) = 0;

    IpPort          getLocalIpPort() { return IpPort{localIP, localPort}; }


//TODO:    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;
    bool            isServerActive();
    virtual void    startServer() = 0;
    virtual void    stopServer() = 0;
    void            close();
//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
protected:
    virtual void    sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet) = 0;

    //для доступа извне------------------------
    std::mutex                  outputThreadsMutex;
    std::deque<PacketMessage>   sendPacketsBuffer;
    std::deque<JsonMessage>     sendJsonsBuffer;

    std::mutex                  inputThreadsMutex;
    std::deque<PacketMessage>   recvPackets;
    std::deque<JsonMessage>     recvJsons;
    //-----------------------------------------
    virtual void    tick() = 0;
    virtual void    sendAutoMsg() = 0;
    virtual void    recvAutoMsg(int timeout) = 0;

public:
    bool isChiphering() { /*TODO: isChiphering()*/ return false; }

    void setMaxLength(uint16_t newMaxSize);     //по умолчанию 1500 байт (установлено MTU)
    void setUseApiVersion(ApiVersion version);  //не может быть больше актуальной

//    virtual void enableChip(/*ChiphgeringSettings*/) = 0;
    void enableCRC(CRC crcLevel = eCRC_OFF);

    virtual bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) = 0;
    virtual bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json) = 0;
    bool sendMsg(const IpPort& remoteIpPort, const Packet& packet);
    bool sendMsg(const IpPort& remoteIpPort, const Json& json);
    virtual PacketMessage getOutPacket() = 0;   //выдаст пустой пакет, если очередь пуста
    virtual JsonMessage getOutJson() = 0;       //выдаст пустой пакет, если очередь пуста
//=====================================
    friend class SocketThread; //NOTE: для функции tick(), надо будет подумать...
};

class UDPSocket : public Socket {
    //работа через tick()
    std::map<IpPort, std::time_t>           mapLastActivity;
    std::map<std::time_t, PacketMessage>    mapSendGlobalPackets; //запоминаем до тех пор, пока не придёт подтверждение о передаче всех фрагментов
    std::deque<PacketMessage>       mapSendPackets; //фрагменты на отправку
    std::vector<PacketMessage>      mapAutoSentPackets;
    std::deque<PacketMessage>       mapRecvPackets;
    PacketMessage tmpRecvJsonPacket; //не в map, потому что сборка Json произойдёт в recvAutoMsg()

    //для доступа извне------------------------
    std::mutex                  outputThreadsMutex;
    std::deque<PacketMessage>   mapSendPacketsBuffer;
    std::deque<JsonMessage>     mapSendJsonsBuffer;

    std::mutex                  inputThreadsMutex;
    std::deque<PacketMessage>   mapRecvPacketsBuffer;
    std::deque<JsonMessage>     mapRecvJsonsBuffer;
    //-----------------------------------------

public:
    UDPSocket(const IpPort& ipPort);
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();

//TODO:    bool isConnected(std::string remoteIP, uint16_t remotePort);

    void open(const uint16_t localPort, const std::string& localIP = "");
    bool sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    //сборка пакетов из фрагментов
    PacketMessage recvRawMsg(int timeout = -1);
    void chiphering(Packet& packet) {};

    void startServer();
    void stopServer();

//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
protected:
    /* принятый пакет делится на части, пришиваются необходимые заголовки...
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet);

    void tick();
    void sendAutoMsg();
    void recvAutoMsg(int timeout);

public:
//    void enableChip(/*ChiphgeringSettings*/);
    void setDeliveryNeed(bool enabled = true);
    void setInactivityTimer(int usec);
    void setMaxMsgsSentOnTick(int count = -1); //-1 если все накопленные отправить разом

    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);
    PacketMessage getOutPacket();
    JsonMessage getOutJson();
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
