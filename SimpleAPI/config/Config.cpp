#include "Config.h"

#include "ElementArray.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementJson.h"


void Config::init() noexcept {
    m_type      = ValueType::eNull;
    m_element   = nullptr;
}

Config &Config::operator=(const Config &other) noexcept {
    return *this;
}

void Config::setValue(const Config &value) noexcept {
    switch(value.getValueType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
//TODO:    case ValueType::eJson:
//TODO:    case ValueType::eArray:     setValue(value.get)
    default:                    init();
    }
}

void Config::setValue(const IElement *value) noexcept {
//TODO: Config::setValue(IElement*)
}

void Config::setValue(const bool value) noexcept {
    m_type      = ValueType::eBool;
    m_element   = std::make_shared<IElement>(ElementBool(value));
}

void Config::setValue(const long double& value) noexcept {
    m_type      = ValueType::eNumber;
    m_element   = std::make_shared<IElement>(ElementNumber(value));
}

void Config::setValue(const std::string& value) noexcept {
    m_type      = ValueType::eString;
    m_element   = std::make_shared<IElement>(ElementString(value));
}

void Config::setValue(const ElementArray &value) noexcept {
    m_type      = ValueType::eArray;
    m_element   = std::make_shared<IElement>(ElementArray(value));
}

void Config::setValue(const ElementJson &value) noexcept {
    m_type      = ValueType::eJson;
    m_element   = nullptr;
//TODO:    m_element   = std::make_shared<IElement>(ElementJson(value));
}


