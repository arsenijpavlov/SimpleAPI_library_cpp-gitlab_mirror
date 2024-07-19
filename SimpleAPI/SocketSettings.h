#ifndef SOCKETSETTINGS_H
#define SOCKETSETTINGS_H


#include "Logger.h"
#include "Message.h"

#include <functional>
#include <string>


class SocketSettings {
public:
    using LogCallback               = std::function<void(std::string)>;
    using LogErrorCallback          = std::function<void(std::string)>;
    using RecvPacketMessageCallback = std::function<void(PacketMessage)>;
    using RecvJsonMessageCallback   = std::function<void(JsonMessage)>;

private:
    ApiVersion          m_api_version;
    CRC                 m_crc_level;
    long                m_inactivity_timer;
    logs::LEVEL         m_log_level;
    LogCallback         m_log_callback;
    LogErrorCallback    m_log_error_callback;
    uint16_t            m_max_length;
    int                 m_max_msgs_sent_on_tick;
    RecvJsonMessageCallback   m_recv_json_callback;
    RecvPacketMessageCallback m_recv_packet_callback;

public:
    SocketSettings() :
        m_api_version           (getLastApiVersion()),
        m_crc_level             (CRC::eCRC_OFF),
        m_inactivity_timer      (10000),
        m_log_level             (logs::eINFO),
        m_log_callback          (nullptr),
        m_log_error_callback    (nullptr),
        m_max_length            (1500),
        m_max_msgs_sent_on_tick (-1),
        m_recv_json_callback    (nullptr),
        m_recv_packet_callback  (nullptr)
    {}

    void setInactivityTimer(long milliseconds = 10000)                      { m_inactivity_timer = milliseconds; }
    void setMaxLength(uint16_t max_length = 1500)                           { m_max_length = max_length; }
    //NOTE: -1 == все накопленные отправить за один заход
    void setMaxMsgsSentOnTick(int max_msgs_sent_on_tick = -1)               { m_max_msgs_sent_on_tick = max_msgs_sent_on_tick; }
    void setCrcLevel(CRC crc_level = CRC::eCRC_OFF)                         { m_crc_level = crc_level; }
    void setApiVersion(ApiVersion version = getLastApiVersion())            { m_api_version = version; }
    void setRecvPacketCallback(RecvPacketMessageCallback callback = nullptr){ m_recv_packet_callback = callback; }
    void setRecvJsonCallback(RecvJsonMessageCallback callback = nullptr)    { m_recv_json_callback = callback; }
    void setLogLevel(logs::LEVEL level = logs::eINFO)                       { m_log_level = level; }
    void setLogCallback(LogCallback callback = nullptr)                     { m_log_callback = callback; }
    void setLogErrorCallback(LogErrorCallback callback = nullptr)           { m_log_error_callback = callback; }

    long                        getInactivityTimer()                        { return m_inactivity_timer; }
    uint16_t                    getMaxLength()                              { return m_max_length; }
    int                         getMaxMsgsSentOnTick()                      { return m_max_msgs_sent_on_tick; }
    CRC                         getCrcLevel()                               { return m_crc_level; }
    ApiVersion                  getApiVersion()                             { return m_api_version; }
    RecvPacketMessageCallback   getPacketCallback()                         { return m_recv_packet_callback; }
    RecvJsonMessageCallback     getJsonCallback()                           { return m_recv_json_callback; }
    logs::LEVEL                 getLogLevel()                               { return m_log_level; }
    LogCallback                 getLogCallback()                            { return m_log_callback; }
    LogErrorCallback            getLogErrorCallback()                       { return m_log_error_callback; }
};

#endif // SOCKETSETTINGS_H
