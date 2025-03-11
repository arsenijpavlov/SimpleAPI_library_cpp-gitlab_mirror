#ifndef SOCKET_SETTINGS_H
#define SOCKET_SETTINGS_H


#include "../logger/LoggerSettings.h"
#include "Message.h"

#include <functional>
#include <string>


class SocketSettings : public LoggerSettings {
public:
    using RecvPacketMessageCallback = std::function<void(PacketMessage)>;
    using RecvJsonMessageCallback   = std::function<void(JsonMessage)>;
    using ConnectionCallback        = std::function<void(IpPort)>;

private:
    ApiVersion                  m_api_version;
    CRC                         m_crc_level;
    long                        m_inactivity_timer;
    uint16_t                    m_max_length;
    int                         m_max_msgs_sent_on_tick;
    RecvJsonMessageCallback     m_recv_json_callback;
    RecvPacketMessageCallback   m_recv_packet_callback;
    ConnectionCallback          m_new_connection_callback;
    ConnectionCallback          m_connection_reset_callback;
    bool                        m_chiphering_enabled;

public:
    SocketSettings() noexcept :
        m_api_version               (getLastApiVersion()),
        m_crc_level                 (CRC::eCRC_OFF),
        m_inactivity_timer          (10000),
        m_max_length                (1500),
        m_max_msgs_sent_on_tick     (-1),
        m_recv_json_callback        (nullptr),
        m_recv_packet_callback      (nullptr),
        m_new_connection_callback   (nullptr),
        m_connection_reset_callback (nullptr),
        m_chiphering_enabled        (false)
    {}

    void enableChiphering(bool enabled = false) noexcept                    { m_chiphering_enabled = enabled; }
    bool isChipheringEnabled() noexcept                                     { return m_chiphering_enabled; }

    void setInactivityTimer(long milliseconds = 10000) noexcept             { m_inactivity_timer = milliseconds; }
    void setMaxLength(uint16_t max_length = 1500) noexcept                  { m_max_length = max_length; }
    //NOTE: -1 == все накопленные отправить за один заход
    void setMaxMsgsSentOnTick(int max_msgs_sent_on_tick = -1) noexcept      { m_max_msgs_sent_on_tick = max_msgs_sent_on_tick; }
    void setCrcLevel(CRC crc_level = CRC::eCRC_OFF) noexcept                { m_crc_level = crc_level; }
    void setApiVersion(ApiVersion version = getLastApiVersion()) noexcept   { m_api_version = version; }
    void setRecvPacketCallback(RecvPacketMessageCallback callback = nullptr) noexcept
                                                                            { m_recv_packet_callback = callback; }
    void setRecvJsonCallback(RecvJsonMessageCallback callback = nullptr) noexcept
                                                                            { m_recv_json_callback = callback; }
    void setNewConnectionCallback(ConnectionCallback callback = nullptr) noexcept
                                                                            { m_new_connection_callback = callback; }
    void setConnectionResetCallback(ConnectionCallback callback = nullptr) noexcept
                                                                            { m_connection_reset_callback = callback; }

    long                        getInactivityTimer() noexcept               { return m_inactivity_timer; }
    uint16_t                    getMaxLength() noexcept                     { return m_max_length; }
    int                         getMaxMsgsSentOnTick() noexcept             { return m_max_msgs_sent_on_tick; }
    CRC                         getCrcLevel() noexcept                      { return m_crc_level; }
    ApiVersion                  getApiVersion() noexcept                    { return m_api_version; }
    RecvPacketMessageCallback   getPacketCallback() noexcept                { return m_recv_packet_callback; }
    RecvJsonMessageCallback     getJsonCallback() noexcept                  { return m_recv_json_callback; }
    ConnectionCallback          getNewConnectionCallback() noexcept         { return m_new_connection_callback; }
    ConnectionCallback          getConnectionResetCallback() noexcept       { return m_connection_reset_callback; }
};

#endif // SOCKET_SETTINGS_H
