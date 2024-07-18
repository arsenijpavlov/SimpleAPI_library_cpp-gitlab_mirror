#ifndef MESSAGE_H
#define MESSAGE_H

#include "EECounter.h"
#include "IpPort.h"
#include "Json.h"

#include <cstdint>
#include <string>
#include <vector>


/* ================================================================================================
 * API v.1
 * ================================================================================================
 * Packets structure (Data/Json):
 * [ C/D(1) | API version(2) | Start Data(1) | Finish Data(1) | Chiphering enable (1) | CRC level (2) |
 *          | global sequence number (8) | sequence number (8) |
 *          | CRC (if enabaled, X bytes) | size (16) | data[size] ]
 * ________________________________________________________________________________________________
 *  C/D                 - тип пакета:
 *                          * Control, необходимый для работы сокета
 *                          * Data, сырые данные
 *  API version         - версия библиотеки, обратная совместимость обязательна
 *  Start Data          - флаг первого фрагмента сообщения
 *  Finish Data         - флаг последнего фрагмента сообщения
 *  Chiphering enabled  - флаг шифрования, параметры шифрования д/б отправлены контрольным пакетом
 *  CRC level           - формат checksum, используемый для проверки целостности пакета
 *                      (в основном необходимо для сборки больших пакетов, скорее всего избыточная информация)
 *  sequence number     - порядковый номер пакета, нужен для сборки больших сообщений и проверки
 *                       корректности доставки
 *  CRC                 - checksum полного сообщения, не дублируется для последующих частей
 *  size                - размер полного сообщения, не дублируется для последующих частей
 *  data                - данные
 * ==============================================================================================
 * TODO: обновить Packets structure (Control):
 * [ C/D/J (2) | API version (2) | Start Data (1) | Chiphering enable (1) | CRC level (2) | JSON:{} ]
 * ________________________________________________________________________________________________
 *  C/D                 - тип пакета:
 *                          * Control, необходимый для работы сокета
 *                          * Data, сырые данные
 *  API version         - ...
 *  Start Data          - ...
 *  Finish Data         - ...
 *  Chiphering enabled  - ..., false для всех [CONTROL] пакетов
 *  CRC level           - ...
 *  JSON:{}             - текстовое представление контрольного сообщения (без шифрования)
 * ==============================================================================================*/
using Packet = std::vector<uint8_t>;
Packet      convert_to_packet(const std::string& str);
Packet      convert_to_packet(const char* str);
std::string convert_from_packet(const Packet& packet);
std::string to_string(const Packet& packet);

enum PacketType {
    eControlType    = 0,
    eDataType       = 1
};
std::string to_string(PacketType type);

enum ApiVersion {
    Version_1   = 1
};
ApiVersion getLastApiVersion(); //TODO:

enum CRC {
    eCRC_OFF    = 0,
    eCRC_8      = 1,
    eCRC_16     = 2,
    eCRC_32     = 3
};

struct PacketHeader {
    PacketType  type;           //1 bit
    ApiVersion  version;        //2 bits
    bool        isFirstFragment;//1 bit
    bool        isLastFragment; //1 bit
    bool        isChip;         //1 bit
    CRC         crcLevel;       //2 bits
};

struct PacketError {
    EECounter sn_finish;

    PacketError() : sn_finish(255){};
};

struct PacketRange {
    EECounter start;
    EECounter finish;

    PacketRange() : start(255), finish(255) {};
};

class PacketMessage {
public:
    IpPort      ipPort;
    Packet      packet;
    EECounter   sn;
    PacketHeader header;

    bool        isError;
    PacketError error;

    bool        isBuiltComplete;
    PacketRange range;

    PacketMessage();

    void clear();
    std::string to_string();
};

class JsonMessage {
public:
    IpPort      ipPort;
    Json        json;

    JsonMessage(){};
    JsonMessage(const JsonMessage& jm);
    JsonMessage(const PacketMessage& pm);

    void clear();
    std::string to_string(int arg = -1);
};

#endif // MESSAGE_H
