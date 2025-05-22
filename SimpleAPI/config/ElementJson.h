#ifndef ELEMENT_JSON_H
#define ELEMENT_JSON_H

#include "IElement.h"
#include "ConfigDefines.h"

#include <vector>


//предобъявление
//class ElementArray;
//
class ElementJson : public IElement {
//protected:
//    VPairElement m_values;
//
//public:
//    ElementJson() noexcept                                  { init(); }
//    ElementJson(const ElementJson& json) noexcept {
//        init();
//        m_values = json.m_values;
//    }
//    ElementJson(const JPair& pair) noexcept {
//        init();
//        put(pair.first, pair.second);
//    }
//    //разнесено для решения конфликта, не изменять следующие два конструктора!
//    ElementJson(const std::string& input_string) noexcept {
//        init();
//        parseJson(input_string);
//    }
//    ElementJson(const std::string& input_string, const ConfigFormat config_format,
//                const bool enable_comment = false) noexcept {
//        init();
//        parse(input_string, config_format, enable_comment);
//    }
//    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson(const std::string& key, const T& value) noexcept {
//        init();
//        put(key, value);
//    }
//    ElementJson(const VPairElement& vec) noexcept {
//        init();
//        m_values = vec;
//    }
//    ElementJson(const IElement& element) noexcept           { init(); }
//    ~ElementJson() noexcept                                 {}
//
//    private:
//        void init() noexcept                                { m_type = ValueType::eArray; }
//    public:
//
//    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    void        parse(const std::string& string, const ConfigFormat config_format = ConfigFormat::eJSON,
//                      const bool enable_comment = false);
//    void        parseJson(const std::string& string, const bool enable_comment = false);
//    void        parseINI(const std::string& string, const bool enable_comment = false);
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- PARSING
//
//    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
////TODO:    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON) const noexcept override;
////TODO:    std::string to_string(const ConfigFormat format, const CommentDesign &design) const noexcept override;
////    std::string to_string(int16_t tabulation_level = 0, const bool enable_comment = false,
////                          const uint8_t column_size = 0, const ConfigFormat config_format = ConfigFormat::eJSON) const noexcept;
////    std::string toJsonString(int16_t tabulation_level = 0, const bool enable_comment = false,
////                               const uint8_t column_size = 0) const noexcept;
////    std::string toIniString(int16_t tabulation_level = 0, const bool enable_comment = false,
////                              const uint8_t column_size = 0, const std::string& preview_title = "") const noexcept;
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING
//
//    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    void        add_comment(const size_t index, const Comment& content) const noexcept;
//    void        add_comment(const size_t index, const std::string& contentBefore,
//                           const std::string& contentAfter) const noexcept;
//    Comment     get_comment(const size_t index) const noexcept;
//    void        clear_comment(const size_t index) const noexcept;
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS
//
//    ElementJson&    clear() noexcept;
//    bool            contains(const std::string& key) const noexcept;
//    bool            isEmpty() noexcept                      { return m_values.empty(); }
//    bool            isEqual(const IElement& other, const bool compare_comments = false) const noexcept override;
//    size_t          size() noexcept                         { return m_values.size(); }
//
//    //TYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    ValueType   get_type_front();
//    ValueType   get_type_at(const size_t index);
//    ValueType   get_type_at(const std::string& key);
//    ValueType   get_type_back();
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TYPES
//
//    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    //NOTE: если индекс больше количества вложенных элементов, то добавятся в конец
//    //NOTE: если ключа не существовало, его надо создать
//    ElementJson&    append(const std::string& key, const IElement& value,
//                           const bool rewrite = true) noexcept  { return push_back(key, value); }
//    ElementJson&    push_front(const std::string& key, const IElement& value,
//                              const bool rewrite = true) noexcept;
//    ElementJson&    push_back(const std::string& key, const IElement& value,
//                             const bool rewrite = true) noexcept;
//
//    //NOTE: ElementJson::put() всегда положит элемент в конец списка
//    ElementJson&    put(const std::string& key, const IElement& value,
//                        const bool rewrite = true) noexcept;
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    put(const std::string& key, const T& value,
//                        const bool rewrite = true) noexcept { return put(key, IElement(value), rewrite); }
//
//    //NOTE: ElementJson::add() всегда положит элемент в конец списка
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    add(const std::string& key, const T& value,
//                        const bool rewrite = true) noexcept { return put(key, value, rewrite); }
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    update_value(const std::string& key, const T& new_value) noexcept {
//                        if(contains(key))   (*this)[key] = Element(new_value);
//                        else                put(key, new_value);
//                        return *this;
//                    }
//    ElementJson&    update_value(const std::string& key, const IElement& new_value) noexcept;
//                    //положить значение в указанную позицию
//                    //если значение существует и флаг поднят - удалить существующее значение
//                    //если индекс больше количества вложенных элементов, то добавятся в конец
//                    //если ключ не найден, добавится в конец
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    insert(const size_t index, const std::string& key,
//                                        const T& value, const bool rewrite = true) noexcept
//                    {
//                        if(contains(key) && rewrite)
//                            erase(key);
//                        else {
//                            if(index > m_values.size() - 1)
//                                put(key, value);
//                            else
//                                m_values.insert(m_values.cbegin() + index, std::make_pair(key, IElement(value)));
//                        }
//                        return *this;
//                    }
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    insert(const VPairElement::iterator& iterator, const std::string& key,
//                                        const T& value, const bool rewrite = true) noexcept
//                    {
//                        if(contains(key) && rewrite) erase(key);
//                        else {
//                            if(iterator == m_values.end())  put(key, value);
//                            else                            m_values.insert(iterator, std::make_pair(key, IElement(value)));
//                        }
//                        return *this;
//                    }
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    insert_before(const std::string& keyIndex, const std::string& key,
//                                              const T& value, const bool rewrite = true) noexcept
//                    {
//                        bool key_exists = false; //чтобы второй раз не искать
//                        //поиск индекса указанного ключа
//                        auto key_found_it = m_values.begin();
//                        for(; key_found_it != m_values.end(); key_found_it++) {
//                            if(key_found_it->first == keyIndex) {
//                                key_exists = true;
//                                break;
//                            }
//                        }
//
//                        if(key_exists && (key_found_it->first == key) && rewrite)
//                            erase(key);
//                        else {
//                            if(key_found_it == m_values.end())
//                                put(key, value);
//                            else
//                                m_values.insert(key_found_it, std::make_pair(key, IElement(value)));
//                        }
//                        return *this;
//                    }
//                    __ONLY_ALLOWED_TYPES__(T)
//    ElementJson&    insert_after(const std::string& keyIndex, const std::string& key,
//                                             const T& value, const bool rewrite = true) noexcept
//                    {
//                        bool key_exists = false; //чтобы второй раз не искать
//                        //поиск индекса указанного ключа
//                        auto key_found_it = m_values.begin();
//                        for(; key_found_it != m_values.end(); key_found_it++) {
//                            if(key_found_it->first == keyIndex) {
//                                key_exists = true;
//                                key_found_it++; //нужен следующий итератор
//                                break;
//                            }
//                        }
//
//                        if(key_exists && (key_found_it->first == key) && rewrite)
//                            erase(key);
//                        else {
//                            if(key_found_it == m_values.end())
//                                put(key, value);
//                            else
//                                m_values.insert(key_found_it, std::make_pair(key, IElement(value)));
//                        }
//                        return *this;
//                    }
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS
//
//    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    IElement&       get_value(const size_t index);
//    IElement&       get_value(const std::string key);
//    IElement&       get_value(const VString& complex_key);
//    IElement&       get_front();
//    IElement&       get_at(const size_t index);
//    IElement&       get_back();
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS
//
//    //DELETERS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    IElement        pop_front();
//    IElement        pop_at(const size_t index);
//    IElement        pop_at(const std::string key);
//    IElement        pop_back();
//    ElementJson&    erase(const size_t index);
//    ElementJson&    erase(const std::string key);
//    ElementJson&    erase(const VPairElement::iterator& iterator) {
//                        m_values.erase(iterator);
//                        return *this;
//                    }
//    ElementJson&    erase(const VPairElement::iterator& begin, const VPairElement::iterator& end) {
//                        m_values.erase(begin, end);
//                        return *this;
//                    }
//    ElementJson&    erase(const std::vector<size_t> indexes);
//    ElementJson&    erase(const VString& keys) noexcept;
//    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= DELETERS
//
//    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    void        operator=(const VPairElement& other) noexcept;
//    void        operator<<(const JPair& element) noexcept; //альтернатива push_back()
//    IElement&   operator[](const size_t index)             { return get_value(index); }
//    IElement&   operator[](const std::string& key)         { return get_value(key); }
//    IElement&   operator[](const VString& complex_key)     { return get_value(complex_key); }
//
//                template<std::size_t SIZE>
//    IElement&   operator[](const std::array<std::string, SIZE>& complex_key) {
//                    VString complex_key_vec;
//                    complex_key_vec.reserve(SIZE);
//                    std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
//                    return (*this)[complex_key_vec];
//                }
//    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS
//
//    VPairElement::iterator       begin() noexcept           { return m_values.begin(); }
//    VPairElement::iterator       end() noexcept             { return m_values.end(); }
//    VPairElement::const_iterator cbegin() const noexcept    { return m_values.begin(); }
//    VPairElement::const_iterator cend() const noexcept      { return m_values.end(); }
};

bool IsElementJson(const std::string& str)  noexcept;
bool IsElementJson(const IElement& e)       noexcept;
bool IsElementJson(const Config& cfg)       noexcept;

#endif // ELEMENT_JSON_H
