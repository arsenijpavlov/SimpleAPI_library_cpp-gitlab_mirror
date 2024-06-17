#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread t; //поток, который просто крутит while(1) { foreach() { socket.tick(); } }
    std::set<Socket> sockets;
public:
    SocketThread();
    SocketThread(Socket& s);
    SocketThread(const SocketType type, const std::string& localIP, uint16_t localPort);
    SocketThread(const SocketType type, const IpPort& localIpPort); //string address as "X.X.X.X:Y"
    ~SocketThread();

    bool addSocket(Socket& s);
    bool addSocket(const std::string& localIP, const uint16_t localPort);
    bool addSocket(const IpPort& localIpPort); //string address as "X.X.X.X:Y"
    bool closeSocket(Socket& s);

    void send(Socket& s, const std::string& remoteIp, const uint16_t remotePort, const Packet& packet);
    void send(Socket& s, const std::string& remoteIp, const uint16_t remotePort, const Json& json);
    void send(Socket& s, const IpPort& remoteIpPort, const Packet& packet); //string address as "X.X.X.X:Y"
    void send(Socket& s, const IpPort& remoteIpPort, const Json& json);     //string address as "X.X.X.X:Y"

    ReceivedPacket setCallbackSocketReadRawData(const Socket& s, ReceivedPacket(*callback)(void));
    ReceivedJson setCallbackSocketReadJsonData(const Socket& s, ReceivedJson(*callback)(void));
};

#endif // SOCKET_THREAD_H
