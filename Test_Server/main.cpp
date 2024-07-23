#include <SimpleAPI.h>

#include <csignal>
#include <unistd.h>
#include <iostream>

#define MAIN_COLOR logs::eCYAN_FG
#define NAME_COLUMN_SIZE 16
#define NAME_COLUMN_RIGHT_ALIGN true

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT) {
        std::cout << "\n";
        isRunning = false;
    }
}

void RecvData(PacketMessage pm) {
    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[SERVER]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << "recv data: 0x" << utils::to_hex_string(pm.packet)
              << std::endl;
}
bool isBigPacketSent = false;
bool isNeedAck = false;
Json jsonAck;
IpPort ipPortAck;
void RecvJson(JsonMessage jm) {
    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[SERVER]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << logs::to_color_string({logs::COLOR::eGREEN_BG, logs::COLOR::eWHITE_FG},
                                       "recv json: " + jm.to_string())
              << std::endl;

    if(!isBigPacketSent) {
        isNeedAck = true;
        jsonAck.clear();
        jsonAck.put("Chapter #1", "Some test text... Some test text... Some test text... Some test text... Some test text...");
        ipPortAck = jm.ipPort;

        isBigPacketSent = true;
    }
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
    SocketSettings settings;
    settings.setRecvPacketCallback(RecvData);
    settings.setRecvJsonCallback(RecvJson);
    settings.setColorLogCallback(Log);
    settings.setColorLogErrorCallback(LogError);
    settings.setNameColumnSize(NAME_COLUMN_SIZE);
    settings.enableNameColumnRightAlign(NAME_COLUMN_RIGHT_ALIGN);
    settings.setLogLevel(logs::eDEBUG);
    settings.enableLogTime(true);
    settings.enablePrintLogLevel(false);

    SocketThread st(eUDP, server, settings);
    st.m_settings.setLogLevel(logs::eINFO);
    st.m_settings.enableLogTime(true);
    st.m_settings.enablePrintLogLevel(false);
    st.m_settings.setNameColumnSize(NAME_COLUMN_SIZE);
    st.m_settings.enableNameColumnRightAlign(NAME_COLUMN_RIGHT_ALIGN);
//    st.m_settings.setLogCallback(Log);
    st.m_settings.setColorLogCallback(Log);

    st.startThread();
    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }

        if(isNeedAck) {
            st.send(server, ipPortAck, jsonAck);
            isNeedAck = false;
        }

        usleep(1);
    }

    return 0;
}
