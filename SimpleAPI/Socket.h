#ifndef SOCKET_H
#define SOCKET_H

#include "Json.h"

#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>

/* Packets structure (actually for v.1):
 * [ API version (4) | CRC (4) | [CRC] | size (16) | data[size] ]
 *                                      \ CRC_CALCULATING_DATA /
*/

#define MAX_PACKET_LENGTH 65535
using Packet = std::vector<uint8_t>;
Packet to_packet(const std::string& str);
Packet to_packet(const char* str) { return to_packet(std::string(str)); }

enum CRC {
    eCRC_OFF,
    eCRC_8,
    eCRC_16,
    eCRC_32,
    eCRC_64
};

class Socket {
protected:
    int mSocketFD;
    CRC crc;
    std::string localIP;
    uint16_t localPort;

public:
    Socket() : mSocketFD(-1) {};
    virtual ~Socket(){};

    virtual int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet)      = 0;
    virtual int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const json::Json& json)    = 0;
    virtual int recvMsg(Packet& packet, int timeout)                                                      = 0;

    bool isBinded() { return mSocketFD > 0; };

//TODO: CRC
//TODO: Chiphering
//    virtual void enableCRC(CRC crcLevel)    = 0;
//    virtual void enableChip()               = 0;

    void close();
};

class UDPSocket : public Socket {
public:
    UDPSocket(uint16_t localPort, std::string localIP = "") {
        open(localPort, localIP);
    }
    ~UDPSocket() { close(); };

    void open(const uint16_t localPort, const std::string& localIP = "");
//    void enableCRC(CRC crcLevel = eCRC_8);
//    void enableChip();

//TODO: передача сообщения по частям (свыше 65535 байтов)
    int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);    //TYPE = 0
    int sendMsg(const std::string& remoteIP, const uint16_t remotePort, const json::Json& json);  //TYPE = 1
    int recvMsg(Packet& packet, const int timeout = -1); //raw bytes, SimpleAPI::packet

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
