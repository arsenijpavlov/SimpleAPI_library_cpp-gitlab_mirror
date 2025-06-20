#include "ElementString.h"

#include "Config.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"


std::string ElementString::toString(const ConfigFormat format, const int8_t tabulation_level,
                                    const CommentDesign &design) const noexcept
{
    //TODO: ElementString::toString()
    return "";
}

std::string ElementString::toJsonString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementString::toJsonString()
    return "";
}

std::string ElementString::toIniString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementString::toIniString()
    return "";
}

std::string ElementString::toYamlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementString::toYamlString()
    return "";
}

std::string ElementString::toXmlString(const int8_t tabulation_level, const CommentDesign &design) const noexcept
{
    //TODO: std::string ElementString::toXmlString()
    return "";
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

bool IsElementString(const IElement &e) noexcept {
    return e.getType() == ValueType::eString;
}

bool IsElementString(const Config &cfg) noexcept {
    return cfg.isString();
}
