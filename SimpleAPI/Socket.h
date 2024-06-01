#ifndef SOCKET_H
#define SOCKET_H

#include <string>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

/* Packets structure (actually for v.0):
 * [ API version (2) | length size (3) | CRC (3) | data... ]
*/

enum CRC {
    eCRC_OFF,
    eCRC_8,
    eCRC_16,
    eCRC_32,
    eCRC_64
};

class Socket
{
protected:
    int mSocketFD;
    CRC crc;
    std::string localIP, remoteIP;
    uint16_t localPort, remotePort;

public:
    virtual ~Socket() = 0;
    void close() {
        if(mSocketFD)
            ::close(mSocketFD);
    }
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

class UDPSocket : protected Socket
{
    bool connected;
public:
    UDPSocket(uint16_t localPort, std::string localIP = "0.0.0.0")
    {
        this->crc = crc;
        open(localPort, localIP);
    }
    ~UDPSocket() { close(); }

    void open(const uint16_t localPort, const std::string& localIP = "0.0.0.0");

    void enableCRC(CRC crcLevel = eCRC_8);

    bool send(); //bytes, hex string, JSons
    bool read(); //raw bytes, SimpleAPI::packet
    bool recv();
    bool recvWithTimeout();

    bool isConnected();
};

#endif // SOCKET_H
