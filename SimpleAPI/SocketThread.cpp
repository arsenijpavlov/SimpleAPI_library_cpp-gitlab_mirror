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

    Log(logs::eINFO, "stopped");
}

void SocketThread::Log(logs::LEVEL level, std::string log_message)
{
    if(level <= m_settings.getLogLevel()) {
        switch(level) {
        case logs::eWARNING:
        case logs::eINFO:
        case logs::eDEBUG: {
            if(m_settings.getLogCallback())
                m_settings.getLogCallback()(
                    logs::get_time_string() + " "
                    + to_color_string(level, "SOCKET THREAD", false) + " "
                    + log_message + "\n");
            break;
        }
        case logs::eERROR:
        default: {
            if(m_settings.getLogErrorCallback())
                m_settings.getLogErrorCallback()(
                    logs::get_time_string() + " "
                    + to_color_string(level, "SOCKET THREAD", false) + " "
                    + log_message + "\n");
            break;
        }
        }
    }
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
        Log(logs::eINFO, "start");
        m_active = true;

        m_thread = std::thread(&SocketThread::run, this);
    }
}

void SocketThread::stopThread() {
    if(isActive()) {
        Log(logs::eDEBUG, "stop...");
        m_active = false; //дали сигнал на остановку

        if(m_thread.joinable())
            m_thread.join(); //ждём завершения потока
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

