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

SocketThread::SocketThread() :
    active(false),
    common_SocketsReadRawDataCallback(nullptr),
    common_SocketsReadJsonDataCallback(nullptr),
    common_LogOutputCallback(nullptr),
    common_LogErrorOutputCallback(nullptr),
    common_logLevel(logs::eINFO),
    common_logErrorLevel(logs::eERROR)
{
//    startThread();
}

SocketThread::SocketThread(const SocketType type, const uint16_t localPort,
                           const std::string& localIP,
                           void (*callbackRecvPacket)(PacketMessage),
                           void (*callbackRecvJson)(JsonMessage),
                           void (*callbackLog)(std::string),
                           void (*callbackLogError)(std::string),
                           const logs::LEVEL logLevel,
                           const logs::LEVEL logErrorLevel)
    : active(false) {
    addSocket(type, localPort, localIP,
              callbackRecvPacket, callbackRecvJson,
              callbackLog, callbackLogError,
              logLevel, logErrorLevel);
//    startThread();
}

SocketThread::SocketThread(const SocketType type, const IpPort &localIpPort,
                           void (*callbackRecvPacket)(PacketMessage),
                           void (*callbackRecvJson)(JsonMessage),
                           void (*callbackLog)(std::string),
                           void (*callbackLogError)(std::string),
                           const logs::LEVEL logLevel,
                           const logs::LEVEL logErrorLevel)
    : active(false) {
    addSocket(type, localIpPort,
              callbackRecvPacket, callbackRecvJson,
              callbackLog, callbackLogError,
              logLevel, logErrorLevel);
//    startThread();
}

SocketThread::~SocketThread() {
    stopThread();
}

bool SocketThread::addSocket(const SocketType type, const uint16_t localPort,
                             const std::string& localIP,
                             void (*callbackRecvPacket)(PacketMessage),
                             void (*callbackRecvJson)(JsonMessage),
                             void (*callbackLog)(std::string),
                             void (*callbackLogError)(std::string),
                             const logs::LEVEL logLevel,
                             const logs::LEVEL logErrorLevel) {
    return addSocket(type, IpPort{localIP, localPort},
                     callbackRecvPacket, callbackRecvJson,
                     callbackLog, callbackLogError,
                     logLevel, logErrorLevel);
}

bool SocketThread::addSocket(const SocketType type, const IpPort &localIpPort,
                             void (*callbackRecvPacket)(PacketMessage),
                             void (*callbackRecvJson)(JsonMessage),
                             void (*callbackLog)(std::string),
                             void (*callbackLogError)(std::string),
                             const logs::LEVEL logLevel,
                             const logs::LEVEL logErrorLevel) {
    if(type == SocketType::eTCP) {
        return false; //TODO: TCP пока не готов
    } else if(type == SocketType::eUDP) {
        std::shared_ptr<Socket> sock(new UDPSocket(localIpPort,
                                                   callbackRecvPacket, callbackRecvJson,
                                                   callbackLog, callbackLogError,
                                                   logLevel, logErrorLevel));
        return this->p_sockets.insert(std::make_pair(localIpPort, sock)).second;
    }

    return false;
}

bool SocketThread::addSocket(const SocketType type, const uint16_t localPort,
                             const std::string localIP, bool commonSettings)
{
    if(commonSettings)
        return addSocket(type, localPort, localIP,
                         this->common_SocketsReadRawDataCallback,
                         this->common_SocketsReadJsonDataCallback,
                         this->common_LogOutputCallback,
                         this->common_LogErrorOutputCallback,
                         this->common_logLevel,
                         this->common_logErrorLevel);
    else
        return addSocket(type, localPort, localIP, nullptr); //остальные тоже nullptr
}

bool SocketThread::addSocket(const SocketType type, const IpPort &localIpPort, bool commonSettings)
{
    if(commonSettings)
        return addSocket(type, localIpPort,
                         this->common_SocketsReadRawDataCallback,
                         this->common_SocketsReadJsonDataCallback,
                         this->common_LogOutputCallback,
                         this->common_LogErrorOutputCallback,
                         this->common_logLevel,
                         this->common_logErrorLevel);
    else
        return addSocket(type, localIpPort, nullptr); //остальные тоже nullptr
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

        if(t.joinable())
            t.join();       //ждём завершения потока
    }
}

std::shared_ptr<Socket> SocketThread::findSocket(const IpPort &localIpPort) {
    return this->p_sockets.find(localIpPort)->second;
}

void SocketThread::setCallbackAllSocketsReadRawData(void (*callback)(PacketMessage))
{
    this->common_SocketsReadRawDataCallback = callback;

    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackSocketReadRawData(callback);
}

void SocketThread::setCallbackAllSocketsReadJsonData(void (*callback)(JsonMessage))
{
    this->common_SocketsReadJsonDataCallback = callback;

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

void SocketThread::setCallbackAllSocketsLogOutput(void (*callback)(std::string),
                                                  const logs::LEVEL logLevel)
{
    this->common_logLevel = logLevel;
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackLogOutput(callback);
}

void SocketThread::setCallbackAllSocketsLogErrorOutput(void (*callback)(std::string),
                                                       const logs::LEVEL logLevel)
{
    this->common_logErrorLevel = logLevel;
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setCallbackLogErrorOutput(callback);
}

void SocketThread::setAllLogLevel(logs::LEVEL logLevel)
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setLogLevel(logLevel);
}

void SocketThread::setAllLogErrorLevel(logs::LEVEL logLevel)
{
    for(auto it = this->p_sockets.begin(); it != this->p_sockets.end(); it++)
        it->second->setLogErrorLevel(logLevel);
}

void SocketThread::setCallbackSocketLogOutput(const IpPort &localIpPort, void (*callback)(std::string),
                                              const logs::LEVEL logLevel)
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end()) {
        it->second->setCallbackLogOutput(callback);
        it->second->setLogLevel(logLevel);
    }
}

void SocketThread::setCallbackSocketLogErrOutput(const IpPort &localIpPort, void (*callback)(std::string),
                                                 const logs::LEVEL logLevel)
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end()) {
        it->second->setCallbackLogErrorOutput(callback);
        it->second->setLogErrorLevel(logLevel);
    }
}

void SocketThread::setLogLevel(const IpPort &localIpPort, logs::LEVEL logLevel)
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setLogLevel(logLevel);
}

void SocketThread::setLogErrorLevel(const IpPort &localIpPort, logs::LEVEL logLevel)
{
    auto it = this->p_sockets.find(localIpPort);
    if(it != this->p_sockets.end())
        it->second->setLogErrorLevel(logLevel);
}
