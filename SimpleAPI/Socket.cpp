#include "Socket.h"
#include <iostream>

#include <unistd.h>
//#include <netdb.h>
//#include <sys/types.h>
#include <sys/select.h>
//#include <sys/un.h>
#include <errno.h>
//#include <list>
//#include <assert.h>
//#include <stdint.h>
//#include <stdio.h>

Packet convert_to_packet(const std::string& str)
{
    Packet packet;
    packet.resize(str.size());
    std::copy(str.begin(), str.end(), packet.begin());

    return packet;
}
std::string to_string(const Packet& packet)
{
    return std::string((char*)packet.data(), packet.size());
}

void Socket::close() {
    if(mSocketFD) {
        ::close(this->mSocketFD);
        std::cout << "The socket has been freed" << std::endl;
        this->mSocketFD = -1;
    }
}

UDPSocket::UDPSocket(uint16_t localPort, std::string localIP) {
    open(localPort, localIP);
}

UDPSocket::~UDPSocket() { close(); }

void UDPSocket::open(const uint16_t localPort, const std::string& localIP) {
    // create
    mSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketFD < 0) {
        perror("socket() failed");
    }

    // bind
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(localPort);
    if(localIP.empty())
        sock.sin_addr.s_addr = INADDR_ANY;
    else {
        if(!inet_pton(AF_INET, localIP.c_str(), &sock.sin_addr.s_addr))
            std::cout << "inet_pton(): return ERROR" << std::endl;
    }
    int res = bind(mSocketFD, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0) {
        perror(std::string("bind() failed with localIP(" + localIP + ")"
                           + ", port(" + std::to_string(localPort) + ")").c_str());
        close();
        return;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr.s_addr), str, INET_ADDRSTRLEN);
    std::cout << "Socket binded at " << str << ":" << localPort << std::endl;
}

int UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) {
    if(!this->isActive()) return -1;

    struct sockaddr_in sock;

    Packet buf;
    //TODO: упаковка
    buf = packet;

    sock.sin_family = AF_INET;
    sock.sin_port = htons(remotePort);
    if(!inet_pton(AF_INET, remoteIP.c_str(), &sock.sin_addr.s_addr))
        std::cout << "inet_pton(): return ERROR" << std::endl;
    int res = sendto(mSocketFD, (char*)buf.data(), buf.size(), 0, (struct sockaddr*)&sock, sizeof(struct sockaddr_in));
    if(res == -1)
        std::cout << "ErrNo: " << errno << std::endl;
    return res;
}
int UDPSocket::sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json) {
    return sendMsg(remoteIP, remotePort, convert_to_packet(json.to_string(-1)));
}

int UDPSocket::recvMsg(Packet& packet, const int timeout) {
    if(!this->isActive()) return -1;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mSocketFD, &fds);
    struct timeval t;
    if(timeout > 0) {
        t.tv_sec    = timeout / 1000;
        t.tv_usec   = (timeout % 1000) * 1000;
    }
    char buf[MAX_PACKET_LENGTH];
    int recv_num;

    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    socklen_t socklen = sizeof(sock);
    recv_num = select(mSocketFD + 1, &fds, NULL, NULL, (timeout > 0 ? &t : NULL));
    if(recv_num < 0) {
        if(errno != EINTR) /* Interrupted system call */
//TODO: сделать флаг для возможности отключения/перенаправления сообщений от API
            std::cout << "Error in select(), errno=" << errno << std::endl;
        return -1;
    }
    if(recv_num > 0) {
        recv_num = recvfrom(mSocketFD, buf,
                            MAX_PACKET_LENGTH, /*flags*/0,
                            (struct sockaddr*)&sock, &socklen);
//        std::cout << "recv_num: " << recv_num << std::endl;
    }

    if(recv_num < 0) {
//        std::cout << "Error reading msg" << std::endl;
        return -1;
    } else if(recv_num > 0) {
        std::string remoteIP;
        remoteIP.resize(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sock.sin_addr), (char*)remoteIP.data(), INET_ADDRSTRLEN);
        std::cout << "ip:" << remoteIP << std::endl;
        std::cout << "port:" << ntohs(sock.sin_port) << std::endl;
        std::cout << "message: \"" << std::string(buf, recv_num) << "\"" << std::endl;
    }

    return recv_num;
}




