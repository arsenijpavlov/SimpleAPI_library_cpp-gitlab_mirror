#include "SocketThread.h"


void SocketThread::run() {
    while(isActive()) {
        std::set<std::shared_ptr<Socket*>>::iterator it = p_sockets.begin();
        while(it != p_sockets.end()) {
            Socket* sock = *(it->get());

            sock->tick(); //вся магия там

            PacketMessage pm = sock->getOutPacket();
            if(!pm.packet.empty() && this->packetCallback)
                this->packetCallback(pm);
            JsonMessage jm = sock->getOutJson();
            if(!jm.json.isEmpty() && this->jsonCallback)
                this->jsonCallback(jm);
        }
    }
}

SocketThread::SocketThread() : active(true) {
    startThread();
}

SocketThread::SocketThread(const SocketType type, const std::string &localIP,
                           uint16_t localPort) : active(true) {
    addSocket(type, localIP, localPort);
    startThread();
}

SocketThread::SocketThread(const SocketType type, const IpPort &localIpPort) : active(true) {
    addSocket(type, localIpPort);
    startThread();
}

SocketThread::~SocketThread() {
    stopThread();

}

bool SocketThread::addSocket(const SocketType type, const std::string &localIP, const uint16_t localPort) {
    if(type == SocketType::eTCP) {
        return false; //TODO: TCP пока не готов
    } else if(type == SocketType::eUDP) {
        std::shared_ptr<Socket*> sock = std::make_shared<Socket*>(UDPSocket(localPort, localIP));
        return this->p_sockets.insert(sock).second;
    }

    return false;
}

bool SocketThread::addSocket(const SocketType type, const IpPort &localIpPort) {
    return addSocket(type, localIpPort.ip, localIpPort.port);
}

std::set<std::shared_ptr<Socket*>>::iterator SocketThread::find(
    const SocketType type, const std::string &localIp, const uint16_t localPort)
{
    return find(type, IpPort{localIp, localPort});
}

std::set<std::shared_ptr<Socket*>>::iterator SocketThread::find(
    const SocketType type, const IpPort &localIpPort)
{
    auto it = p_sockets.begin();
    while(it != p_sockets.end()) {
        switch(type) {
        case eUDP: {
            if(std::dynamic_pointer_cast<UDPSocket*>(*it)) {
                if(((UDPSocket*)it->get())->getLocalIpPort() == localIpPort)
                    return it;
            }
            break;
        }
        case eTCP: {
//            if(std::dynamic_pointer_cast<TCPSocket*>(*it))
//                if(((TCPSocket*)it->get())->getLocalIpPort() == ipPort)
//                    return it;
            break;
        }
        default: p_sockets.end();
        }

    }

    return p_sockets.end();
}

void SocketThread::closeSocket(const std::set<std::shared_ptr<Socket*>>::iterator it) {
    this->p_sockets.erase(it);
}

void SocketThread::closeAllSockets() {
    this->p_sockets.erase(this->p_sockets.begin(), this->p_sockets.cend());
}

void SocketThread::startSocket(const std::set<std::shared_ptr<Socket*>>::iterator it) {
    ((Socket*)it->get())->startServer();
}

void SocketThread::stopSocket(const std::set<std::shared_ptr<Socket*>>::iterator it) {
    ((Socket*)it->get())->stopServer();
}

void SocketThread::send(const std::set<std::shared_ptr<Socket*>>::iterator it,
                        const std::string &remoteIp, const uint16_t remotePort, const Packet &packet) {
    send(it, IpPort{remoteIp, remotePort}, packet);
}


void SocketThread::send(const std::set<std::shared_ptr<Socket*>>::iterator it,
                        const std::string &remoteIp, const uint16_t remotePort, const Json &json) {
    send(it, IpPort{remoteIp, remotePort}, json);
}

void SocketThread::send(const std::set<std::shared_ptr<Socket*>>::iterator it,
                        const IpPort &remoteIpPort, const Packet &packet) {
    ((Socket*)it->get())->sendMsg(remoteIpPort, packet);
}

void SocketThread::send(const std::set<std::shared_ptr<Socket*>>::iterator it,
                        const IpPort &remoteIpPort, const Json &json) {
    ((Socket*)it->get())->sendMsg(remoteIpPort, json);
}

bool SocketThread::isActive() {
    return active;
}


void SocketThread::setCallbackSocketReadRawData(const Socket &s, void (*callback)(PacketMessage pm)) {
    this->packetCallback = callback;
}

void SocketThread::setCallbackSocketReadJsonData(const Socket &s, void (*callback)(JsonMessage jm)) {
    this->jsonCallback = callback;
}
