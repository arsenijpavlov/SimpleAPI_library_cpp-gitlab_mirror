#include "JsonElementString.h"
#include "JsonDefines.h"

#include "../utils/Utils.h"


std::string ElementString::to_string(const ConfigFormat format, const bool to_one_line) const noexcept {
    return m_value;
}

bool ElementString::isEqual(const Element &other, const bool compare_comments) const noexcept {
    if(m_value != dynamic_cast<const ElementString&>(other).m_value)        return false;
    if(compare_comments) {
        if(m_preview_comment == other.getPreviewComment())  return false;
        if(m_suffix_comment == other.getSuffixComment())    return false;
    }

    return true;
}

void ElementString::operator<<(const std::string &other) noexcept {
    m_value.append(other);
}

void ElementString::operator=(const std::string &other) noexcept {
    m_value = other;
}


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
                        if(!utils::CharsInString(str[i], __SPACES__)) {
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
            for(char ch : str)
                //TODO: пробелов уже быть не может! Надо сразу ошибку возвращать!
                if(utils::CharsInString(ch, __SPACES__))
                    return false;
            return true;
        }
    }
    case ConfigFormat::eINI: {
        //TODO: подумать, что делать с переносами строк

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

bool IsElementString(const Element &e) noexcept {
    return e.getType() == ValueType::eString;
}
