#ifndef JSON_ELEMENT_ARRAY_H
#define JSON_ELEMENT_ARRAY_H

#include "JsonBaseElement.h"
#include "JsonDefines.h"


//предобъявление
class ElementJson;

class ElementArray : public Element {
protected:
    VElement m_values;

public:
    ElementArray() noexcept                                     { init(); }
    template<typename ... Types>
    ElementArray(const Types... args) noexcept {
        init();
        for(const Element el : {Element(args)...})
            pushBack(el);
    }
    ElementArray(const std::string& string, const ConfigFormat format,
                 const bool enable_comments = false) noexcept;
    ~ElementArray() noexcept                                    {}

private:
    void init() noexcept                                        { m_type = ValueType::eArray; }
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
    //TODO: to_JSON_string()
    //TODO: to_INI_string()
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= PRINTING

    //COMMENTS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void        addComment(const size_t index, const Comment& content);
    void        addComment(const size_t index, const std::string& content_before,
                           const std::string& content_after);
    void        addPrefixComment(const size_t index, const std::string& content);
    void        addSuffixComment(const size_t index, const std::string& content);

    Comment&        getComment(const size_t index);
    Comment         getComment(const size_t index) const;
    std::string&    getPrefixComment(const size_t index);
    std::string     getPrefixComment(const size_t index) const;
    std::string&    getSuffixComment(const size_t index);
    std::string     getSuffixComment(const size_t index) const;

    //NOTE: noexcept, потому что неправильный индекс просто пропустит действие
    void        clearComment(const size_t index) noexcept;
    void        clearPrefixComment(const size_t index) noexcept;
    void        clearSuffixComment(const size_t index) noexcept;
    void        deleteComment(const size_t index) noexcept;
    void        deletePrefixComment(const size_t index) noexcept;
    void        deleteSuffixComment(const size_t index) noexcept;
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= COMMENTS

    ElementArray&   clear() noexcept;
    bool            contains(const size_t& index) const noexcept
                                                                { return m_values.size() > index; }
    bool            isEmpty() const noexcept                    { return m_values.empty(); }
    bool            isEqual(const Element& other, const bool compare_comments = false) const noexcept override;
    size_t          size() const noexcept                       { return m_values.size(); }

    //TYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ValueType   getTypeFront();
    ValueType   getTypeAt(const size_t index);
    ValueType   getTypeBack();
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TYPES

    //SETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //NOTE: если индекс больше количества вложенных элементов, то добавятся в конец
    ElementArray&   append(const Element& element) noexcept     { return pushBack(element); }
    ElementArray&   pushFront(const Element& element) noexcept;
    ElementArray&   pushAt(const Element& element, const size_t index);
    ElementArray&   pushBack(const Element& element) noexcept;
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const size_t index, const T& value) noexcept {
                        if(index > m_values.size() - 1) m_values.push_back(value);
                        else                            m_values.insert(m_values.cbegin() + index, Element(value));
                        return *this;
                    }
                    __ONLY_ALLOWED_TYPES__(T)
    ElementArray&   insert(const VElement::iterator& iterator, const T& value) {
                        m_values.insert(iterator, value);
                        return *this;
                    }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- SETTERS

    //GETTERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    Element&    getValue(const size_t index);
    Element&    getValue(const VString& complex_key);
    Element&    getValue(const std::vector<size_t>& complex_key);
    Element&    getFront()                                      { return m_values.front(); }
    Element&    getAt(const size_t index)                       { return getValue(index); }
    Element&    getBack()                                       { return m_values.back(); }
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- GETTERS

    //DELETERS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    ElementArray&   popFront();
    ElementArray&   popAt(const size_t index);
    ElementArray&   popBack();
    ElementArray&   erase(const size_t index);
    ElementArray&   erase(const VElement::iterator& iterator);
    ElementArray&   erase(const VElement::iterator& begin, const VElement::iterator& end);
    ElementArray&   erase(const std::vector<size_t> indexes);
    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= DELETERS

    //OPERATORS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    void            operator=(const VElement& other) noexcept;
    ElementArray&   operator<<(const Element& element) noexcept { return pushBack(element); }
    Element&        operator[](const size_t index)              { return getValue(index); }
    Element&        operator[](const VString& complex_key)      { return getValue(complex_key); }
                    template<std::size_t SIZE>
    Element&        operator[](const std::array<std::string, SIZE>& complex_key) {
                        VString complex_key_vec;
                        complex_key_vec.reserve(SIZE);
                        std::copy(complex_key.begin() + 1, complex_key.end(), complex_key_vec.begin());
                        return (*this)[complex_key_vec];
                    }
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= OPERATORS

    VElement::iterator       begin() noexcept                   { return m_values.begin(); }
    VElement::iterator       end() noexcept                     { return m_values.end(); }
    VElement::const_iterator cbegin() const noexcept            { return m_values.cbegin(); }
    VElement::const_iterator cend() const noexcept              { return m_values.cend(); }
};

bool IsElementArray(const std::string& str, const ConfigFormat format = ConfigFormat::eJSON) noexcept;
bool IsElementJsonArray(const std::string& str) noexcept;
bool IsElementIniArray(const std::string& str) noexcept;
inline bool IsElementArray(const Element& e) noexcept           { return e.getType() == ValueType::eArray; }

#endif // JSON_ELEMENT_ARRAY_H
