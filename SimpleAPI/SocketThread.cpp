#include "SocketThread.h"




PacketMessage SocketThread::setCallbackSocketReadRawData(const Socket &s, PacketMessage (*callback)())
{
    if(callback != nullptr) return callback();
    else                    return {};
}

JsonMessage SocketThread::setCallbackSocketReadJsonData(const Socket &s, JsonMessage (*callback)())
{
    if(callback != nullptr) return callback();
    else                    return {};
}
