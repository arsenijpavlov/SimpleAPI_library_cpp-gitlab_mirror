#include "SocketThread.h"
#include "TcpSocket.h"
#include "UdpSocket.h"
#include <iostream>
#include <regex>
#include <unistd.h>


namespace simpleapi {

#define SOCKETS_THREAD_NAME "SERVERS_THREAD"

void SocketThread::run() noexcept {
    pthread_setname_np(pthread_self(), SOCKETS_THREAD_NAME);

    while(this->isActive()) {
        /*MUTEX*/ {
            std::lock_guard<std::mutex> lg(m_sockets_mutex);

            for(auto it = m_sockets.begin(); it != m_sockets.end(); it++) {
                Socket* sock = it->second.get();

                sock->tick(); //вся магия там
            }
        }

        usleep(100);
    }
}

void SocketThread::log(const logs::LEVEL level, std::string log_message) noexcept {
    LoggerSettings::LogCallback currentCallback = nullptr;
    LoggerSettings::LogCallback currentColorCallback = nullptr;
    std::string levelSubstring = "";
    std::string timeString = "";
    if(m_settings.isLogTimeEnabled())
        timeString = logs::get_time_string() + " ";

    if(level <= m_settings.getLogLevel()) {
        switch(level) {
        case logs::LEVEL::eWARNING:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".w";
            break;
        case logs::LEVEL::eINFO:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".i";
            break;
        case logs::LEVEL::eDEBUG:
        case logs::LEVEL::eDEBUG_2:
        case logs::LEVEL::eDEBUG_3:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".d";
            break;
        case logs::LEVEL::eERROR:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".e";
            break;
        default:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".unknown";
            break;
        }
    }

    //цветной вывод
    if(currentColorCallback)
        currentColorCallback(
            timeString
            + logs::columned(level, std::string("[") + SOCKETS_THREAD_NAME
                                        + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                        + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " " + log_message + "\n");

    //обычный вывод
    if(currentCallback) {
        /* перед выводом удаляются все команды форматирования */ {
            static std::regex reg("\\x1B\[[0-9;]*m");
            log_message = std::regex_replace(log_message, reg, "");
        }
        currentCallback(
            timeString
            + logs::columned(std::string("[") + SOCKETS_THREAD_NAME
                                 + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                 + "]",
                             m_settings.getNameColumnSize(),
                             m_settings.isNameColumnRightAlignEnabled())
            + " " + log_message + "\n");
    }
}

bool SocketThread::addSocket(const SocketType type, const uint16_t local_port,
               const std::string& local_ip,
               const SocketSettings settings) noexcept
{
    return addSocket(type, IpPort{local_ip, local_port}, settings);
}

bool SocketThread::addSocket(const SocketType type, const IpPort& local_ip_port,
                             const SocketSettings settings) noexcept {
    if(type == SocketType::eTCP) {
        return false; //TODO (потом): TCP пока не готов
    } else if(type == SocketType::eUDP) {
        std::shared_ptr<Socket> sock(new UDPSocket(local_ip_port, settings));
        std::lock_guard<std::mutex> lg(m_sockets_mutex);

        return m_sockets.insert(std::make_pair(local_ip_port, sock)).second;
    }

    return false;
}

bool SocketThread::addSocket(const SocketType type, const uint16_t local_port,
                             const std::string local_ip, const bool commonSettings) noexcept {
    if(commonSettings)  return addSocket(type, local_port, local_ip, m_common_socket_settings);
    else                return addSocket(type, local_port, local_ip, SocketSettings());
}

bool SocketThread::addSocket(const SocketType type, const IpPort &local_ip_port,
                             bool commonSettings) noexcept {
    if(commonSettings)  return addSocket(type, local_ip_port, m_common_socket_settings);
    else                return addSocket(type, local_ip_port, SocketSettings());
}

void SocketThread::closeSocket(const IpPort &local_ip_port) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    m_sockets.erase(local_ip_port);
}

void SocketThread::closeAllSockets() noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    m_sockets.erase(m_sockets.begin(), m_sockets.cend());
}

bool SocketThread::startSocket(const IpPort& local_ip_Port) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    auto it = m_sockets.find(local_ip_Port);
    if(it != m_sockets.end())
        return it->second->startServer();
    return false;
}

void SocketThread::stopSocket(const IpPort& local_ip_Port) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    auto it = m_sockets.find(local_ip_Port);
    if(it != m_sockets.end())
        it->second->stopServer();
}

void SocketThread::send(const IpPort &source, const IpPort &destination,
                        const Packet &packet) noexcept
{
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    auto it = m_sockets.find(source);
    if(it != m_sockets.end())
        it->second->sendMsg(destination, packet);
}

void SocketThread::send(const IpPort &source, const IpPort &destination,
                        const Config &json) noexcept
{
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    auto it = m_sockets.find(source);
    if(it != m_sockets.end())
        it->second->sendMsg(destination, json);
}

void SocketThread::startThread() noexcept {
    if(!isActive()) {
        log(logs::LEVEL::eDEBUG, "starting...");
        m_active = true;

        m_thread = std::thread(&SocketThread::run, this);
        log(logs::LEVEL::eINFO, "started");
    }
}

void SocketThread::stopThread() noexcept {
    if(isActive()) {
        log(logs::LEVEL::eDEBUG, "stop...");
        m_active = false; //дали сигнал на остановку

        if(m_thread.joinable()) {
            m_thread.join(); //ждём завершения потока
            log(logs::LEVEL::eINFO, "stopped");
        }
    }
}

std::shared_ptr<Socket> SocketThread::findSocket(const IpPort &local_ip_Port) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    return m_sockets.find(local_ip_Port)->second;
}

void SocketThread::setAllSocketsSettings(const SocketSettings settings) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);
    m_common_socket_settings = settings;

    for(auto it = m_sockets.begin(); it != m_sockets.end(); it++)
        it->second->setSettings(settings);
}

void SocketThread::setSocketsSettings(const IpPort& local_ip_port, const SocketSettings settings) noexcept {
    std::lock_guard<std::mutex> lg(m_sockets_mutex);

    auto it = m_sockets.find(local_ip_port);
    if(it != m_sockets.end())
        it->second->setSettings(settings);
}

} // namespace simpleapi

