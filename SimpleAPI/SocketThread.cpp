#include "SocketThread.h"




ReceivedPacket SocketThread::setCallbackSocketReadRawData(const Socket &s, ReceivedPacket (*callback)())
{
    if(callback != nullptr) return callback();
    else                    return {};
}

ReceivedJson SocketThread::setCallbackSocketReadJsonData(const Socket &s, ReceivedJson (*callback)())
{
    if(callback != nullptr) return callback();
    else                    return {};
}
