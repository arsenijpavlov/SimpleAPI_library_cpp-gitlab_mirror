#include "JsonElementArray.h"



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

std::string ElementArray::to_string(const ConfigFormat format,
                                    const bool to_one_line) const noexcept {
    //TODO: to_one_line

    return "";
}

void ElementArray::addComment(const size_t index, const Comment &content) {
    //TODO: ElementArray::addComment()
    //TODO: std::exception
}

void ElementArray::addComment(const size_t index, const std::string &contentBefore,
                              const std::string &contentAfter) {
    //TODO: ElementArray::addComment()
    //TODO: std::exception
}

void ElementArray::addPrefixComment(const size_t index, const std::string &content) {
    //TODO: ElementArray::addPrefixComment()
    //TODO: std::exception
}

void ElementArray::addSuffixComment(const size_t index, const std::string &content) {
    //TODO: ElementArray::addSuffixComment()
    //TODO: std::exception
}

Comment ElementArray::getComment(const size_t index) {
    //TODO: ElementArray::getComment()
    //TODO: std::exception

    return {};
}

std::string& ElementArray::getPrefixComment(const size_t index) {
    //TODO: ElementArray::getPrefixComment()
    //TODO: std::exception

    return *new std::string();
}

std::string& ElementArray::getSuffixComment(const size_t index) {
    //TODO: ElementArray::getSuffixComment()
    //TODO: std::exception

    return *new std::string();
}

void ElementArray::clearComment(const size_t index) noexcept {
    //TODO: ElementArray::clearComment()
}

void ElementArray::clearPrefixComment(const size_t index) noexcept {
    //TODO: ElementArray::clearPrefixComment()
}

void ElementArray::clearSuffixComment(const size_t index) noexcept {
    //TODO: ElementArray::clearSuffixComment()
}

void ElementArray::deleteComment(const size_t index) noexcept {
    //TODO: ElementArray::deleteComment()
}

void ElementArray::deletePrefixComment(const size_t index) noexcept {
    //TODO: ElementArray::clearPrefixComment()
}

void ElementArray::deleteSuffixComment(const size_t index) noexcept {
    //TODO: ElementArray::deleteSuffixComment()
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
    //TODO: ElementArray::getTypeFront()
    //TODO: std::exception

    return {};
}

ValueType ElementArray::getTypeAt(const size_t index) {
    //TODO: ElementArray::getTypeAt()
    //TODO: std::exception

    return {};
}

ValueType ElementArray::getTypeBack() {
    //TODO: ElementArray::getTypeBack()
    //TODO: std::exception

    return {};
}

ElementArray& ElementArray::pushFront(const Element &element) noexcept {
    //TODO: ElementArray::pushFront()
    return *this;
}

ElementArray& ElementArray::pushAt(const Element &element, const size_t index) noexcept {
    //TODO: ElementArray::pushAt()
    return *this;
}

ElementArray& ElementArray::pushBack(const Element &element) noexcept {
    //TODO: ElementArray::pushBack()
    return *this;
}

Element& ElementArray::getValue(const size_t index) {
    //TODO: ElementArray::getValue()
    return *this;
}

Element& ElementArray::getValue(const VString& complex_key) {
    //TODO: ElementArray::getValue()
    return *this;
}

Element& ElementArray::getValue(const std::vector<size_t>& complex_key) {
    //TODO: ElementArray::getValue()
    return *this;
}

ElementArray &ElementArray::popFront() {
    //TODO: ElementArray::popFront()
    return *this;
}

ElementArray &ElementArray::popAt(const size_t index) {
    //TODO: ElementArray::popAt()
    return *this;
}

ElementArray &ElementArray::popBack() {
    //TODO: ElementArray::popBack()
    return *this;
}

ElementArray &ElementArray::erase(const size_t index) {
    //TODO: ElementArray::erase()
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
    //TODO: ElementArray::erase()
    return *this;
}

void ElementArray::operator=(const std::vector<Element> &other) noexcept {
    m_values = other;
}


bool IsElementArray(const std::string &str) noexcept {
    //TODO: IsElementArray()
    return false;
}


