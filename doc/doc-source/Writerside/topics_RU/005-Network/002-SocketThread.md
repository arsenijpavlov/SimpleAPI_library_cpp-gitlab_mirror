# 5.2. Поток для автоматизации работы сокетов

Класс `SocketThread` предназначен для вынесения всех сокетных соединений в отдельный поток с автоматизацией работы с сокетами.

Главное преимущество этого класса заключается в том, что достаточно один раз объявить добавление к объекту `SocketThread` нового экземпляра сокета и дальнейшее взаимодействие с сокетами будет происходить по логике функций обратного вызова (callback).

## Улучшения по сравнению с ручными сокетами
- автоматическая фрагментация пакетов при отправке и их дефрагментация при получении
- если полученный пакет без ошибок сконвертировался в Json, то будет вызван callback для полученного Json-пакета
- автоматическая проверка наличия соединения (пинги раз в несколько секунд)
- уведомления о новых соединениях, входящих пакетах/Json-документах и разрывах соединений при долгом отсутствии активности адресата

## Основные параметры автоматизированных сокетов
При использовании такого подхода, `SocketSettings` позволяет настроить ещё и такие параметры:
- указать размер одного фрагмента, `void setMaxLength(uint16_t max_length = 1500) noexcept`
- указать количество отправляемых фрагментов за один проход цикла, `void setMaxMsgsSentOnTick(int max_msgs_sent_on_tick = -1/*все разом*/) noexcept`
- указать функцию для callback-вызова при приходе нового пакета, `void setRecvPacketCallback(RecvPacketMessageCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова при приходе нового JSON-пакета, `void setRecvJsonCallback(RecvJsonMessageCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова при создании нового входящего соединения, `void setNewConnectionCallback(ConnectionCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова при дисконнекте соединения (провал проверки соединения), `void setConnectionResetCallback(ConnectionCallback callback = nullptr) noexcept`

## Логирование автоматизированных сокетов
Так как `SocketThread` является классом-наследником от `LoggerSetting`, то следует здесь же объяснить уже его возможности:
- включить вывод логов, `void enablePrintLogLevel(bool enabled = false) noexcept`
- включить вывод времени лога при выводе, `void enableLogTime(bool enabled = false) noexcept`
- включить привязку колонок при выводе к правому краю, `void enableNameColumnRightAlign(bool enabled = false) noexcept`
- указать стандартную ширину заголовка колонки при выводе, `void setNameColumnSize(int size = -1) noexcept`
- указать уровень детализации логов, `void setLogLevel(logs::LEVEL level = logs::eINFO) noexcept`
- указать функцию для callback-вызова для вывода логов, `void setLogCallback(LogCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова для вывода логов ошибок, `void setLogErrorCallback(LogCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова для вывода цветных логов, `void setColorLogCallback(LogCallback callback = nullptr) noexcept`
- указать функцию для callback-вызова для вывода цветных логов ошибок, `void setColorLogErrorCallback(LogCallback callback = nullptr) noexcept`

## Безопасность потоков
*Обратите внимание, что все callback-функции будут вызваны из потока SocketThread. Вся потоконезависимая часть программы реализуется самим пользователем библиотеки.*

## Пример кода использования SocketThread (в минимальном виде):
```c++
#include "SimpleAPI.h"

// код обработчика нажатия Ctrl+C (SIGINT)
bool isRunning = true;
void signalHandler(int signal) {
    if(signal == SIGINT) {
        std::cout << "\n";
        isRunning = false;
    }
}

int main()
{
    signal(SIGINT, signalHandler); // включить обработчик для SIGINT
    
    using namespace simpleapi;
    
    // создание callback-функции на входящий Json
    auto RecvJson = [](JsonMessage jm) -> void {
        std::cout << logs::get_time_string() << " "
                  << logs::columned(MAIN_COLOR, "[SERVER]",
                                    NAME_COLUMN_SIZE,
                                    NAME_COLUMN_RIGHT_ALIGN) << " "
                  << logs::to_color_string({logs::COLOR::eGREEN_BG, logs::COLOR::eWHITE_FG},
                                           "recv json: " + jm.toString())
                  << std::endl;
    }
    
    // создание callback-функции на входящий пакет в виде байтов
    auto RecvData = [](PacketMessage pm) -> void {
    std::cout << logs::get_time_string() << " "
              << logs::columned(MAIN_COLOR, "[SERVER]",
                                NAME_COLUMN_SIZE,
                                NAME_COLUMN_RIGHT_ALIGN) << " "
              << "recv data: 0x" << utils::ToHexString(pm.m_packet)
              << std::endl;
    };
    
    IpPort local_server{"127.0.0.15", 31115};
    SocketSettings settings;
    settings.setRecvPacketCallback(RecvData);
    settings.setRecvJsonCallback(RecvJson);

    // создание потока сокетов
    SocketThread st;

    // добавление к потоку нового UDP-сокета    
    st.addSocket(eUDP, server, settings);
    
    // запуск добавленного потока
    st.startThread();
    
    IpPort remote_ip_port{"127.0.0.16", 51113};
    Config json_message;
    json_message["Hello"] = "World!";
    
    bool isNeedSend = true; // флаг для одноразовой отправки
    while(1)
    {
        // флаг вызовется при нажатии Ctrl+C
        if(!isRunning) {
            st.stopThread();
            break;
        }

        if(isNeedSend)
            st.send(server, remote_ip_port, json_message);
            isNeedSend = false;
        }
    
        usleep(100); 
    }
}
```
