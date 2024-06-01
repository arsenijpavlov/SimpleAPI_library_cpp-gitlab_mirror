#include "Socket.h"

#include <arpa/inet.h>
#include <sys/socket.h>

//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/un.h>
//#include <errno.h>
//#include <list>
//#include <assert.h>
//#include <stdint.h>
//#include <stdio.h>

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
    inet_pton(AF_INET, localIP.c_str(), &sock.sin_addr.s_addr);
    int res = bind(mSocketFD, (struct sockaddr*)&sock, sizeof(sock));
    if(res < 0)
        perror(std::string("bind() failed with localIP(" + localIP + ")"
                           + ", port(" + std::to_string(localPort) + ")").c_str());
}


