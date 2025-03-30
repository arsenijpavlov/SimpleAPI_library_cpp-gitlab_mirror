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
    ElementArray() noexcept : m_values({})                  { m_type = ValueType::eArray; }
    template<typename ... Types>
    ElementArray(const Types... args) noexcept              { for(const Element el : {Element(args)...}) pushBack(el); }
    //TODO: ElementArray(const std::string& string) noexcept;
    ~ElementArray() noexcept;

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
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        addComment(const size_t index, const Comment& content) const noexcept;
    void        addComment(const size_t index, const std::string& contentBefore,
                    const std::string& contentAfter) const noexcept;
    Comment     getComment(const size_t index) const noexcept;
    void        clearComment(const size_t index) const noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    size_t          size() noexcept                             { return m_values.size(); }
    bool            isEmpty() noexcept                          { return m_values.empty(); }
    bool            isEqual(const Element& other, const bool compare_comments = false) const noexcept override;
    ElementArray&   clear() noexcept;

    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        pushBack(const Element& element) noexcept;
    void        append(const Element& element) noexcept     { pushBack(element); }
    void        pushFront(const Element& element) noexcept;
    Element     popBack();
    Element     popFront();
    Element     popAt(const size_t index);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS

    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    std::vector<Element>&   getValue(const size_t index) noexcept;
    std::vector<Element>&   getValue(const std::vector<std::string>& complex_key) noexcept;
    Element&    getBack();
    Element&    getFront();
    Element&    getAt(const size_t index);

    ValueType   getType(const size_t index);
    ValueType   getTypeFront();
    ValueType   getTypeBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        operator=(const std::vector<Element>& other) noexcept;
    Element&    operator[](const size_t index) noexcept;
    void        operator<<(const Element& element) noexcept; //альтернатива push_back()
    Element&    operator[](const std::vector<std::string>& complex_key);
                template<std::size_t SIZE>
    Element&    operator[](const std::array<std::string, SIZE>& complex_key) {
        std::vector<std::string> complex_key_vec;
        complex_key_vec.reserve(SIZE);
        std::copy(complex_key.begin(), complex_key.end(), complex_key_vec.begin());
        return (*this)[complex_key_vec];
    }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS

    //если индекс больше количества вложенных элементов, то добавятся в конец
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

    ElementArray&   erase(const size_t index);
    ElementArray&   erase(const AVector::iterator& iterator) {
        m_values.erase(iterator);
        return *this;
    }
    ElementArray&   erase(const AVector::iterator& begin, const AVector::iterator& end) {
        m_values.erase(begin, end);
        return *this;
    }

    AVector::iterator       begin() noexcept                { return m_values.begin(); }
    AVector::iterator       end() noexcept                  { return m_values.end(); }
    AVector::const_iterator cbegin() const noexcept         { return m_values.cbegin(); }
    AVector::const_iterator cend() const noexcept           { return m_values.cend(); }
};

bool IsElementArray(const std::string& str) noexcept;
bool IsElementArray(const Element& e) noexcept;

#endif // JSON_ELEMENT_ARRAY_H
