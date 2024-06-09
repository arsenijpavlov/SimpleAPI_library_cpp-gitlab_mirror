#include <SimpleAPI.h>

#include <csignal>
#include <unistd.h>
#include <iostream>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    UDPSocket socket(31115, "127.0.0.5");

    while(1) {
        if(!isRunning) break;

        usleep(1);
        Packet p;
        socket.recvMsg(p, 100);
    }

    return 0;
}
