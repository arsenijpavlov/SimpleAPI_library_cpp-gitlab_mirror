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

    UDPSocket socket(31116);

    Packet packet = to_packet("Hello world!");
    socket.sendMsg("127.0.0.1", 31115, packet);

    return 0;
}
