#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <set>
#include <thread>
#include "Socket.h"


class SocketThread {
    std::thread t; //поток, который просто крутит while(1) { readMsg(socket); }
    std::set<Socket> sockets;
public:
    SocketThread();
    SocketThread(Socket& s);
    SocketThread(SocketType type, std::string localIP, uint16_t localPort);
    SocketThread(SocketType type, IpPort localIpPort); //string address as "X.X.X.X:Y"
    ~SocketThread();

    bool addSocket(Socket& s);
    bool addSocket(std::string localIP, uint16_t localPort);
    bool addSocket(IpPort localIpPort); //string address as "X.X.X.X:Y"
    bool closeSocket(Socket& s);

    void send(Socket& s, const std::string remoteIp, const uint16_t remotePort, const Packet& packet);
    void send(Socket& s, const std::string remoteIp, const uint16_t remotePort, const Json& json);
    void send(Socket& s, const std::string remoteIpPort, const Packet& packet); //string address as "X.X.X.X:Y"
    void send(Socket& s, const std::string remoteIpPort, const Json& json);     //string address as "X.X.X.X:Y"

    ReceivedPacket setCallbackSocketReadRawData(const Socket& s, ReceivedPacket(*callback)(void))
    {
        if(callback != nullptr) return callback();
        else                    return {};
    }
    ReceivedJson setCallbackSocketReadJsonData(const Socket& s, ReceivedJson(*callback)(void))
    {
        if(callback != nullptr) return callback();
        else                    return {};
    }
};

#endif // SOCKET_THREAD_H
