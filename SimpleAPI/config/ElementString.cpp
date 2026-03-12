#include "ElementString.h"

#include "Config.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"
#include <algorithm>


void ElementString::clear() noexcept {
    clearComment();
    m_value.clear();
}

bool ElementString::isEqual(const IElement &other, const bool compare_comments,
                            const bool map_sort_important) const noexcept
{
    bool b1 = !compare_comments || isCommentsEqual(other);
    bool b2 = m_value == reinterpret_cast<const ElementString&>(other).getValue();

    return b1 && b2;
}

std::string ElementString::toString(const ConfigFormat format, const CommentDesign &design,
                                    const int8_t custom_tabulation_level) const noexcept
{
    return (format == ConfigFormat::eONLY_VALUE ? m_value : utils::AddQuotes(m_value));
}

std::string ElementString::toJsonString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toIniString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toYamlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

std::string ElementString::toXmlString(const CommentDesign &design, const int8_t custom_tabulation_level) const noexcept
{
    return utils::AddQuotes(m_value);
}

//----------------------------------------------------------------------------------------------------------------------


bool IsElementString(std::string &str, const ConfigFormat format,
                     std::string* error_log) noexcept {
    //удалить пробелы в начале и конце строки
    RemoveIllegalSpaces(str);

    bool done   = false;
    char ch     = 0;
    std::string temp;

    switch(format) {
    case ConfigFormat::eJSON: {
        if(str[0] == '"') {
            for(size_t i = 0; i < str.length(); i++) {
                if(ch != 0) { //начинаем запись слова
                    if(!done) {
                        //экранированные символы ВСЕГДА заносятся в значение (все)
                        if(str[i] == '\\') {
                            if(str.length() > i + 1) {
                                temp += str[i];
                                temp += str[i+1];
                                i++;
                                continue;
                            } else {
                                if(error_log) *error_log = std::string("Error with parse String in: ") + str[i];
                                return false;
                            }
                        }

                        if(str[i] == '"')   done = true;
                        else                temp += str[i];
                    } else { //замкнули слово, надо проверить оставшиеся символы
                        //TODO: пробелов уже быть не может! Надо сразу ошибку возвращать!
                        if(!utils::CharInString(str[i], __SPACES__)) {
                            if(error_log) *error_log = std::string("Error with parse String in: ") + str[i];
                            return false;
                        }
                    }
                } else if(str[i] == '"') {
                    ch = str[i];
                }
            }
            str = temp;
            return done;
        } else {
            //TODO: пробелов уже быть не может! Надо сразу ошибку возвращать!
            return !std::any_of(str.cbegin(), str.cend(),
                                [](const char ch) { return utils::CharInString(ch, __SPACES__); });
        }
    }
    case ConfigFormat::eINI: {
        //TODO: подумать, что делать с переносами строк (нужна ли табуляция)

        for(size_t i = 0; i < str.length(); i++) {
            //экранированные символы ВСЕГДА заносятся в значение (все)
            if(str[i] == '\\') {
                if(str.length() > i + 1) {
                    temp += str[i] + str[i];
                    i++;
                    continue;
                } else {
                    if(error_log) *error_log = std::string("Error with parse String in: ") + str[i];
                    return false;
                }
            } else
                temp += str[i];
        }
        str = temp;
        return true;
    }
    default: return false;
    }
}

bool IsElementString(const IElement &e) noexcept {
    return e.getType() == ValueType::eString;
}

bool IsElementString(const Config &cfg) noexcept {
    return cfg.isString();
}
