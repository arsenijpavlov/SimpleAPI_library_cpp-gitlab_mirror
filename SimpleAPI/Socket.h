#ifndef SOCKET_H
#define SOCKET_H

#include "EECounter.h"
#include "Json.h"

#include <deque>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

#include <arpa/inet.h>
#include <sys/socket.h>

/* ================================================================================================
 * Packets structure (actually for v.1):
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

//using IpPort = std::string; //"X.X.X.X:Y"
//IpPort      toIpPort(const std::string& ip, const uint16_t port);
//std::string toIp(const IpPort& ipPort);
//uint16_t    toPort(const IpPort& ipPort);

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
};

struct ReceivedPacket {
    std::string senderIp;
    uint16_t    senderPort;
    Packet      packet;
};
struct ReceivedJson {
    std::string senderIp;
    uint16_t    senderPort;
    Json        json;
};

struct Message {
    std::string remoteIP;
    uint16_t    remotePort;
    Packet      packet;
};


class Socket {
protected:
    int         mSocketFD;
    CRC         crcLevel;
    uint16_t    maxLength;
    ApiVersion  useApiVersion;
    std::map<std::string, SNumber> mapActiveConnections;

    std::string localIP;
    uint16_t    localPort;

    uint8_t packHeader(const PacketType type, const uint8_t version, const bool isFirstFragment,
                       const bool isChip, const CRC crcLevel);
    EECounter getSeqNumber(const std::string& remoteIP, const uint16_t remotePort);
    virtual void sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet) = 0;

    virtual void tick()                                 = 0;
    virtual void sendAutoMsg()                          = 0;
    virtual ReceivedPacket recvAutoMsg(int timeout)     = 0;
    virtual ReceivedJson recvJsonAutoMsg(int timeout)   = 0;

public:
    Socket();
    virtual ~Socket(){};

    virtual bool sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet)  = 0;
    virtual int  recvRawMsg(Packet& packet, int timeout)                                                   = 0;

//TODO:    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;
    bool isServerActive();

    void close();
//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
protected:

public:
    bool isChiphering() { /*TODO: isChiphering()*/ return false; }

    void setMaxLength(uint16_t newMaxSize);     //по умолчанию 1500 байт (установлено MTU)
    void setUseApiVersion(ApiVersion version);  //не может быть больше актуальной

//    virtual void enableChip(/*ChiphgeringSettings*/) = 0;
    void enableCRC(CRC crcLevel = eCRC_OFF);
//=====================================
};

class UDPSocket : public Socket {
    //работа через tick()
//    std::map<IpPort, std::time_t>   mapLastActivity;
    std::deque<Message>             mapSendPackets;
    std::deque<Message>             mapRecvPackets;

    //для доступа извне------------------------
    std::mutex          outputThreadsMutex;
    std::deque<Message> mapSendPacketsBuffer;
    //-----------------------------------------

public:
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();;

//TODO:    bool isConnected(std::string remoteIP, uint16_t remotePort);

    void open(const uint16_t localPort, const std::string& localIP = "");
    bool sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    //сборка пакетов из фрагментов
    ReceivedPacket recvRawMsg(int timeout = -1);

//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
protected:
    /* принятый пакет делится на части, пришиваются необходимые заголовки...
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void sendFragments(const std::string& remoteIP, const uint16_t remotePort, const PacketType type, const Packet& packet);

    void tick();
    void sendAutoMsg();
    ReceivedPacket recvAutoMsg(int timeout);
    ReceivedJson recvJsonAutoMsg(int timeout);


public:
//    void enableChip(/*ChiphgeringSettings*/);
    void setDeliveryNeed(bool enabled = true);
    void setInactivityTimer(int usec);

    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);
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
