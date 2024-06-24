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
        PacketMessage pm = socket.recvRawMsg(100);
        if(!pm.packet.empty()) {
            std::cout << "sender: " << pm.ip << ":" << pm.port << ", ";
            std::cout << "packet(" << pm.packet.size() << "):";
            std::cout << " [" << utils::to_hex_string(pm.packet) << "]" << std::endl;
        }
    }

    return 0;
}
