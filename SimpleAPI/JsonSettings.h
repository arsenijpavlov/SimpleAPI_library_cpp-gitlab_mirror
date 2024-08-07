#ifndef JSON_SETTINGS_H
#define JSON_SETTINGS_H

#include "Json.h"

#include <string>

class JsonSettings {
    Json m_settings_without_groups; //по задумке, они должны идти ДО объявленных в группе
    Json m_settings_in_groups;
    Json m_comments;

    std::vector<std::string> m_current_group;
    std::string m_one_comment;
    std::string m_start_multi_line_comment;
    std::string m_end_multi_line_comment;

public:
    JsonSettings(){
        m_current_group.reserve(5); //преаллокация для 5 уровней вложенности
    }

    bool        loadSettings(const std::string& path);
    bool        saveSettings(const std::string& path);

    void        beginGroup(const std::string& name);
    void        endGroup();
    void        setOrUpdate(const std::string& key, const Element& value,
                            const std::string& comment = "", const bool use_auto_new_liner = false);
    Element&    getOrDefault(const std::string& key, const Element& default_value);

};

#endif // JSON_SETTINGS_H
