#ifndef JSON_SETTINGS_H
#define JSON_SETTINGS_H

#include "Json.h"

#include <string>

/* Стиль комментариев в файле:
 *
 * # go;ijnh;bofdj;ov
 *
 * или
 *
 * ###################################
 * # gindooldgbmnd
 * # dfsnmjfpsdkjfd's[ok ['d
 * #
 * # dfsdm'gopsk'[
 * ###################################
 *
 */

enum class CommentType {
    eBeforeValue,           // пользователь сам распределяет переносы строки
    eBeforeValueMultiLine,  // автоматическая расстановка переносов строки
    eAfterValueOneLine      // подсказка на той же строке
};

/* группа значений
 *  - комментарий
 *  - Json для хранения значений и комментариев к ним
 *
*/
class SettingsGroup {

public:
    SettingsGroup(const std::string& name) {}
};

/* основной класс настроек
 *
*/
class Settings {
    Json m_settings;            //по задумке, они должны идти ДО объявленных в группе
    Json m_settings_in_groups;
    Json m_comments;
    Json m_comments_in_groups;

    std::vector<std::string> m_current_group;
    char m_comment_sym; //многострочные комментарии всегда имеют этот символ в начале строки
    uint8_t m_comment_column_size;

public:
    Settings() : m_comment_column_size(60) {
        m_current_group.reserve(5); //преаллокация для 5 уровней вложенности
    }

    bool        loadSettings(const std::string& path);
    bool        saveSettings(const std::string& path);

    void        beginGroup(const std::string& name);
    void        endGroup();

    void        setOrUpdate(const std::string& key, const Element& value,
                            const std::string& comment = "",
                     const CommentType comment_type = CommentType::eBeforeValue);
    Element&    getOrDefault(const std::string& key, const Element& default_value);


    void        setColumnWidth(const uint8_t new_width = 60);

};

#endif // JSON_SETTINGS_H
