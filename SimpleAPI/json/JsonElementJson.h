#ifndef JSON_ELEMENT_JSON_H
#define JSON_ELEMENT_JSON_H

#include "JsonBaseElement.h"
#include "JsonDefines.h"

#include <vector>

using VString = std::vector<std::string>;

//предобъявление
class ElementArray;

//TODO: перенести в JsonDefines.h
using JPair     = std::pair<std::string, Element>;
using JVector   = std::vector<JPair>;

class ElementJson : public Element {
protected:
    JVector m_values;

public:
    ElementJson() noexcept                                  { init(); }
    ElementJson(const ElementJson& json) noexcept {
        init();
        m_values = json.m_values;
    }
    ElementJson(const JPair& pair) noexcept                 { /*TODO*/put(pair.first, pair.second); }
    //разнесено для решения конфликта, не изменять следующие два конструктора!
    ElementJson(const std::string& input_string) noexcept {
        init();
        parseJSON(input_string);
    }
    ElementJson(const std::string& input_string, const ConfigFormat config_format,
                const bool enable_comment = false) noexcept {
        init();
        parse(input_string, config_format, enable_comment);
    }
    __ONLY_ALLOWED_TYPES__(T)
    ElementJson(const std::string& key, const T& value) noexcept {
        init();
        put(key, value);
    }
    ElementJson(const JVector& vec) noexcept {
        init();
        m_values = vec;
    }
    ElementJson(const Element& element) noexcept            { init(); }
    ~ElementJson() noexcept                                 {}

    private:
        void init() noexcept                                { m_type = ValueType::eArray; }
    public:

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        parse(const std::string& string, const ConfigFormat config_format = ConfigFormat::eJSON,
                      const bool enable_comment = false);
    void        parseJSON(const std::string& string, const bool enable_comment = false);
    void        parseINI(const std::string& string, const bool enable_comment = false);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- PARSING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                          const bool to_one_line = false) const noexcept override;
    //TODO: comment_column_size
    //TODO: comment_symbol
//    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
//                          const uint8_t column_size = 0, const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept;
//    std::string to_JSON_string(int16_t tabulation_level = 0, const bool enable_comment = false,
//                               const uint8_t column_size = 0) const noexcept;
//    std::string to_INI_string(int16_t tabulation_level = 0, const bool enable_comment = false,
//                              const uint8_t column_size = 0, const std::string& preview_title = "") const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        addComment(const size_t index, const Comment& content) const noexcept;
    void        addComment(const size_t index, const std::string& contentBefore,
                           const std::string& contentAfter) const noexcept;
    Comment     getComment(const size_t index) const noexcept;
    void        clearComment(const size_t index) const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    ElementJson&    clear() noexcept;
    bool            contains(const std::string& key) const noexcept;
    bool            isEmpty() noexcept                          { return m_values.empty(); }
    bool            isEqual(const Element& other, const bool compare_comments = false) const noexcept override;
    size_t          size() noexcept                             { return m_values.size(); }

    //TYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ValueType   getTypeFront();
    ValueType   getTypeAt(const size_t index);
    ValueType   getTypeAt(const std::string& key);
    ValueType   getTypeBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TYPES

    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: если индекс больше количества вложенных элементов, то добавятся в конец
    //NOTE: если ключа не существовало, его надо создать
    ElementJson&    append(const std::string& key, const Element& value,
                           const bool rewrite = true) noexcept  { return pushBack(key, value); }
    ElementJson&    pushFront(const std::string& key, const Element& value,
                              const bool rewrite = true) noexcept;
    ElementJson&    pushBack(const std::string& key, const Element& value,
                             const bool rewrite = true) noexcept;

    //NOTE: ElementJson::put() всегда положит элемент в конец списка
    ElementJson&    put(const std::string& key, const Element& value,
                        const bool rewrite = true) noexcept;
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    put(const std::string& key, const T& value,
                        const bool rewrite = true) noexcept     { return put(key, Element(value), rewrite); }

    //NOTE: ElementJson::add() всегда положит элемент в конец списка
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    add(const std::string& key, const T& value,
                        const bool rewrite = true) noexcept     { return put(key, value, rewrite); }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    updateValue(const std::string& key, const T& new_value) noexcept {
                        if(contains(key))   (*this)[key] = Element(new_value);
                        else                put(key, new_value);
                        return *this;
                    }
    ElementJson&    updateValue(const std::string& key, const Element& new_value) noexcept;
                    //положить значение в указанную позицию
                    //если значение существует и флаг поднят - удалить существующее значение
                    //если индекс больше количества вложенных элементов, то добавятся в конец
                    //если ключ не найден, добавится в конец
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    insert(const size_t index, const std::string& key,
                                        const T& value, const bool rewrite = true) noexcept
                    {
                        if(contains(key) && rewrite)
                            erase(key);
                        else {
                            if(index > m_values.size() - 1)
                                put(key, value);
                            else
                                m_values.insert(m_values.cbegin() + index, std::make_pair(key, Element(value)));
                        }
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    insert(const JVector::iterator& iterator, const std::string& key,
                                        const T& value, const bool rewrite = true) noexcept
                    {
                        if(contains(key) && rewrite) erase(key);
                        else {
                            if(iterator == m_values.end())  put(key, value);
                            else                            m_values.insert(iterator, std::make_pair(key, Element(value)));
                        }
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    insertBefore(const std::string& keyIndex, const std::string& key,
                                              const T& value, const bool rewrite = true) noexcept
                    {
                        bool key_exists = false; //чтобы второй раз не искать
                        //поиск индекса указанного ключа
                        auto key_found_it = m_values.begin();
                        for(; key_found_it != m_values.end(); key_found_it++) {
                            if(key_found_it->first == keyIndex) {
                                key_exists = true;
                                break;
                            }
                        }

                        if(key_exists && (key_found_it->first == key) && rewrite)
                            erase(key);
                        else {
                            if(key_found_it == m_values.end())
                                put(key, value);
                            else
                                m_values.insert(key_found_it, std::make_pair(key, Element(value)));
                        }
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementJson&    insertAfter(const std::string& keyIndex, const std::string& key,
                                             const T& value, const bool rewrite = true) noexcept
                    {
                        bool key_exists = false; //чтобы второй раз не искать
                        //поиск индекса указанного ключа
                        auto key_found_it = m_values.begin();
                        for(; key_found_it != m_values.end(); key_found_it++) {
                            if(key_found_it->first == keyIndex) {
                                key_exists = true;
                                key_found_it++; //нужен следующий итератор
                                break;
                            }
                        }

                        if(key_exists && (key_found_it->first == key) && rewrite)
                            erase(key);
                        else {
                            if(key_found_it == m_values.end())
                                put(key, value);
                            else
                                m_values.insert(key_found_it, std::make_pair(key, Element(value)));
                        }
                        return *this;
                    }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS

    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Element&    getValue(const size_t index);
    Element&    getValue(const std::string key);
    Element&    getValue(const std::vector<std::string>& complex_key);
    Element&    getFront();
    Element&    getAt(const size_t index);
    Element&    getBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS

    //DELETERS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Element     popFront();
    Element     popAt(const size_t index);
    Element     popAt(const std::string key);
    Element     popBack();
    ElementJson&    erase(const size_t index);
    ElementJson&    erase(const std::string key);
    ElementJson&    erase(const JVector::iterator& iterator) {
                        m_values.erase(iterator);
                        return *this;
                    }
    ElementJson&    erase(const JVector::iterator& begin, const JVector::iterator& end) {
                        m_values.erase(begin, end);
                        return *this;
                    }
    ElementJson&    erase(const std::vector<size_t> indexes);
    ElementJson&    erase(const std::vector<std::string>& keys) noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= DELETERS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        operator=(const JVector& other) noexcept;
    void        operator<<(const JPair& element) noexcept; //альтернатива push_back()
    Element&    operator[](const size_t index)              { return getValue(index); }
    Element&    operator[](const std::string& key)          { return getValue(key); }
    Element&    operator[](const VString& complex_key)      { return getValue(complex_key); }

    //TODO: using AString<SIZE>
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_key) {
                    std::vector<std::string> complex_key_vec;
                    complex_key_vec.reserve(SIZE);
                    std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
                    return (*this)[complex_key_vec];
                }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS

    JVector::iterator       begin() noexcept                    { return m_values.begin(); }
    JVector::iterator       end() noexcept                      { return m_values.end(); }
    JVector::const_iterator cbegin() const noexcept             { return m_values.begin(); }
    JVector::const_iterator cend() const noexcept               { return m_values.end(); }
};

bool IsElementJson(const std::string& str) noexcept;
bool IsElementJson(const Element& e) noexcept;

#endif // JSON_ELEMENT_JSON_H
