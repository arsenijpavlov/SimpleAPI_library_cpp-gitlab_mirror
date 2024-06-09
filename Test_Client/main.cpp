#include <SimpleAPI.h>

#include <csignal>
#include <iostream>
#include <unistd.h>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    UDPSocket socket(31116, "127.0.0.1");

    Packet packet;
//    packet = to_packet("Hello world!123");
//    int num = socket.sendMsg("127.0.0.5", 31115, packet);
    Json json;
    json.add("Hello", "WORLD!");
    int num = socket.sendMsg("127.0.0.5", 31115, json);
    std::cout << "Sended: " << num << std::endl;

    return 0;
}
