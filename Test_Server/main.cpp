#include <SimpleAPI.h>

#include <csignal>
#include <unistd.h>
#include <iostream>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

void RecvData(PacketMessage pm) {
    std::cout << "[SERVER] recv data: 0x" << utils::to_hex_string(pm.packet) << std::endl;
}
void RecvJson(JsonMessage jm) {
    std::cout << "[SERVER] recv json: " << jm.to_string() << std::endl;
}
void Log(std::string msg) {
    std::cout << msg;
}
void LogError(std::string msg) {
    std::cout << msg;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    IpPort server{"127.0.0.15", 31115};
    SocketThread st(eUDP, server, RecvData, RecvJson, Log, LogError, logs::eDEBUG);
//    st.setLogLevel(server, logs::eDEBUG);

//    st.setCallbackAllSocketsReadRawData(RecvData);
//    st.setCallbackAllSocketsReadJsonData(RecvJson);

//    st.setCallbackAllSocketsLogOutput(Log);
//    st.setCallbackAllSocketsLogOutput(LogError);

    st.startThread();
    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }
        usleep(1);
    }

    return 0;
}
