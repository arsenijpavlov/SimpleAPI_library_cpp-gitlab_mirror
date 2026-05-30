# 5.1. Сокеты

Сокеты в SimpleAPI представлены классом `Socket`, в конструкторе которого нужно указать его тип `SocketType`.
На данный момент это типы `SocketType::eUDP` и `SocketType::eTCP`(_TODO_).

Соответствуя логике SimpleAPI, пользователь не обязан знать детали внутренней реализации тех или иных классов.
Поэтому абстрактный класс `Socket` имеет двух потомков: `UDPSocket` и `TCPSocket`(_TODO_).

Детально настроить логику внутреннего поведения функций внутри этих классов можно с помощью вспомогательного класса `SocketSettings`.
SocketSettings даёт возможность выставить и получить информацию о следующих параметрах:
- включить и выключить шифрование на сокетном канале (_TODO_), `void enableChiphering(bool enabled = 0) noexcept`
- проверить активность шифрования на сокетном канале, `bool isChipheringEnabled() noexcept`
- выставить таймер неактивности, `void setInactivityTimer(long milliseconds = 10000) noexcept`
- (при автоматизации) указать размер одного фрагмента, `void setMaxLength(uint16_t max_length = 1500) noexcept`
- (при автоматизации) указать количество отправляемых фрагментов за один проход цикла, `void setMaxMsgsSentOnTick(int max_msgs_sent_on_tick = -1/*все разом*/) noexcept`
- выбрать уровень проверки целостности выкл/8B/16B/32B, `void setCrcLevel(CRC crc_level = CRC::eCRC_OFF) noexcept`
- указание использовать указанную версию внутреннего сетевого протокола SimpleAPI как максимальную, `void setApiVersion(ApiVersion version = GetLastApiVersion()) noexcept`
- (при автоматизации) указать функцию для callback-вызова при приходе нового пакета, `void setRecvPacketCallback(RecvPacketMessageCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова при приходе нового JSON-пакета, `void setRecvJsonCallback(RecvJsonMessageCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова при создании нового входящего соединения, `void setNewConnectionCallback(ConnectionCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова при дисконнекте соединения (провал проверки соединения), `void setConnectionResetCallback(ConnectionCallback callback = nullptr) noexcept`

Автоматизированный вариант сокетов будет представлен в следующем разделе.

Так как `SocketThread` является классом-наследником от `LoggerSetting`, то следует здесь же объяснить уже его возможности:
- (при автоматизации) включить вывод логов, `void enablePrintLogLevel(bool enabled = false) noexcept`
- (при автоматизации) включить вывод времени лога при выводе, `void enableLogTime(bool enabled = false) noexcept`
- (при автоматизации) включить привязку колонок при выводе к правому краю, `void enableNameColumnRightAlign(bool enabled = false) noexcept`
- (при автоматизации) указать стандартную ширину заголовка колонки при выводе, `void setNameColumnSize(int size = -1) noexcept`
- (при автоматизации) указать уровень детализации логов, `void setLogLevel(logs::LEVEL level = logs::eINFO) noexcept`
- (при автоматизации) указать функцию для callback-вызова для вывода логов, `void setLogCallback(LogCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова для вывода логов ошибок, `void setLogErrorCallback(LogCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова для вывода цветных логов, `void setColorLogCallback(LogCallback callback = nullptr) noexcept`
- (при автоматизации) указать функцию для callback-вызова для вывода цветных логов ошибок, `void setColorLogErrorCallback(LogCallback callback = nullptr) noexcept`

Вспомогательный класс `IpPort` используется для удобного хранения и передачи пары `IP-адрес - порт` при использовании SimpleAPI.
Также этот класс умеет применять значение на основе строки формата `X.X.X.X:port`, в случае неуспеха вернёт false.

## UDP сокеты

Пример создания сокета:
```c++
#include "SimpleAPI.h"

int main()
{
    using namespace simpleapi;

    // инициализация сокета в ручном режиме
    IpPort local_ip_port{"127.0.0.1", 12345};
    SocketSettings settings;
    UDPSocket sock(local_ip_port, settings);
    
    // отправка сообщения до адресата
    std::string message = "Hello world!";
    Packet packet = message.data();
    IpPort remote_ip_port{"192.168.0.100", 54321};
    sock.sendRawMsg(remote_ip_port.ip, remote_ip_port.port, packet);
    
    // ожидание ответа от адресата в течение пяти секунд
    PacketMessage answer = sock.recvRawMsg(5000);
    // проверяем, что первый пришедший на сокет пакет оказался от адресата remote_ip_port
    if (answer.m_ip_port == remote_ip_port && !answer.m_packet.empty())
    {
        std::cout << "OK" << std::endl;
    } 
    else
    {
        std::cout << "FAIL" << std::endl;
    }
}
```

## TCP сокеты

_TODO: на данный момент нет реализации_