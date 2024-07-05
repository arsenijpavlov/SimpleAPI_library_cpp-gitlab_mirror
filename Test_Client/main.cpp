#include <SimpleAPI.h>

#include <csignal>
#include <iostream>
#include <unistd.h>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

void Log(std::string msg) {
    std::cout << msg;
}

void LogError(std::string msg) {
    std::cout << msg;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    IpPort server{"127.0.0.15", 31116};
    SocketThread st(eUDP, server);
//    st.findSocket(server)->enableCRC(eCRC_32);
//    st.findSocket(server)->setMaxLength(5);
//    st.setCallbackAllSocketsLogOutput(Log);
//    st.setCallbackAllSocketsLogOutput(LogError);

    Packet packet;
    packet = convert_to_packet("Hello world!");
    Json json;
    json.add("Hello", "WORLD!");

    bool isSent = st.send(server, {"127.0.0.15", 31115}, packet);
//    std::cout << "Sent: " << (isSent ? "true" : "false") << std::endl;
//    isSent = st.send(server, {"127.0.0.15", 31115}, json);
//    std::cout << "Sent: " << (isSent ? "true" : "false") << std::endl;

    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }
        usleep(1);
    }

    return 0;
}
