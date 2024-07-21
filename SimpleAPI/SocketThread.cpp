#include "SocketThread.h"
#include <iostream>
#include <unistd.h>


void SocketThread::run() {
    pthread_setname_np(pthread_self(), "SERVERS_THREAD");

    while(this->isActive()) {

        for(auto it = m_sockets.begin(); it != m_sockets.end(); it++) {
            Socket* sock = it->second.get();

            sock->tick(); //вся магия там
        }

        usleep(1);
    }
}

void SocketThread::Log(const logs::LEVEL level, const std::string log_message, const std::string color_log_message)
{   
    LoggerSettings::LogCallback currentCallback = nullptr;
    LoggerSettings::LogCallback currentColorCallback = nullptr;
    std::string levelSubstring = "";
    std::string timeString = "";
    if(m_settings.isLogTimeEnabled())
        timeString = logs::get_time_string() + " ";

    if(level <= m_settings.getLogLevel()) {
        switch(level) {
        case logs::eWARNING:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".w";
            break;
        case logs::eINFO:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".i";
            break;
        case logs::eDEBUG:
            currentCallback         = m_settings.getLogCallback();
            currentColorCallback    = m_settings.getColorLogCallback();
            levelSubstring          = ".d";
            break;
        case logs::eERROR:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".e";
        default:
            currentCallback         = m_settings.getLogErrorCallback();
            currentColorCallback    = m_settings.getColorLogErrorCallback();
            levelSubstring          = ".unknown";
            break;
        }
    }

    //обычный вывод
    if(currentCallback)
        currentCallback(
            timeString
            + "["
            + "SOCKET THREAD"
            + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
            + "] "
            + log_message
            + "\n");
    //цветной вывод
    if(currentColorCallback)
        currentColorCallback(
            timeString
            + to_color_string(level, std::string("[")
                                         + "SOCKET THREAD"
                                         + (m_settings.isPrintLogLevelEnabled() ? levelSubstring : "")
                                         + "] ")
            + (color_log_message.empty() ? log_message : color_log_message)
            + "\n");
}

bool SocketThread::addSocket(const SocketType type, const IpPort& local_ip_port,
                             const SocketSettings settings) {
    if(type == SocketType::eTCP) {
        return false; //TODO: TCP пока не готов
    } else if(type == SocketType::eUDP) {
        std::shared_ptr<Socket> sock(new UDPSocket(local_ip_port, settings));
        return m_sockets.insert(std::make_pair(local_ip_port, sock)).second;
    }

    return false;
}

bool SocketThread::addSocket(const SocketType type, const uint16_t local_port,
                             const std::string local_ip, const bool commonSettings)
{
    if(commonSettings)  return addSocket(type, local_port, local_ip, m_common_socket_settings);
    else                return addSocket(type, local_port, local_ip, SocketSettings());
}

bool SocketThread::addSocket(const SocketType type, const IpPort &local_ip_port, bool commonSettings)
{
    if(commonSettings)  return addSocket(type, local_ip_port, m_common_socket_settings);
    else                return addSocket(type, local_ip_port, SocketSettings());
}

void SocketThread::startSocket(const IpPort& local_ip_Port) {
    auto it = m_sockets.find(local_ip_Port);
    if(it != m_sockets.end())
        it->second->startServer();
}

void SocketThread::stopSocket(const IpPort& local_ip_Port) {
    auto it = m_sockets.find(local_ip_Port);
    if(it != m_sockets.end())
        it->second->stopServer();
}

bool SocketThread::send(const IpPort &source, const IpPort &destination, const Packet &packet) {
    auto it = m_sockets.find(source);
    if(it != m_sockets.end())
        return it->second->sendMsg(destination, packet);
    return false;
}

bool SocketThread::send(const IpPort &source, const IpPort &destination, const Json &json) {
    auto it = m_sockets.find(source);
    if(it != m_sockets.end())
        return it->second->sendMsg(destination, json);
    return false;
}

void SocketThread::startThread() {
    if(!isActive()) {
        Log(logs::eDEBUG, "starting...");
        m_active = true;

        m_thread = std::thread(&SocketThread::run, this);
        Log(logs::eINFO, "started");
    }
}

void SocketThread::stopThread() {
    if(isActive()) {
        Log(logs::eDEBUG, "stop...");
        m_active = false; //дали сигнал на остановку

        if(m_thread.joinable()) {
            m_thread.join(); //ждём завершения потока
            Log(logs::eINFO, "stopped");
        }
    }
}

std::shared_ptr<Socket> SocketThread::findSocket(const IpPort &local_ip_Port) {
    return m_sockets.find(local_ip_Port)->second;
}

void SocketThread::setAllSocketsSettings(const SocketSettings settings)
{
    m_common_socket_settings = settings;

    for(auto it = m_sockets.begin(); it != m_sockets.end(); it++)
        it->second->setSettings(settings);
}

void SocketThread::setSocketsSettings(const IpPort& local_ip_port, const SocketSettings settings)
{

    auto it = m_sockets.find(local_ip_port);
    if(it != m_sockets.end())
        it->second->setSettings(settings);
}

