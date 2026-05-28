#include <SimpleAPI.h>

#include <csignal>
#include <iostream>
#include <regex>
#include <unistd.h>

#define MAIN_COLOR logs::eCYAN_FG
#define NAME_COLUMN_SIZE 16
#define NAME_COLUMN_RIGHT_ALIGN true

using namespace simpleapi;

logs::LEVEL common_log_level = logs::eDEBUG;
void ParseParameters(int argc, char** argv) {
    for(int i = 1; i < argc; i++) {
        std::string str = argv[i];
        std::smatch match_log_level;
        if(std::regex_search(str, match_log_level, std::regex("log_level=([0-9]+)"))) {
            common_log_level = static_cast<logs::LEVEL>(std::stoi(match_log_level.str(1)));
            std::cout << "set LOG LEVEL to " << to_string(common_log_level) << std::endl;
        }
    }
}

bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT) {
        std::cout << "\n";
        isRunning = false;
    }
}

void RecvData(PacketMessage pm) {
    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[CLIENT]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << "recv data: 0x" << utils::ToHexString(pm.m_packet)
              << std::endl;
}
void RecvJson(JsonMessage jm) {
    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[CLIENT]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << logs::to_color_string({logs::COLOR::eGREEN_BG, logs::COLOR::eWHITE_FG},
                                       "recv json: " + jm.toString())
              << std::endl;
}
void Log(std::string msg) {
    std::cout << msg;
}
void LogError(std::string msg) {
    std::cout << msg;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);
    ParseParameters(argc, argv);

    IpPort server{"127.0.0.15", 31116};
    SocketSettings settings;
    settings.setRecvPacketCallback(RecvData);
    settings.setRecvJsonCallback(RecvJson);
    settings.setColorLogCallback(Log);
    settings.setColorLogErrorCallback(LogError);
    settings.enableLogTime(true);
    settings.enablePrintLogLevel(false);
    settings.setNameColumnSize(NAME_COLUMN_SIZE);
    settings.enableNameColumnRightAlign(NAME_COLUMN_RIGHT_ALIGN);
    settings.setLogLevel(common_log_level);
    settings.setCrcLevel(eCRC_32);

    SocketThread st(eUDP, server, settings);
    st.m_settings.setLogLevel(common_log_level);
//    st.m_settings.setLogCallback(Log);
    st.m_settings.enableLogTime(true);
    st.m_settings.enablePrintLogLevel(false);
    st.m_settings.setColorLogCallback(Log);
    st.m_settings.setNameColumnSize(NAME_COLUMN_SIZE);
    st.m_settings.enableNameColumnRightAlign(NAME_COLUMN_RIGHT_ALIGN);
//    st.findSocket(server)->m_settings.setMaxLength(5);

    st.startThread();

    Packet packet = ConvertToPacket("Hello world!");
    Config json(ValueType::eJson, "Hello", std::string("WORLD!"));

//    st.findSocket(server)->m_settings.enableChiphering(true);
//    st.send(server, {"127.0.0.15", 31115}, packet);
    st.send(server, {"127.0.0.15", 31115}, json);

    while(1) {
        if(!isRunning) {
            st.stopThread();
            break;
        }
        usleep(1);
    }

    return 0;
}
