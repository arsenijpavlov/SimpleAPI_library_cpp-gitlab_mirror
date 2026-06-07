#include "SimpleAPI.h"


std::string simpleapi::GetVersion() {
    return std::string(SIMPLEAPI_VERSION);
}

std::string simpleapi::GetVersionName() {
    return std::string(SIMPLEAPI_VERSION_NAME);
}

std::string simpleapi::GetVersionCommit() {
    return std::string(SIMPLEAPI_VERSION_COMMIT);
}

std::string simpleapi::GetFullVersionInfo() {
    return std::string("SimpleAPI ")
           + std::string(SIMPLEAPI_VERSION)
           + " \"" + std::string(SIMPLEAPI_VERSION_NAME)
           + "\" from commit " + std::string(SIMPLEAPI_VERSION_COMMIT);
}
