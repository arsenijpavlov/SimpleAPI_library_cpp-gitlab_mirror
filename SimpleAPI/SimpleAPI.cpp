#include "SimpleAPI.h"

std::string GetVersion() {
    return std::string(SIMPLEAPI_VERSION);
}

std::string GetVersionName() {
    return std::string(SIMPLEAPI_VERSION_NAME);
}

std::string GetVersionCommit() {
    return std::string(SIMPLEAPI_VERSION_COMMIT);
}

std::string GetFullVersionInfo() {
    return std::string("SimpleAPI ")
           + std::string(SIMPLEAPI_VERSION)
           + " \"" + std::string(SIMPLEAPI_VERSION_NAME)
           + "\" from commit " + std::string(SIMPLEAPI_VERSION_COMMIT);
}
