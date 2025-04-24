#include "ConfigCommon.h"
#include "ConfigDefines.h"

#include "../utils/Utils.h"

#include <fstream>
#include <regex>


std::string ToString(const ValueType type) noexcept {
    switch(type) {
    case eNumber:   return "Number";    break;
    case eBool:     return "Bool";      break;
    case eString:   return "String";    break;
    case eJson:     return "Json";      break;
    case eArray:    return "Array";     break;
    default:        return "null";      break;
    }
}

void RemoveIllegalSpaces(std::string &string) noexcept {
    if(!string.empty()) {
        while(utils::CharsInString(string.back(), __SPACES_WITHOUT_SEPARATORS__))
            string.pop_back();

        while(utils::CharsInString(*string.begin(), __SPACES_WITHOUT_SEPARATORS__))
            string = string.erase(0, 1);
    }
}

bool GetAllStringsFromFile(const std::string& path, std::string& dest_string,
                           std::string* error_log) noexcept {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            if(error_log) *error_log = "File not found";
            return false;
        }

        std::string temp_string;
        while(getline(file, temp_string))
            dest_string += temp_string + '\n';
        file.close();
    } catch (std::exception e) {
        if(error_log) *error_log = std::string("Unable to read file: ") + e.what();
        return false;
    }

    return true;
}

bool CreateEmptyFile(const std::string &file_path, const std::string &start_comment,
                     const std::string &finish_comment, std::string* error_log) noexcept {
    try{
        std::ofstream file(file_path);
        if (!file.is_open()) {
            if(error_log) *error_log = "File not found";
            return false;
        }

        file.write(start_comment.c_str(), start_comment.length());
        file.write("\n", 1);
        file.write("\n", 1);
        file.write(finish_comment.c_str(), finish_comment.length());
        file.write("\n", 1);
        file.close();
    } catch (std::exception e) {
        if(error_log) *error_log = std::string("Unable to write file: ") + e.what();
        return false;
    }

    return false;
}

ValueType CheckValue(std::string &value, const ConfigFormat &format) noexcept {
    //    std::cout << "CheckValue(): \"" << value << "\"" << std::endl;
    if(value.empty()) return eNull;

    using namespace utils;
    bool isValue = false;
    std::string _value;
    ValueType vType = eNull;
    for(size_t i = 0; i < value.length(); i++) {
        if(vType == ValueType::eNull) {
            //удалить пустоту в начале и конце строки
            RemoveIllegalSpaces(value);

            if(IsNumber(value[i])
                || CharsInString(value[i], "-+"))
                vType = ValueType::eNumber;
            else if(!CharsInString(value[i], __SPACES__)
                     && (value[0] == 't'
                         || value[0] == 'f'
                         || value[0] == 'T'
                         || value[0] == 'F'))
                vType = ValueType::eBool;
            else if(!CharsInString(value[i], __SPACES__)
                     && (value[0] == 'N' || value[0] == 'n'))
                vType = ValueType::eNull;
            else                            vType = ValueType::eString;
        }
        _value += value[i];
    }

    switch(vType) {
    case ValueType::eNumber:    {
        isValue = CheckNumber(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eBool:      {
        isValue = CheckBool(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eNull:      {
        isValue = CheckNull(_value);
        if(!isValue) {
            vType = ValueType::eString;
            isValue = CheckString(_value);
        }
        break;
    }
    case ValueType::eString:    { isValue = CheckString(_value, format); break; }
    default:                    return ValueType::eNull;
    }

    if(isValue) {
        value = _value;
        return vType;
    } else
        return ValueType::eNull;
}

bool CheckNumber(const std::string &value) noexcept {
    if(value.empty()) return false;
    if(value[0] == 'e' || value[0] == 'E' || value[0] == 'f' || value[0] == 'F')
        return false;

    using namespace utils;
    std::regex reg("^[+-]?[0-9]*[.]?[0-9]*[eE]?[+-]?[0-9]*[fF]?$");
    bool matched = std::regex_match(value, reg);

    bool e_is_last = value[value.length() - 1] == 'e' || value[value.length() - 1] == 'E';
    bool f_is_last = value[value.length() - 1] == 'f' || value[value.length() - 1] == 'F';

    if(f_is_last && value.length() > 1) {
        e_is_last = value[value.length() - 2] == 'e' || value[value.length() - 2] == 'E';
    }

    return matched && !e_is_last;
}

bool CheckBool(std::string &value) noexcept {
    //    std::cout << "CheckBool(): \"" << value << "\"" << std::endl;
    using namespace utils;
    std::string temp;
    bool flag = false;
    for(char c : value) {
        if(CharsInString(c, __SPACES__)) {
            if(flag) break;
        } else {
            if(!flag)   flag = true;
            if(flag)    temp += std::tolower(c);
        }
    }

    if(temp != "true" && temp != "false") return false;
    value = temp;
    return true;
}

bool CheckNull(std::string &value) noexcept {
    //    std::cout << "CheckNull(): \"" << value << "\"" << std::endl;
    using namespace utils;
    std::string temp;
    bool flag = false;
    for(char c : value) {
        if(CharsInString(c, __SPACES__)) {
            if(flag) break;
        } else {
            if(!flag)   flag = true;
            if(flag)    temp += std::tolower(c);
        }
    }

    if(temp != "null") return false;
    value = temp;
    return true;
}

bool CheckString(std::string &value, const ConfigFormat &format) noexcept {
    //    std::cout << "CheckString(): \"" << value << "\"" << std::endl;
    using namespace utils;

    //удалить пробелы в начале и конце строки
    RemoveIllegalSpaces(value);

    bool done = false;
    switch(format) {
    case ConfigFormat::eJSON: {
        if(value[0] == '"') {
            char ch = 0;
            std::string temp;
            for(size_t i = 0; i < value.length(); i++) {
                if(ch != 0) { //начинаем запись слова
                    if(!done) {
                        //экранированные символы ВСЕГДА заносятся в значение (все)
                        if(value[i] == '\\') {
                            if(value.length() > i + 1) {
                                temp += value[i];
                                temp += value[i+1];
                                i++;
                                continue;
                            } else {
                                std::cout << "Error with parse String in: " << value[i] << std::endl;
                                return false;
                            }
                        }

                        if(value[i] == '"') done = true;
                        else                temp += value[i];
                    } else { //замкнули слово, надо проверить оставшиеся символы
                        if(!CharsInString(value[i], __SPACES__)) {
                            std::cout << "Error with parse String in: " << value[i] << std::endl;
                            return false;
                        }
                    }
                } else if(value[i] == '"') {
                    ch = value[i];
                }
            }
            value = temp;
            return done;
        } else {
            for(char ch : value)
                if(CharsInString(ch, __SPACES__))
                    return false;
            return true;
        }
    }
    case ConfigFormat::eINI: {
        char ch = 0;
        std::string temp;
        for(size_t i = 0; i < value.length(); i++) {
            //экранированные символы ВСЕГДА заносятся в значение (все)
            if(value[i] == '\\') {
                if(value.length() > i + 1) {
                    temp += value[i] + value[i];
                    i++;
                    continue;
                } else {
                    std::cout << "Error with parse String in: " << value[i] << std::endl;
                    return false;
                }
            } else
                temp += value[i];
        }
        value = temp;
        return true;
    }
    case ConfigFormat::eYAML:
    default: break;
    }
    return false;
}

bool CheckJson(std::string &value) noexcept {
    //    std::cout << "CheckJson(): \"" << value << "\"" << std::endl;
    using namespace utils;
    char ch = 0;
    std::string temp;
    bool done = false;
    char innerWord = 0;
    uint32_t innerLvlFBrace = 0;
    uint32_t innerLvlQBrace = 0;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            if(value[i] == '"') {
                if(innerWord == 0)              innerWord = value[i];
                else if(value[i] == innerWord)  innerWord = 0;
            }

            if(!done) {
                if(innerWord == 0) {
                    if(value[i] == '{')  innerLvlFBrace++;
                    if(value[i] == '}')  innerLvlFBrace--;
                    if(value[i] == '[')  innerLvlQBrace++;
                    if(value[i] == ']')  innerLvlQBrace--;
                }
                if(value[i] == ch
                    && innerLvlFBrace == 0
                    && innerLvlQBrace == 0
                    && innerWord == 0) { //пока не встретили конец слова
                    done = true;
                }
                temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!CharsInString(value[i], __SPACES__)) {
                    std::cout << "Error with parse Json in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!CharsInString(value[i], __SPACES__)) {
            if (value[i] == '{') {
                temp += value[i];
                ch = '}';
                innerLvlFBrace++;
            } else
                return false;
        }
    }

    if(!done) return false;
    value = temp;
    return true;
}

//TODO: debug log in args
bool CheckArray(std::string &value) noexcept {
    //    std::cout << "CheckArray(): \"" << value << "\"" << std::endl;
    using namespace utils;
    char ch = 0;
    std::string temp;
    bool done = false;
    char innerWord = 0;
    uint32_t innerLvlFBrace = 0;
    uint32_t innerLvlQBrace = 0;
    for(size_t i = 0; i < value.length(); i++) {
        if(ch != 0) { //начинаем запись слова
            //экранированные кавычки ВСЕГДА заносится в значение
            if(value[i] == '\\') {
                if(value.length() > i + 1) {
                    temp += value[i] + value[i+1];
                    i++;
                    continue;
                } else {
                    std::cout << "Error with parse JArray in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }

            if(value[i] == '"') {
                if(innerWord == 0)              innerWord = value[i];
                else if(value[i] == innerWord)  innerWord = 0;
            }

            if(!done) {
                if(innerWord == 0) {
                    if(value[i] == '{')  innerLvlFBrace++;
                    if(value[i] == '}')  innerLvlFBrace--;
                    if(value[i] == '[')  innerLvlQBrace++;
                    if(value[i] == ']')  innerLvlQBrace--;
                }
                if(value[i] == ch
                    && innerLvlFBrace == 0
                    && innerLvlQBrace == 0
                    && innerWord == 0) { //пока не встретили конец слова
                    done = true;
                }
                temp += value[i];
            } else { //замкнули слово, надо проверить оставшиеся символы
                if(!CharsInString(value[i], __SPACES__)) {
                    std::cout << "Error with parse JArray in: [" << value[i] << "]" << std::endl;
                    return false;
                }
            }
        } else if(!CharsInString(value[i], __SPACES_WITHOUT_SEPARATORS__)) {
            if (value[i] == '[') {
                temp += value[i];
                ch = ']';
                innerLvlQBrace++;
            } else
                return false;
        }
    }

    if(!done) return false;
    value = temp;
    return true;
}
