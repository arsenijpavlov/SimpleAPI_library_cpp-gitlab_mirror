#ifndef SOCKET_H
#define SOCKET_H

#include "Json.h"

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

using IpPort = std::string; //"X.X.X.X:Y"
IpPort      toIpPort(std::string ip, uint16_t port);
std::string toIp(IpPort ipPort);
uint16_t    toPort(IpPort ipPort);

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

struct Connection {
    SNumber sn;
    //NOTE: если подтверждение не пришло и через 200 (sn [0-255]), пакет удаляется из очереди с...
    std::map<SNumber, Packet> sentPackets; //по приходе подтверждения пакет удаляется из этого списка
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
    SNumber getSeqNumber(const struct sockaddr_in& sock);
    virtual bool sendFragments(const PacketType type, struct sockaddr_in& sock, const Packet& packet)      = 0;

    virtual bool sendAutoMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) = 0;
    virtual bool sendAutoMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json)     = 0;
    virtual ReceivedPacket recvAutoMsg(int timeout)                                                        = 0;

public:
    Socket();
    virtual ~Socket(){};

    virtual bool sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet)  = 0;
    virtual int  recvRawMsg(Packet& packet, int timeout)                                                   = 0;

    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;

    bool isServerActive();

    void close();
//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
    bool isChiphering() { /*TODO: isChiphering()*/ return false; }

    //по умолчанию 1500 байт (установлено MTU)
    void setMaxLength(uint16_t newMaxSize);
    void setUseApiVersion(ApiVersion version); //не может быть больше актуальной

//    virtual void enableChip(/*ChiphgeringSettings*/) = 0;
    void enableCRC(CRC crcLevel = eCRC_OFF);

protected:
    virtual void tick() = 0;
//=====================================
};

class UDPSocket : public Socket {
    std::map<IpPort, std::time_t>           mapLastActivity;
    std::map<IpPort, std::vector<Packet>>   mapSendPackets;
    std::map<IpPort, std::vector<Packet>>   mapRecvPackets;

public:
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();;

    bool isConnected(std::string remoteIP, uint16_t remotePort) { /*TODO: isConnected()*/ return true; }

    void open(const uint16_t localPort, const std::string& localIP = "");
    bool sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    ReceivedPacket recvRawMsg(int timeout = -1);

//=====================================
//ONLY FOR USE IN SOCKET_THREAD!
protected:
    bool sendFragments(const PacketType type, struct sockaddr_in& sock, const Packet& packet);

    void tick() {};

public:
//    void enableChip(/*ChiphgeringSettings*/);
    void setDeliveryNeed(bool enabled = true);
    void setInactivityTimer(int usec);

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
