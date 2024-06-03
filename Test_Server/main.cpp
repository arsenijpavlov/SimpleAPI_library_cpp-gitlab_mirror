#include "SimpleAPI.h"

#include <csignal>
#include <unistd.h>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    UDPSocket socket(31115, "127.0.0.1");

    while(isRunning) {
        usleep(1);
        socket.recvMsg();
    }

    return 0;
}
