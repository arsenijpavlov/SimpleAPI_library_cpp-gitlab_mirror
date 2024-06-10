#ifndef SOCKET_H
#define SOCKET_H

#include "Json.h"

#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>

/* Packets structure (actually for v.1):
 * [ C/D/J (2) | API version (3) | Chiphering enable (1) | CRC level (2) | ...
 *       ... | sequence number (8) | CRC (if enabaled, X bytes) | size (16) | data[size] ]
 *
 *  C/D/J               - тип пакета:
 *                          * Control, необходимый для работы сокета
 *                          * Data, сырые данные
 *                          * Json, JSON-формат текстового сообщения
 *  API version         - версия библиотеки, обратная совместимость обязательна
 *  Chiphering enabled  - флаг шифрования, параметры шифрования д/б отправлены контрольным пакетом
 *  CRC level           - формат checksum, используемый для проверки целостности пакета
 *                      (в основном необходимо для сборки больших пакетов)
 *  sequence number     - порядковый номер пакета, нужен для сборки больших сообщений и проверки
 *                       корректности доставки
 *  CRC                 - checksum
 *  size                - размер полного сообщения, не дублируется для последующих частей
 *  data                - данные
*/

#define MAX_PACKET_LENGTH 65535
using Packet = std::vector<uint8_t>;
Packet convert_to_packet(const std::string& str);
Packet convert_to_packet(const char* str) { return convert_to_packet(std::string(str)); }

enum CRC {
    eCRC_OFF,
    eCRC_8,
    eCRC_16,
    eCRC_32
};

class Socket {
protected:
    int         mSocketFD;
    CRC         crcLevel;
    uint16_t    maxLength;
    std::string localIP;
    uint16_t    localPort;

public:
    Socket() : mSocketFD(-1), maxLength(1500) {};
    virtual ~Socket(){};

    virtual int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet)   = 0;
    virtual int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json)       = 0;
    virtual int recvMsg(Packet& packet, int timeout)                                                    = 0;

    bool isActive();;

//TODO: CRC checking
    void enableCRC(CRC crcLevel = eCRC_OFF) { this->crcLevel = crcLevel; }
//TODO: Chiphering
//    virtual void enableChip()               = 0;

    //большие пакеты могут фрагментироваться по пути, что не работает для некоторых маршрутизаторов
    //по умолчанию 1500 байт (установлено MTU)
    void setMaxLength(uint16_t newMaxSize) { maxLength = newMaxSize; }

    void close();
};

class UDPSocket : public Socket {
public:
    UDPSocket(uint16_t localPort, std::string localIP = "");
    ~UDPSocket();;

//    void enableChip();
//    void enableDeliveryMsg();


    void open(const uint16_t localPort, const std::string& localIP = "");
//TODO: передача сообщения по частям (свыше this->maxLength)
    int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet); //TYPE = 0
    int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);     //TYPE = 1
    int recvMsg(Packet& packet, const int timeout = -1);

//    bool isConnected();
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
