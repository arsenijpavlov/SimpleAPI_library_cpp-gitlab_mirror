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

    IpPort server{"127.0.0.1", 31116};
    SocketThread st(eUDP, server);

    Packet packet;
    packet = convert_to_packet("1Hello world!123");
    Json json;
    json.add("Hello", "WORLD!");

    bool isSent = st.send(server, {"127.0.0.5", 31115}, packet);
    std::cout << "Sent: " << (isSent ? "true" : "false") << std::endl;
    isSent = st.send(server, {"127.0.0.5", 31115}, json);
    std::cout << "Sent: " << (isSent ? "true" : "false") << std::endl;

    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }
        usleep(1);
    }

    return 0;
}
