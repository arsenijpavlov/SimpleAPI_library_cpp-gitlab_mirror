#ifndef SOCKET_H
#define SOCKET_H

#include "Json.h"

#include <string>
#include <vector>

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

class Socket {
protected:
    int         mSocketFD;
    CRC         crcLevel;
    uint16_t    maxLength;
    ApiVersion  useApiVersion;
    std::map<std::string, SNumber> mapActiveConnections;

    std::string localIP;
    uint16_t    localPort;

    uint8_t packHeader(const PacketType type, const uint8_t version,
                       const bool isFirstFragment, const bool isChip, const CRC crcLevel);
    SNumber getSeqNumber(const struct sockaddr_in& sock);
    virtual bool sendFragments(const PacketType type, struct sockaddr_in& sock, const Packet& packet) = 0;

public:
    Socket();
    virtual ~Socket(){};

    virtual bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet)  = 0;
    virtual bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json)      = 0;
    virtual int recvMsg(Packet& packet, int timeout)                                                    = 0;

    bool isActive();
    bool isChiphering() { /*TODO: isChiphering()*/ return true; }
    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;

    void enableCRC(CRC crcLevel = eCRC_OFF);
//TODO: Chiphering
//    virtual void enableChip() = 0;

    //большие пакеты могут фрагментироваться по пути, что не работает для некоторых маршрутизаторов
    //по умолчанию 1500 байт (установлено MTU)
    void setMaxLength(uint16_t newMaxSize);
    void setUseApiVersion(ApiVersion version); //не может быть больше актуальной

    void close();
};

class UDPSocket : public Socket {
    //std::map<std::string(localIP:port), timer(lastActivity)> сonnected; //будет зависеть от таймера последнего сообщения

    bool sendFragments(const PacketType type, struct sockaddr_in& sock, const Packet& packet);
public:
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();;

//    void enableChip(settings);
//    void enableDeliveryMsg(); //для локального клиент-сервера не имеет смысла

    bool isConnected(std::string remoteIP, uint16_t remotePort) { /*TODO: isConnected()*/ return true; }

    void open(const uint16_t localPort, const std::string& localIP = "");
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    bool sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);
    int recvMsg(Packet& packet, const int timeout = -1);
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
