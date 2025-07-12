#include "IElement.h"

#include "../../utils/Utils.h"

#include "../ElementBool.h"
#include "../ElementNumber.h"
#include "../ElementString.h"
#include "../ElementArray.h"
#include "../ElementJson.h"



//void IElement::setValue(const long double &value) noexcept {
//    *this = static_cast<IElement>(ElementNumber(value));
//}

//void IElement::setValue(const bool &value) noexcept {
//    *this = static_cast<IElement>(ElementBool(value));
//}

//void IElement::setValue(const std::string &value) noexcept {
//    *this = static_cast<IElement>(ElementString(value));
//}

//bool IElement::getBool() const {
//    __CHECK_TYPE_IS_BOOL__
//    return dynamic_cast<const ElementBool&>(*this).getValue();
//}

//bool& IElement::getBool() {
//    __CHECK_TYPE_IS_BOOL__
//    return dynamic_cast<ElementBool&>(*this).getValue();
//}

//long double IElement::getNumber() const {
//    __CHECK_TYPE_IS_NUMBER__
//    return dynamic_cast<const ElementNumber&>(*this).getValue();
//}

//long double& IElement::getNumber() {
//    __CHECK_TYPE_IS_NUMBER__
//    return dynamic_cast<ElementNumber&>(*this).getValue();
//}

//std::string IElement::getString() const {
//    __CHECK_TYPE_IS_STRING__
//    return dynamic_cast<const ElementString&>(*this).getValue();
//}

//std::string& IElement::getString() {
//    __CHECK_TYPE_IS_STRING__
//    auto& ptr = dynamic_cast<ElementString&>(*this);
//    return dynamic_cast<ElementString&>(*this).getValue();
//}

//ElementArray IElement::getArray() const {
//    __CHECK_TYPE_IS_ARRAY__
//    return dynamic_cast<const ElementArray&>(*this);
//}

//ElementArray& IElement::getArray() {
//    __CHECK_TYPE_IS_ARRAY__
//    return dynamic_cast<ElementArray&>(*this);
//}

////TODO: ElementJson IElement::getJson() const
////ElementJson IElement::getJson() {
////    __CHECK_TYPE_IS_JSON__
////    return dynamic_cast<const ElementJson&>(*this);
////}

////TODO: ElementJson& IElement::getJson()
////ElementJson& IElement::getJson() {
////    __CHECK_TYPE_IS_JSON__
////    return dynamic_cast<ElementJson&>(*this);
////}

//bool IElement::writeFile(const std::string &file_path, const ConfigFormat format,
//                         const bool with_comments) noexcept {
//    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
//}

//bool IElement::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
//    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
//}

//bool IElement::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
//    return CreateEmptyFile(file_path, m_comment.prefix(), m_comment.suffix());
//}

////IElement &IElement::operator=(const IElement &other) noexcept {

////}


////----------------------------------------------------------------------------------------------------------------------


//IElement ReadFile(const std::string &file_path, const ConfigFormat format,
//                  const bool with_comments, std::string* error_log) {
//    std::string dest;
//    GetAllStringsFromFile(file_path, dest, error_log);
//    return Parse(dest, format, with_comments, error_log);
//}

//IElement ReadFileJson(const std::string &file_path, const bool with_comments,
//                     std::string* error_log) {
//    std::string dest;
//    GetAllStringsFromFile(file_path, dest, error_log);
//    return ParseJson(dest, with_comments, error_log);
//}

//IElement ReadFileIni(const std::string &file_path, const bool with_comments,
//                    std::string* error_log) {
//    std::string dest;
//    GetAllStringsFromFile(file_path, dest, error_log);
//    return ParseIni(dest, with_comments, error_log);
//}

//IElement Parse(const std::string &content, const ConfigFormat format,
//              const bool with_comments, std::string* error_log) {
//    switch(format) {
//    case ConfigFormat::eJSON:   return ParseJson(content, with_comments, error_log);
//    case ConfigFormat::eINI:    return ParseIni(content, with_comments, error_log);
//    default:                    return IElement();
//    }
//}

//IElement ParseJson(const std::string &file_path, const bool with_comments,
//                  std::string* error_log) {
//    //TODO: IElement &ParseJson()
//    return IElement();
//}

//IElement ParseIni(const std::string &file_path, const bool with_comments,
//                 std::string* error_log) {
//    //TODO: IElement &ParseIni()
//    return IElement();
//}

//NOTE: каждый дочерний класс должен описать эту функцию
IElement &IElement::operator=(const IElement &other) {
    if(this != &other) {
        switch(other.getType()) {
        case ValueType::eNull:
        case ValueType::eNumber:
        case ValueType::eBool:
        case ValueType::eString:
        case ValueType::eJson:
        case ValueType::eArray:     this = dynamic_cast<ElementArray*>(ElementArray(other));
        default:                    break;
        }
    }

    return *this;
}

//NOTE: каждый дочерний класс должен описать эту функцию
IElement &IElement::operator=(IElement &&other) noexcept {
    if(this != &other) {
        m_type      = other.m_type;
        m_comment   = other.m_comment;
    }

    return *this;
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
//числа, контейнеры(размер), строки(количество выводимых СИМВОЛОВ)
bool IElement::operator>(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL__((*this))
    __CHECK_TYPES_IS_EQUAL__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(this)->getValue() >
               dynamic_cast<const ElementBool*>(this)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(this)->getValue() >
               dynamic_cast<const ElementNumber*>(this)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue()) >
               GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() > other.size();
    default: return false;
    }
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
bool IElement::operator>=(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL__((*this))
    __CHECK_TYPES_IS_EQUAL__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(this)->getValue() >=
               dynamic_cast<const ElementBool*>(this)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(this)->getValue() >=
               dynamic_cast<const ElementNumber*>(this)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue()) >=
               GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() >= other.size();
    default: return false;
    }
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
bool IElement::operator<(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL__((*this))
    __CHECK_TYPES_IS_EQUAL__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(this)->getValue() <
               dynamic_cast<const ElementBool*>(this)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(this)->getValue() <
               dynamic_cast<const ElementNumber*>(this)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue()) <
               GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() < other.size();
    default: return false;
    }
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
bool IElement::operator<=(const IElement& other) const noexcept {
    __CHECK_TYPE_IS_NOT_NULL__((*this))
    __CHECK_TYPES_IS_EQUAL__((*this), other)

    using namespace utils;
    switch(getType()) {
    case ValueType::eBool:
        return dynamic_cast<const ElementBool*>(this)->getValue() <=
               dynamic_cast<const ElementBool*>(this)->getValue();
    case ValueType::eNumber:
        return dynamic_cast<const ElementNumber*>(this)->getValue() <=
               dynamic_cast<const ElementNumber*>(this)->getValue();
    case ValueType::eString:
        return GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue()) <=
               GetStringCharCount(dynamic_cast<const ElementString*>(this)->getValue());
    case ValueType::eJson:
    case ValueType::eArray:
        return size() <= other.size();
    default: return false;
    }
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
IElement& IElement::operator[](const size_t index) noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return get_at(index);
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
IElement IElement::operator[](const size_t index) const noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return get_at(index);
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
IElement& IElement::operator[](const std::string key) noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return get_at(key);
}

//NOTE: ??? возможно, нужно переместить в конкретные классы ???
IElement IElement::operator[](const std::string key) const noexcept {
    __CHECK_TYPE_IS_CONTAINER__((*this))
    return get_at(key);
}

bool IElement::isContainer() const noexcept {
    switch(getType()) {
    //контейнерные типы
    case ValueType::eJson:
    case ValueType::eArray:     return true;
    //все остыльные
    case ValueType::eNull:
    case ValueType::eNumber:
    case ValueType::eBool:
    case ValueType::eString:    break;
    }

    return false;
}

size_t IElement::size() const noexcept {
    switch(getType()) {
    case ValueType::eNull:      return 0; //TODO: размер NULL элемента под вопросом
    case ValueType::eNumber:
    case ValueType::eBool:
    case ValueType::eString:    return 1;
    case ValueType::eArray:     return dynamic_cast<const ElementArray*>(this)->size();
    case ValueType::eJson:      return dynamic_cast<const ElementJson*>(this)->size();
//    default:                    return 0;
    }
}

bool IElement::isEqual(const IElement &other, const bool compare_comments) const noexcept {
    //FIXME:
    return false;
}

bool &IElement::getBool() {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<ElementBool*>(this)->getValue();
}

bool IElement::getBool() const {
    __CHECK_TYPE_IS_BOOL__((*this))
    return dynamic_cast<const ElementBool*>(this)->getValue();
}

long double &IElement::getNumber() {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<ElementNumber*>(this)->getValue();
}

long double IElement::getNumber() const {
    __CHECK_TYPE_IS_NUMBER__((*this))
    return dynamic_cast<const ElementNumber*>(this)->getValue();
}

std::string &IElement::getString() {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<ElementString*>(this)->getValue();
}

std::string IElement::getString() const {
    __CHECK_TYPE_IS_STRING__((*this))
    return dynamic_cast<const ElementString*>(this)->getValue();
}

ElementArray &IElement::getArray() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray&>(*this);
}

ElementArray IElement::getArray() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<const ElementArray&>(*this);
}

ElementJson &IElement::getJson() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson&>(*this);
}

ElementJson IElement::getJson() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<const ElementJson&>(*this);
}

IElement &IElement::get_front() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(this)->get_front();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(this)->get_front();
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return *this;
}

//FIXME: const element ???!
IElement IElement::get_front() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    IElement e;

    switch(getType()) {
    case ValueType::eArray: e = reinterpret_cast<const ElementArray*>(this)->get_front(); //решение?
    case ValueType::eJson:  return dynamic_cast<const ElementJson*>(this)->get_front();
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return *this;
}

IElement &IElement::get_at(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(this)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(this)->get_at(index);
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return *this;
}

//FIXME: const element ???!
IElement IElement::get_at(const size_t index) const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(this)->get_at(index);
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(this)->get_at(index);
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return *this;
}

IElement &IElement::get_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return dynamic_cast<ElementArray*>(this)->get_back();
    case ValueType::eJson:  return dynamic_cast<ElementJson*>(this)->get_back();
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return *this;
}

//FIXME: const element ???!
IElement IElement::get_back() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return IElement(dynamic_cast<ElementArray*>(this)->get_back());
    case ValueType::eJson:  return IElement(dynamic_cast<ElementJson*>(this)->get_back());
//NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return IElement();
}














