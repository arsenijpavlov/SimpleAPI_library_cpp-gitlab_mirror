#include "JsonElementArray.h"
#include <stdexcept>


//предобъявление
#include "JsonElementJson.h"
class ElementJson;

ElementArray::ElementArray(const std::string &string, const ConfigFormat format,
                           const bool enable_comments) noexcept {
    init();
    parseArray(string, enable_comments, format);
}

void ElementArray::parseArray(const std::string &string, const bool enable_comment,
                              const ConfigFormat config_format) {
    //TODO: ElementArray::parseArray()
    //TODO: std::exception
}

void ElementArray::parseJSON_array(const std::string &string, const bool enable_comment) {
    //TODO: ElementArray::parseJSON_array()
    //TODO: std::exception
}

void ElementArray::parseINI_array(const std::string &string, const bool enable_comment) {
    //TODO: ElementArray::parseINI_array()
    //TODO: std::exception
}

void ElementArray::addComment(const size_t index, const Comment &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addComment(content);
}

void ElementArray::addComment(const size_t index, const std::string &content_before,
                              const std::string &content_after) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addComment(content_before, content_after);
}

void ElementArray::addPrefixComment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addPrefixComment(content);
}

void ElementArray::addSuffixComment(const size_t index, const std::string &content) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values[index].addSuffixComment(content);
}

Comment& ElementArray::getComment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getComment();
}

Comment ElementArray::getComment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getComment();
}

std::string& ElementArray::getPrefixComment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getPrefixComment();
}

std::string ElementArray::getPrefixComment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getPrefixComment();
}

std::string& ElementArray::getSuffixComment(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getSuffixComment();
}

std::string ElementArray::getSuffixComment(const size_t index) const {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getSuffixComment();
}

void ElementArray::clearComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteComment();
}

void ElementArray::clearPrefixComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deletePrefixComment();
}

void ElementArray::clearSuffixComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteSuffixComment();
}

void ElementArray::deleteComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteComment();
}

void ElementArray::deletePrefixComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deletePrefixComment();
}

void ElementArray::deleteSuffixComment(const size_t index) noexcept {
    __IF_INDEX_NOT_BOUND2__(m_values, index)
    m_values[index].deleteSuffixComment();
}

ElementArray &ElementArray::clear() noexcept {
    m_values.clear();
    return *this;
}

bool ElementArray::isEqual(const Element &other, const bool compare_comments) const noexcept {
    if(m_values != dynamic_cast<const ElementArray&>(other).m_values)   return false;
    if(compare_comments && m_comment == other.getComment())             return false;

    return true;
}

ValueType ElementArray::getTypeFront() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    return m_values.front().getType();
}

ValueType ElementArray::getTypeAt(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index].getType();
}

ValueType ElementArray::getTypeBack() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    return m_values.back().getType();
}

ElementArray& ElementArray::pushFront(const Element &element) noexcept {
    m_values.insert(m_values.cbegin(), element);
    return *this;
}

ElementArray& ElementArray::pushAt(const Element &element, const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index - 1) //проверяем, что size() < index - 1 (+1[нумерация с 0])
    m_values.insert(m_values.cbegin() + index, element);
    return *this;
}

ElementArray& ElementArray::pushBack(const Element &element) noexcept {
    m_values.push_back(element);
    return *this;
}

Element& ElementArray::getValue(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    return m_values[index];
}

Element& ElementArray::getValue(const VString& complex_key) {
    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key vector is empty");

    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)

    if(complex_key.front().empty())
        throw std::invalid_argument("complex_key argument cannot be empty");

    size_t current_index;
    try         { current_index = std::stoi(complex_key.front()); }
    catch(...)  { __ARRAY_INCORRECT_INDEX_EXCEPTION__ }

    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, current_index)

    if(complex_key.size() == 1)
        return (*this)[current_index];
    else {
        Element& el = (*this)[current_index];

        auto new_complex_key = complex_key;
        new_complex_key.erase(new_complex_key.begin());
        switch(el.getType()) {
        case eJson:     return dynamic_cast<ElementJson&>((*this)[current_index])[new_complex_key];
        case eArray:    return dynamic_cast<ElementArray&>((*this)[current_index])[new_complex_key];
        default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }
}

Element& ElementArray::getValue(const std::vector<size_t>& complex_key) {
    if(complex_key.size() == 0)
        throw std::invalid_argument("complex_key vector is empty");

    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)

    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, complex_key.front())

    if(complex_key.size() == 1)
        return (*this)[complex_key.front()];
    else {
        Element& el = (*this)[complex_key.front()];

        auto new_complex_key = complex_key;
        new_complex_key.erase(new_complex_key.begin());
        switch(el.getType()) {
        case eJson:     return dynamic_cast<ElementJson&>((*this)[complex_key.front()]).getValue(new_complex_key);
        case eArray:    return dynamic_cast<ElementArray&>((*this)[complex_key.front()]).getValue(new_complex_key);
        default: __INCORRECT_TYPE_ELEMENT_FOR_INDEX_EXCEPTION__
        }
    }
}

ElementArray &ElementArray::popFront() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    m_values.erase(m_values.cbegin());
    return *this;
}

ElementArray &ElementArray::popAt(const size_t index) {
    __CHECK_INDEX_BOUND2_EXCEPTION__(m_values, index)
    m_values.erase(m_values.cbegin() + index);
    return *this;
}

ElementArray &ElementArray::popBack() {
    __CHECK_ARRAY_EMPTY_EXCEPTION__(m_values)
    m_values.pop_back();
    return *this;
}

ElementArray &ElementArray::erase(const size_t index) {
    popAt(index);
    return *this;
}

ElementArray &ElementArray::erase(const VElement::iterator &iterator) {
    m_values.erase(iterator);
    return *this;
}

ElementArray &ElementArray::erase(const VElement::iterator &begin, const VElement::iterator &end) {
    m_values.erase(begin, end);
    return *this;
}

ElementArray &ElementArray::erase(const std::vector<size_t> indexes) {
    for(auto i : indexes) popAt(i);
    return *this;
}

void ElementArray::operator=(const std::vector<Element> &other) noexcept {
    m_values = other;
}


bool IsElementArray(const std::string &str, const ConfigFormat format) noexcept {
    switch(format) {
    case ConfigFormat::eJSON:   return IsElementJsonArray(str);
    case ConfigFormat::eINI:    return IsElementIniArray(str);
//case:    case ConfigFormat::eYAML:
//case:    case ConfigFormat::eXML:
    default: return false;
    }

    return false;
}

bool IsElementJsonArray(const std::string &str) noexcept {
    //TODO: IsElementJsonArray()
    return false;
}

bool IsElementIniArray(const std::string &str) noexcept {
    //TODO: IsElementIniArray()
    return false;
}
