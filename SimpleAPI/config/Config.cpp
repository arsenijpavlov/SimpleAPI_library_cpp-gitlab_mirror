#include "Config.h"

#include "ElementNull.h"
#include "ElementBool.h"
#include "ElementNumber.h"
#include "ElementString.h"
#include "ElementArray.h"
#include "ElementJson.h"
#include "ElementYaml.h"
#include "ElementXml.h"


Config &Config::setValue() noexcept {
    m_type  = ValueType::eNull;
    m_value = dynamic_cast<IElement*>(new ElementNull());
    return *this;
}

Config &Config::setValue(const Config &value) noexcept {
    switch(value.getType()) {
    case ValueType::eNull:      return setValue();
    case ValueType::eBool:      return setValue(value.getBool());
    case ValueType::eNumber:    return setValue(value.getNumber());
    case ValueType::eString:    return setValue(value.getString());
    case ValueType::eArray:     return setValue(value.getArray());
    case ValueType::eJson:      return setValue(value.getJson());
    default: break;
    }

    return *this;
}

Config &Config::setValue(Config &&value) noexcept {
    switch(value.getType()) {
    case ValueType::eNull:      return setValue();
    case ValueType::eBool:      return setValue(value.getBool());
    case ValueType::eNumber:    return setValue(std::move(value.getNumber()));
    case ValueType::eString:    return setValue(std::move(value.getString()));
    case ValueType::eArray:     return setValue(std::move(value.getArray()));
    case ValueType::eJson:      return setValue(std::move(value.getJson()));
    default: break;
    }

    return *this;
}

Config &Config::setValue(const bool value) noexcept {
    m_type  = ValueType::eBool;
    m_value = dynamic_cast<IElement*>(new ElementBool(value));
    return *this;
}

Config &Config::setValue(const long double &value) noexcept {
    m_type  = ValueType::eNumber;
    m_value = dynamic_cast<IElement*>(new ElementNumber(value));
    return *this;
}

Config &Config::setValue(long double &&value) noexcept {
    m_type  = ValueType::eNumber;
    m_value = dynamic_cast<IElement*>(new ElementNumber(std::move(value)));
    return *this;
}

Config &Config::setValue(const std::string &value) noexcept {
    m_type  = ValueType::eString;
    m_value = dynamic_cast<IElement*>(new ElementString(value));
    return *this;
}

Config &Config::setValue(std::string &&value) noexcept {
    m_type  = ValueType::eString;
    m_value = dynamic_cast<IElement*>(new ElementString(std::move(value)));
    return *this;
}

Config &Config::setValue(const ElementArray &value) noexcept {
    m_type  = ValueType::eArray;
    m_value = dynamic_cast<IElement*>(new ElementArray(value));
    return *this;
}

Config &Config::setValue(ElementArray &&value) noexcept {
    m_type  = ValueType::eArray;
    m_value = dynamic_cast<IElement*>(new ElementArray(std::move(value)));
    return *this;
}

Config &Config::setValue(const ElementJson &value) noexcept {
    m_type  = ValueType::eJson;
    m_value = dynamic_cast<IElement*>(new ElementJson(value));
    return *this;
}

Config &Config::setValue(ElementJson &&value) noexcept {
    m_type  = ValueType::eJson;
    m_value = dynamic_cast<IElement*>(new ElementJson(std::move(value)));
    return *this;
}

Config &Config::operator=(const Config &other) noexcept {
    m_type  = other.getType();
    m_value = new IElement(*other.m_value);
    return *this;
}

Config &Config::operator=(Config &&other) noexcept {
    m_type  = other.getType();
    m_value = other.m_value;
    delete other.m_value;

    return *this;
}

