#include <SimpleAPI.h>

#include <csignal>
#include <regex>
#include <unistd.h>
#include <iostream>

#define MAIN_COLOR              { Color(logs::COLOR::eCYAN_FG) }
#define NAME_COLUMN_SIZE        18
#define NAME_COLUMN_RIGHT_ALIGN true

using namespace simpleapi;

logs::LEVEL common_log_level = logs::LEVEL::eDEBUG_2;
void ParseParameters(int argc, char** argv) {
    using namespace logs;

    for(int i = 1; i < argc; i++) {
        std::string str = argv[i];
        std::smatch match_log_level;
        if(std::regex_search(str, match_log_level, std::regex("log_level=([0-9]+)"))) {
            common_log_level = static_cast<LEVEL>(std::stoi(match_log_level.str(1)));
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
    using namespace logs;

    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[SERVER]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << "recv data: 0x" << utils::ToHexString(pm.m_packet)
              << std::endl;
}
bool isBigPacketSent = false;
bool isNeedAck = false;
Config jsonAck;
IpPort ipPortAck;
void RecvJson(JsonMessage jm) {
    using namespace logs;

    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[SERVER]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << logs::to_color_string({COLOR::eGREEN_BG, COLOR::eWHITE_FG},
                                       "recv json: " + jm.toString())
              << std::endl;

    if(!isBigPacketSent) {
        isNeedAck = true;
        jsonAck = Config(ValueType::eJson, "Chapter #1", "SERVER: Some test text... Some test text... Some test text... Some test text... Some test text...");
        ipPortAck = jm.m_ip_port;

        isBigPacketSent = true;
    }
}
void Log(std::string msg) {
    std::cout << msg;
}
void LogError(std::string msg) {
    std::cerr << msg;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);
    ParseParameters(argc, argv);

    IpPort server{"127.0.0.15", 31115};
    SocketSettings settings;
    settings.setRecvPacketCallback(RecvData);
    settings.setRecvJsonCallback(RecvJson);
    settings.setColorLogCallback(Log);
    settings.setColorLogErrorCallback(LogError);
    settings.setNameColumnSize(NAME_COLUMN_SIZE);
    settings.enableNameColumnRightAlign(NAME_COLUMN_RIGHT_ALIGN);
    settings.setLogLevel(common_log_level);
    settings.enableLogTime(true);
    settings.enablePrintLogLevel(true);

    SocketThread st(eUDP, server, settings);
    st.m_settings.setLogLevel(common_log_level);
    st.m_settings.enableLogTime(true);
    st.m_settings.enablePrintLogLevel(true);
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
