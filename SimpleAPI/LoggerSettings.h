#ifndef LOGGER_SETTINGS_H
#define LOGGER_SETTINGS_H

#include "Logger.h"
#include <functional>
//TODO: add "noexcept"

class LoggerSettings {
public:
    using LogCallback = std::function<void(std::string)>;

protected:
    bool                m_log_level_print;
    logs::LEVEL         m_log_level;
    bool                m_log_time_print;
    int                 m_name_column_size;
    bool                m_name_column_right_align;

    LogCallback         m_log_callback;
    LogCallback         m_log_error_callback;
    LogCallback         m_color_log_callback;
    LogCallback         m_color_log_error_callback;

public:
    LoggerSettings() :
        m_log_level_print           (true),
        m_log_level                 (logs::eINFO),
        m_log_time_print            (false),
        m_name_column_size          (-1),
        m_name_column_right_align   (false),
        m_log_callback              (nullptr),
        m_log_error_callback        (nullptr),
        m_color_log_callback        (nullptr),
        m_color_log_error_callback  (nullptr)
    {}

    void enablePrintLogLevel(bool enabled = false)                  { m_log_level_print = enabled; }
    bool isPrintLogLevelEnabled()                                   { return m_log_level_print; }
    void enableLogTime(bool enabled = false)                        { m_log_time_print = enabled; }
    bool isLogTimeEnabled()                                         { return m_log_time_print; }
    void enableNameColumnRightAlign(bool enabled = false)           { m_name_column_right_align = enabled; }
    bool isNameColumnRightAlignEnabled()                            { return m_name_column_right_align; }

    void setNameColumnSize(int size = -1)                           { m_name_column_size = size; }
    void setLogLevel(logs::LEVEL level = logs::eINFO)               { m_log_level = level; }
    void setLogCallback(LogCallback callback = nullptr)             { m_log_callback = callback; }
    void setLogErrorCallback(LogCallback callback = nullptr)        { m_log_error_callback = callback; }
    void setColorLogCallback(LogCallback callback = nullptr)        { m_color_log_callback = callback; }
    void setColorLogErrorCallback(LogCallback callback = nullptr)   { m_color_log_error_callback = callback; }

    int         getNameColumnSize()                                 { return m_name_column_size; }
    logs::LEVEL getLogLevel()                                       { return m_log_level; }
    LogCallback getLogCallback()                                    { return m_log_callback; }
    LogCallback getLogErrorCallback()                               { return m_log_error_callback; }
    LogCallback getColorLogCallback()                               { return m_color_log_callback; }
    LogCallback getColorLogErrorCallback()                          { return m_color_log_error_callback; }
};

#endif // LOGGER_SETTINGS_H
