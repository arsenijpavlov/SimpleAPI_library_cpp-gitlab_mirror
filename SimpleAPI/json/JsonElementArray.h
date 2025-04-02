#ifndef JSON_ELEMENT_ARRAY_H
#define JSON_ELEMENT_ARRAY_H

#include "JsonBaseElement.h"
#include "JsonDefines.h"

#include <vector>


//предобъявление
class ElementJson;

using AVector = std::vector<Element>;
class ElementArray : public Element {
protected:
    std::vector<Element> m_values;

public:
    ElementArray() noexcept : m_values({})                  { init(); }
    template<typename ... Types>
    ElementArray(const Types... args) noexcept {
        init();
        for(const Element el : {Element(args)...})
            pushBack(el);
    }
    ElementArray(const std::string& string, const ConfigFormat format,
                 const bool enable_comments = false) noexcept {
        init();
        parseArray(string, enable_comments, format);
    }
    ~ElementArray() noexcept                                {}

private:
    void init() noexcept                                    { m_type = ValueType::eArray; }
public:

    //PARSING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        parseArray(const std::string& string, const bool enable_comment = false,
                           const ConfigFormat config_format = ConfigFormat::eJSON);
    void        parseJSON_array(const std::string& string, const bool enable_comment = false);
    void        parseINI_array(const std::string& string, const bool enable_comment = false);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- PARSING

    //PRINTING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //to_one_line - следует ли попытаться вывести всё в одну строку (комментарии будут проигнорированы, \
                    а многострочные значения так же станут занимать несколько строк)
    std::string to_string(const ConfigFormat format = ConfigFormat::eJSON,
                          const bool to_one_line = false) const noexcept override;
    //TODO: comment_column_size
    //TODO: comment_symbol
    //TODO: to_JSON_string()
    //TODO: to_INI_string()
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        addComment(const size_t index, const Comment& content) const noexcept;
    void        addComment(const size_t index, const std::string& contentBefore,
                           const std::string& contentAfter) const noexcept;
    Comment     getComment(const size_t index) const noexcept;
    void        clearComment(const size_t index) const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    ElementArray&   clear() noexcept;
    bool            contains(const std::string& key) const noexcept;
    bool            isEmpty() const noexcept                { return m_values.empty(); }
    bool            isEqual(const Element& other, const bool compare_comments = false) const noexcept override;
    size_t          size() const noexcept                   { return m_values.size(); }

    //TYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ValueType   getTypeFront();
    ValueType   getTypeAt(const size_t index);
    ValueType   getTypeBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TYPES

    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: если индекс больше количества вложенных элементов, то добавятся в конец
    void        append(const Element& element) noexcept     { pushBack(element); }
    void        pushFront(const Element& element) noexcept;
    void        pushAt(const Element& element, const size_t index) noexcept;
    void        pushBack(const Element& element) noexcept;
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const size_t index, const T& value) noexcept {
                        if(index > m_values.size() - 1) m_values.push_back(value);
                        else                            m_values.insert(m_values.cbegin() + index, Element(value));
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const AVector::iterator& iterator, const T& value) {
                        m_values.insert(iterator, value);
                        return *this;
                    }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS

    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ElementArray&   getValue(const size_t index);
    ElementArray&   getValue(const std::vector<std::string>& complex_key);
    ElementArray&   getValue(const std::vector<size_t>& complex_key);
    ElementArray&   getFront();
    ElementArray&   getAt(const size_t index);
    ElementArray&   getBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS

    //DELETERS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Element     popFront();
    Element     popAt(const size_t index);
    Element     popBack();
    ElementArray&   erase(const size_t index);
    ElementArray&   erase(const AVector::iterator& iterator) {
                        m_values.erase(iterator);
                        return *this;
                    }
    ElementArray&   erase(const AVector::iterator& begin, const AVector::iterator& end) {
                        m_values.erase(begin, end);
                        return *this;
                    }
    ElementArray&   erase(const std::vector<size_t> indexes);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= DELETERS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        operator=(const std::vector<Element>& other) noexcept;
    void        operator<<(const Element& element) noexcept; //альтернатива push_back()
    Element&    operator[](const size_t index);
    Element&    operator[](const std::vector<std::string>& complex_key);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_key) {
                    std::vector<std::string> complex_key_vec;
                    complex_key_vec.reserve(SIZE);
                    std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
                    return (*this)[complex_key_vec];
                }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS

    AVector::iterator       begin() noexcept                { return m_values.begin(); }
    AVector::iterator       end() noexcept                  { return m_values.end(); }
    AVector::const_iterator cbegin() const noexcept         { return m_values.cbegin(); }
    AVector::const_iterator cend() const noexcept           { return m_values.cend(); }
};

bool IsElementArray(const std::string& str) noexcept;
bool IsElementArray(const Element& e) noexcept;

#endif // JSON_ELEMENT_ARRAY_H
