#include "SocketThread.h"
#include <iostream>
#include <unistd.h>


void SocketThread::run() {
    pthread_setname_np(pthread_self(), "SERVERS_THREAD");

    while(this->isActive()) {

        for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++) {
            Socket* sock = it->second.get();

            sock->tick(); //вся магия там
        }

        usleep(1);
//        std::cout << "[THREAD]" << std::endl;
    }
}

SocketThread::SocketThread() {
    startThread();
}

SocketThread::SocketThread(const SocketType type, const std::string &localIP,
                           uint16_t localPort) {
    addSocket(type, localIP, localPort);
    startThread();
}

SocketThread::SocketThread(const SocketType type, const IpPort &localIpPort) {
    addSocket(type, localIpPort);
    startThread();
}

SocketThread::~SocketThread() {
    stopThread();
}

bool SocketThread::addSocket(const SocketType type, const std::string &localIP, const uint16_t localPort) {
    return addSocket(type, {localIP, localPort});
}

bool SocketThread::addSocket(const SocketType type, const IpPort &localIpPort) {
    if(type == SocketType::eTCP) {
        return false; //TODO: TCP пока не готов
    } else if(type == SocketType::eUDP) {
        std::shared_ptr<Socket> sock(new UDPSocket(localIpPort));
        return this->p_sockets.insert(std::make_pair(localIpPort, sock)).second;
    }

    return false;
}

void SocketThread::closeSocket(const IpPort& localIpPort) {
    this->p_sockets.erase(localIpPort);
}

void SocketThread::closeAllSockets() {
    this->p_sockets.erase(this->p_sockets.begin(), this->p_sockets.cend());
}

void SocketThread::startSocket(const IpPort& localIpPort) {
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->startServer();
}

void SocketThread::stopSocket(const IpPort& localIpPort) {
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->stopServer();
}

bool SocketThread::send(const IpPort &source, const IpPort &destination, const Packet &packet) {
    auto it = this->p_sockets.find(source);
    if(it != this->p_sockets.end())
        return it->second->sendMsg(destination, packet);
    return false;
}

bool SocketThread::send(const IpPort &source, const IpPort &destination, const Json &json) {
    auto it = this->p_sockets.find(source);
    if(it != this->p_sockets.end())
        return it->second->sendMsg(destination, json);
    return false;
}

bool SocketThread::isActive() {
    return active;
}

void SocketThread::startThread() {
    if(!isActive()) {
        std::cout << "[THREAD START]" << std::endl;
        active = true;

        t = std::thread(&SocketThread::run, this);
    }
}

void SocketThread::stopThread() {
    if(isActive()) {
        std::cout << "[THREAD STOP]" << std::endl;
        active = false; //остановили
        t.join();       //ждём завершения потока
    }
}

std::shared_ptr<Socket> SocketThread::findSocket(const IpPort &localIpPort) {
    return this->p_sockets.find(localIpPort)->second;
}

void SocketThread::setCallbackAllSocketsReadRawData(void (*callback)(PacketMessage))
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackSocketReadRawData(callback);
}

void SocketThread::setCallbackAllSocketsReadJsonData(void (*callback)(JsonMessage))
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackSocketReadJsonData(callback);
}

void SocketThread::setCallbackSocketReadRawData(const IpPort &localIpPort, void (*callback)(PacketMessage pm)) {
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setCallbackSocketReadRawData(callback);
}

void SocketThread::setCallbackSocketReadJsonData(const IpPort &localIpPort, void (*callback)(JsonMessage jm)) {
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setCallbackSocketReadJsonData(callback);
}

void SocketThread::setCallbackAllSocketsLogOutput(void (*callback)(std::string))
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackLogOutput(callback);
}

void SocketThread::setCallbackAllSocketsLogErrorOutput(void (*callback)(std::string))
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackLogErrorOutput(callback);
}

void SocketThread::setCallbackSocketLogOutput(const IpPort &localIpPort, void (*callback)(std::string))
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setCallbackLogOutput(callback);
}

void SocketThread::setCallbackSocketLogErrOutput(const IpPort &localIpPort, void (*callback)(std::string))
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setCallbackLogErrorOutput(callback);
}
