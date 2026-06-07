#pragma once

#include "utils/Utils.h"
#include "utils/Logger.h"

#include "config/Config.h"

#include "network/Socket.h"
#include "network/SocketThread.h"

// функции для получения информации о библиотеке SimpleAPI
std::string GetVersion();
std::string GetVersionName();
std::string GetVersionCommit();
std::string GetFullVersionInfo();
