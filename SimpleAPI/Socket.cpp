#include "Socket.h"
#include <iostream>

//#include <unistd.h>
//#include <netdb.h>
//#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
//#include <sys/un.h>
//#include <errno.h>
//#include <list>
//#include <assert.h>
//#include <stdint.h>
//#include <stdio.h>

Packet to_packet(const std::string& str)
{
    Packet packet;
    for(char ch : str)
        packet.push_back(ch);
    return packet;
}

UDPSocket::UDPSocket(uint16_t localPort, std::string localIP) {
    open(localPort, localIP);
}

void UDPSocket::open(const uint16_t localPort, const std::string& localIP) {
    close();

    // create
    mSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketFD < 0) {
        perror("socket() failed");
    }

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(localPort);
    inet_pton(AF_INET, localIP.c_str(), &sock.sin_addr.s_addr);
    int res = bind(mSocketFD, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0)
        perror(std::string("bind() failed with localIP(" + localIP + ")"
                           + ", port(" + std::to_string(localPort) + ")").c_str());
}

bool UDPSocket::sendMsg(std::string remoteIP, uint16_t remotePort, Packet packet) {
    struct sockaddr_in sock;

    Packet buf;
    //TODO: упаковка
    buf = packet;

    sock.sin_port = htons(remotePort);
    inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr);
    int res = sendto(mSocketFD, buf.data(), buf.size(), 0, (struct sockaddr*)&sock, sizeof(sock));
    return res > 0;
}
bool UDPSocket::sendMsg(std::string remoteIP, uint16_t remotePort, json::Json json) {
    return true;
}

bool UDPSocket::recvMsg() {
    char buf[MAX_PACKET_LENGTH];
    struct sockaddr_in sock;
//    struct sockaddr *sock = struct sockaddr_in();
//    socklen_t socklen = sizeof(sock);
    int recv_num;
//    int recv_num = recvfrom(mSocketFD, buf, 10/*MAX_PACKET_LENGTH*/, /*flags*/MSG_PEEK, sock, &socklen);
    recv_num = recv(mSocketFD, buf, MAX_PACKET_LENGTH, /*flags*/0);
    if(recv_num < 0)
        std::cout << "Error reading msg" << std::endl;
    else if(recv_num > 0) {
        std::string remoteIP;
        remoteIP.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)remoteIP.data(), INET_ADDRSTRLEN);
        std::cout << "ip:" << remoteIP << std::endl;
        std::cout << "port:" << sock.sin_port << std::endl;
        std::cout << "message: \"" << std::string(buf, recv_num) << "\"" << std::endl;
    }

    return true;
}

bool UDPSocket::recvMsgTimeout() {
    return true;
}



