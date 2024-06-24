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

    IpPort server1{"127.0.0.1", 31116};
    SocketThread st(eUDP, server1);
//    UDPSocket socket(31116, "127.0.0.1");
//    socket.enableCRC(eCRC_32);

    Packet packet;
//    packet = to_packet("Hello world!123");
//    int num = socket.sendMsg("127.0.0.5", 31115, packet);
    Json json;
    json.add("Hello", "WORLD!");
//    bool isSent = socket.sendMsg("127.0.0.5", 31115, json);
    bool isSent = st.send(server1, {"127.0.0.5", 31115}, json);
    std::cout << "Sent: " << (isSent ? "true" : "false") << std::endl;

//    Packet p = convert_to_packet(json.to_string(-1));
//    std::cout << "packet(" << p.size() << "): [" << utils::to_hex_string(p) << "]" << std::endl;

    return 0;
}
