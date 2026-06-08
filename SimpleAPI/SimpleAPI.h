#pragma once

#include "VersionInfo.h"
#include "utils/Utils.h"
#include "utils/Logger.h"

#include "config/Config.h"

#include "network/Socket.h"
#include "network/SocketThread.h"


namespace simpleapi {

// функции для получения информации о библиотеке SimpleAPI
std::string GetVersion();
std::string GetVersionName();
std::string GetVersionCommit();
std::string GetFullVersionInfo();

//TODO: нужно обновление скрипта check_version_from_git.sh для параллельного выполнения в пределах Makefile (QMake)
//TODO: описать в документации версионирование проекта

} // namespace simpleapi
