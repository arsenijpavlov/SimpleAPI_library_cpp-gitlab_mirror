#include <SimpleAPI.h>

#include <csignal>
#include <unistd.h>
#include <iostream>

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT)
        isRunning = false;
}

//void RecvData(PacketMessage pm) {
//}

//void RecvJson(JsonMessage jm) {
//}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);

    IpPort server{"127.0.0.15", 31115};
    SocketThread st(eUDP, server);
//    st.setCallbackSocketReadJsonData(server, RecvData);
//    st.setCallbackSocketReadRawData();

    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }
        usleep(1);
    }

    return 0;
}
