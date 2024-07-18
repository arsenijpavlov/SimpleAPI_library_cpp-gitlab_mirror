#ifndef SOCKET_H
#define SOCKET_H

#include "EECounter.h"
#include "IpPort.h"
#include "Json.h"
#include "Logger.h"
#include "Message.h"

#include <deque>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <set>

#include <arpa/inet.h>
#include <sys/socket.h>



#define MAX_PACKET_LENGTH 65535

using time_point_default = std::chrono::system_clock::time_point;


enum SocketType {
    eUDP,
    eTCP
};
std::string to_string(SocketType type);

//TODO: на удаление
struct ChannelSettings {
    CRC         crcLevel;
    uint16_t    maxLength;
    int         maxMsgsSentOnTick;
    ApiVersion  useApiVersion;
    uint32_t    inactivityTimer; //для проверки коннекта, перепосылки недоставленных сообщений и прочего
};

class Connection {
public:
    time_point_default  lastPingTime;
    time_point_default  lastActivity;
    EECounter           outSn;
    EECounter           inSnLastRecv;                           //влияет на границу окна ожидания фрагментов
    EECounter           inNextSn;

    std::map<EECounter, PacketMessage> mapRecvFragments;        //фрагменты сообщений (в беспорядке)
    std::map<EECounter, PacketMessage> mapRecvBuildedMessages;  //собранные по очереди фрагменты сообщений

    Connection();
};

//TODO: закончить идею
class SocketSettings {
    long        m_inactivity_timer;
    uint16_t    m_max_length;
    int         m_max_msgs_sent_on_tick;
    ApiVersion  m_api_version;

    void        (*m_packet_callback)(PacketMessage);
    void        (*m_json_callback)(JsonMessage);

    logs::LEVEL m_log_level;
    void        (*m_log_callback)(std::string);
    void        (*m_log_error_callback)(std::string);


public:
    SocketSettings() :
        m_inactivity_timer(10000),
        m_max_length(1500),
        m_max_msgs_sent_on_tick(-1),
        m_api_version(getLastApiVersion()),
        m_packet_callback(nullptr),
        m_json_callback(nullptr),
        m_log_level(logs::eINFO),
        m_log_callback(nullptr),
        m_log_error_callback(nullptr)
        {};

    void setInactivityTimer(long usec = 10000);
    void setMaxLength(uint16_t messageLength = 1500);
    void setMaxMsgsSentOnTick(int maxSendMessagesOnTick = -1);
    void setApiVersion(ApiVersion version = getLastApiVersion());
    void setPacketCallback(void (*callback)(PacketMessage) = nullptr);
    void setJsonCallback(void (*callback)(JsonMessage) = nullptr);
    void setLogLevel(logs::LEVEL level = logs::eINFO);
    void setLogCallback(void (*callback)(std::string));
    void setLogErrorCallback(void (*callback)(std::string));

    //TODO: get'теры для них же
};

class Socket {
protected:
    int             _socketFD;
    SocketType      _socketType;
    std::string     _localIP;
    uint16_t        _localPort;

    ChannelSettings _settings;

    std::map<IpPort, Connection> mapConnections; //счётчики сообщений на отправку

    void            (*logCallback)(std::string);
    void            (*logErrorCallback)(std::string);

    void            (*packetCallback)(PacketMessage);
    void            (*jsonCallback)(JsonMessage);

    logs::LEVEL     logLevel;
    logs::LEVEL     logErrorLevel;

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    //для доступа извне------------------------
    std::mutex                  outputThreadsMutex;
    std::deque<PacketMessage>   sendPacketsBuffer;      //sendFragments(), sendAutoMsg()
    std::vector<PacketMessage>  sentGlobalPackets;      //запоминаем до тех пор, пока не придёт подтверждение о передаче всех фрагментов

    std::mutex                  inputThreadsMutex;
    //собранные пакеты
    std::deque<PacketMessage>   mapRecvPacketsBuffer;   //buildPackets(), getOutPacket()
    std::deque<JsonMessage>     mapRecvJsonsBuffer;     //buildPackets(), getOutJson()
    //-----------------------------------------

    bool            checkCorrectIp(const std::string& ipString);

    uint8_t         packHeader(const PacketHeader& ph);
    PacketHeader    unpackHeader(uint8_t header);
    EECounter&      getOutSeqNumber(const IpPort& ipPort);
    PacketMessage   buildPacket(PacketMessage receivedPM);

    void            Log(logs::LEVEL level, std::string log_message);
    void            setCallbackLogOutput(void (*callback)(std::string));
    void            setCallbackLogErrorOutput(void (*callback)(std::string));

    void            setCallbackSocketReadRawData(void (*callback)(PacketMessage));
    void            setCallbackSocketReadJsonData(void (*callback)(JsonMessage));

    void            sendFragments(const std::string& remoteIp, const uint16_t remotePort, const PacketType type, const Packet& packet);
    virtual void    sendFragments(const IpPort& remoteIpPort, const PacketType type, const Packet& packet) = 0;

    virtual void    tick() = 0;
    virtual void    sendAutoMsg() = 0;
    virtual void    recvAutoMsg(int timeout) = 0;
    //=====================================
    friend class SocketThread; //для функции tick()

public:
            Socket();
    virtual ~Socket(){};
    /*NOTE: (описания конструкторов сервера)
     * конструктор с адресом
     * конструктор с адресом И callback на приём сообщений
     * конструктор с адресом И callback на приём сообщений И перенаправлением логов
     */

    //-----------------------------------------
    void            setLogLevel(logs::LEVEL logLevel);
    void            setLogErrorLevel(logs::LEVEL logLevel);
    IpPort          getLocalIpPort() { return IpPort{this->_localIP, this->_localPort}; }
    bool            isChiphering() { /*TODO: isChiphering()*/ return false; }
//TODO:    virtual bool isConnected(std::string remoteIP, uint16_t remotePort) = 0;
    bool            isServerActive();
    //-----------------------------------------
    void            chiphering(Packet& packet) {};
    void            dechiphering(Packet& packet) {};
    virtual void    startServer() = 0;
    virtual void    stopServer() = 0;
    void            close();
    //-----------------------------------------
    bool            sendRawMsg(const PacketMessage &packetMessage);
    virtual bool    sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet) = 0;
    virtual PacketMessage recvRawMsg(int timeout) = 0;
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    void            setMaxLength(uint16_t newMaxSize);     //по умолчанию 1500 байт (установлено MTU)
    void            setUseApiVersion(ApiVersion version);  //не может быть больше актуальной
//    void            enableChip(/*ChiphgeringSettings*/);
    void            enableCRC(CRC crcLevel = eCRC_OFF);
    //-----------------------------------------
    /* пользователь библиотеки вызывает эти функции
     *  внутри функции проверяется корректность адреса назначения
     *  и вызывается sendFragments() */
    bool            sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    bool            sendMsg(const std::string& remoteIP, const uint16_t remotePort, const Json& json);
    virtual bool    sendMsg(const IpPort& remoteIpPort, const Packet& packet) = 0;
    virtual bool    sendMsg(const IpPort& remoteIpPort, const Json& json) = 0;
    //-----------------------------------------
    //Эти функции работают в связке с tick()
    virtual PacketMessage   getOutPacket() = 0; //выдаст пустой пакет, если очередь пуста
    virtual JsonMessage     getOutJson() = 0;   //выдаст пустой json, если очередь пуста
    //=====================================
};

class UDPSocket : public Socket {
    //работа через tick()
//    std::map<IpPort, time_point_default>        mapLastActivity;    //только UDP, проверка коннекта
    std::map<time_point_default, PacketMessage> mapAutoSentPackets; //только UDP, уже отправленные фрагменты

    //=====================================
    //ONLY FOR USE IN SOCKET_THREAD!
    /* принятый пакет делится на части, пришиваются необходимые заголовки
     * и полученные фрагменты прокидываются в очередь на отправку через функцию sendAutoMsg */
    void            sendFragments(const IpPort& remoteIpPort, const PacketType type, const Packet& packet);

    void            tick();
    void            checkConnections();//только UDP, проверка коннекта
    void            sendAutoMsg();
    void            recvAutoMsg(int timeout);
    //=====================================

public:
    UDPSocket(const IpPort& ipPort,
              void (*callbackRecvPacket)(PacketMessage) = nullptr,
              void (*callbackRecvJson)(JsonMessage)     = nullptr,
              void (*callbackLog)(std::string)          = nullptr,
              void (*callbackLogError)(std::string)     = nullptr,
              const logs::LEVEL logLevel                = logs::eINFO,
              const logs::LEVEL logErrorLevel           = logs::eERROR);
    UDPSocket(uint16_t localPort, std::string localIP = "",
              void (*callbackRecvPacket)(PacketMessage) = nullptr,
              void (*callbackRecvJson)(JsonMessage)     = nullptr,
              void (*callbackLog)(std::string)          = nullptr,
              void (*callbackLogError)(std::string)     = nullptr,
              const logs::LEVEL logLevel                = logs::eINFO,
              const logs::LEVEL logErrorLevel           = logs::eERROR);
    ~UDPSocket();
    void            open(const uint16_t localPort, const std::string& localIP = "");

    //-----------------------------------------
//TODO:    bool isConnected(std::string remoteIP, uint16_t remotePort); //TODO: постоянный пинг
    //-----------------------------------------
    void            startServer();
    void            stopServer();
    //-----------------------------------------
    bool            sendRawMsg(const std::string& remoteIP, const uint16_t remotePort, const Packet& packet);
    PacketMessage   recvRawMsg(int timeout = -1);
    //-----------------------------------------

    //=====================================
    //УПРАВЛЕНИЕ АВТОМАТИЧЕСКИМ СЕРВЕРОМ
    void            setDeliveryNeed(bool enabled = true); //только UDP
    void            setInactivityTimer(int usec = 10000);
    void            setMaxMsgsSentOnTick(int count = -1); //-1 если все накопленные отправить разом

    bool            sendMsg(const IpPort& remoteIpPort, const Packet& packet);
    bool            sendMsg(const IpPort& remoteIpPort, const Json& json);

    PacketMessage   getOutPacket();
    JsonMessage     getOutJson();
    //=====================================
};

//class TCPSocket : public Socket
//{
//public:
//    //crc, localIP, localPort, remoteIP, remotePort
//    TCPSocket(CRC crc = eCRC_OFF);
//    ~TCPSocket(){};
//};

//class TCPServer : public TCPSocket
//{
//public:
//    ~TCPServer(){};
//};

#endif // SOCKET_H
