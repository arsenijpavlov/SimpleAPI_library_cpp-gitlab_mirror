#ifndef SOCKET_H
#define SOCKET_H

#include "Json.h"

#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>

/* Packets structure (actually for v.0):
 * [ API version (2) | length size (3) | CRC (3) | data... ]
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
    std::string localIP/*, remoteIP*/;
    uint16_t localPort/*, remotePort*/;

public:
    virtual ~Socket(){};

    virtual bool sendMsg(std::string remoteIP, uint16_t remotePort, Packet packet)      = 0;
    virtual bool sendMsg(std::string remoteIP, uint16_t remotePort, json::Json json)    = 0;
    virtual bool recvMsg()                                                              = 0;
    virtual bool recvMsgTimeout()                                                       = 0;

//TODO: CRC
//TODO: Chiphering
//    virtual void enableCRC(CRC crcLevel)    = 0;
//    virtual void enableChip()               = 0;

    void close(){ if(mSocketFD) shutdown(mSocketFD, SHUT_RDWR); }
};

class UDPSocket : public Socket {
public:
    UDPSocket(uint16_t localPort, std::string localIP = "0.0.0.0");
    ~UDPSocket() { close(); };

    void open(const uint16_t localPort, const std::string& localIP = "0.0.0.0");
//    void enableCRC(CRC crcLevel = eCRC_8);
//    void enableChip();

//TODO: передача сообщения по частям (свыше )
    bool sendMsg(std::string remoteIP, uint16_t remotePort, Packet packet);    //TYPE = 0
    bool sendMsg(std::string remoteIP, uint16_t remotePort, json::Json json);  //TYPE = 1
    bool recvMsg(); //raw bytes, SimpleAPI::packet
    bool recvMsgTimeout();

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
