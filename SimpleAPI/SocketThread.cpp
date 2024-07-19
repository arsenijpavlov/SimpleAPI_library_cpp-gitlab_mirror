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

    std::cout << "[THREAD STOPPED]" << std::endl;
}

SocketThread::SocketThread() :
    m_active(false),
    m_common_settings(SocketSettings())
{
//    startThread();
}

SocketThread::SocketThread(const SocketType type, const uint16_t localPort,
                           const std::string& localIP,
                           SocketSettings settings)
    : m_active(false) {
    addSocket(type, localPort, localIP, settings);
//    startThread();
}

SocketThread::SocketThread(const SocketType type, const IpPort& local_ip_port,
                           const SocketSettings settings)
    : m_active(false) {
    addSocket(type, local_ip_port, settings);
//    startThread();
}

SocketThread::~SocketThread() {
    stopThread();
}

bool SocketThread::addSocket(const SocketType type, const uint16_t local_port,
                             const std::string& local_ip,
                             const SocketSettings settings) {
    return addSocket(type, IpPort{local_ip, local_port}, settings);
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
    if(commonSettings)
        return addSocket(type, local_port, local_ip, m_common_settings);
    else
        return addSocket(type, local_port, local_ip, SocketSettings());
}

bool SocketThread::addSocket(const SocketType type, const IpPort &local_ip_port, bool commonSettings)
{
    if(commonSettings)
        return addSocket(type, local_ip_port, m_common_settings);
    else
        return addSocket(type, local_ip_port, SocketSettings());
}

void SocketThread::closeSocket(const IpPort& localIpPort) {
    m_sockets.erase(localIpPort);
}

void SocketThread::closeAllSockets() {
    m_sockets.erase(m_sockets.begin(), m_sockets.cend());
}

void SocketThread::startSocket(const IpPort& localIpPort) {
    auto it = m_sockets.find(localIpPort);
    if(it != m_sockets.end())
        it->second->startServer();
}

void SocketThread::stopSocket(const IpPort& localIpPort) {
    auto it = m_sockets.find(localIpPort);
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

bool SocketThread::isActive() {
    return m_active;
}

void SocketThread::startThread() {
    if(!isActive()) {
        std::cout << "[THREAD START]" << std::endl;
        m_active = true;

        m_thread = std::thread(&SocketThread::run, this);
    }
}

void SocketThread::stopThread() {
    if(isActive()) {
        std::cout << "[THREAD STOP]" << std::endl;
        m_active = false; //дали сигнал на остановку

        if(m_thread.joinable())
            m_thread.join(); //ждём завершения потока
    }
}

std::shared_ptr<Socket> SocketThread::findSocket(const IpPort &localIpPort) {
    return m_sockets.find(localIpPort)->second;
}

void SocketThread::setAllSocketsSettings(const SocketSettings settings)
{
    m_common_settings = settings;

    for(auto it = m_sockets.begin(); it != m_sockets.end(); it++)
        it->second->setSettings(settings);
}

void SocketThread::setSocketsSettings(const IpPort& local_ip_port, const SocketSettings settings)
{

    auto it = m_sockets.find(local_ip_port);
    if(it != m_sockets.end())
        it->second->setSettings(settings);
}

