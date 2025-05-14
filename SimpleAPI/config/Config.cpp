

IElement &IElement::get_front() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eJson:  return IElement(dynamic_cast<ElementJson*>(this)->get_front());
    case ValueType::eArray: return IElement(dynamic_cast<ElementArray*>(this)->get_front());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return IElement();
}

IElement &IElement::get_at(const size_t index) {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eJson:  return IElement(dynamic_cast<ElementJson*>(this)->get_at(index));
    case ValueType::eArray: return IElement(dynamic_cast<ElementArray*>(this)->get_at(index));
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return IElement();
}

IElement &IElement::get_back() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eJson:  return IElement(dynamic_cast<ElementJson*>(this)->get_back());
    case ValueType::eArray: return IElement(dynamic_cast<ElementArray*>(this)->get_back());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return IElement();
}

IElement &&IElement::setValue() noexcept {
    m_element = std::unique_ptr<IElement>(new ElementNull());
    return *this;
}

IElement &&IElement::setValue(const IElement &&value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    case ValueType::eNull:      init();
    }
    return *this;
}

IElement &&IElement::setValue(const IElement& value) noexcept {
    switch(value.getType()) {
    case ValueType::eNumber:    setValue(value.getNumber());
    case ValueType::eBool:      setValue(value.getBool());
    case ValueType::eString:    setValue(value.getString());
    case ValueType::eArray:     setValue(value.getArray());
    case ValueType::eJson:      setValue(value.getJson());
    case ValueType::eNull:      init();
    }
    return *this;
}

IElement &&IElement::setValue(const bool value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementBool(value));
    return *this;
}

IElement &&IElement::setValue(const long double& value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementNumber(value));
    return *this;
}

IElement &&IElement::setValue(const std::string& value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementString(value));
    return *this;
}

IElement &&IElement::setValue(const ElementArray &value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementArray(value));
    return *this;
}

IElement &&IElement::setValue(const ElementJson &value) noexcept {
    m_element = std::unique_ptr<IElement>(new ElementJson(value));
    return *this;
}

std::string IElement::toString(const ConfigFormat format, const CommentDesign &design,
                             const int8_t tabulation_level) const noexcept
{
    return m_element->toString(format, design, tabulation_level);
}

std::string IElement::toString(const ConfigFormat format, const int8_t tabulation_level) const noexcept {
    return m_element->toString(format, tabulation_level);
}

IElement &&IElement::readFile(const std::string &file_path, const ConfigFormat format,
                         const bool with_comments, std::string *error_log)
{
    *this = ReadFile(file_path, format, with_comments, error_log);
    return *this;
}

IElement &&IElement::readFileJson(const std::string &file_path, const bool with_comments,
                             std::string *error_log)
{
    *this = ReadFileJson(file_path, with_comments, error_log);
    return *this;
}

IElement &&IElement::readFileIni(const std::string &file_path, const bool with_comments,
                            std::string *error_log)
{
    *this = ReadFileIni(file_path, with_comments, error_log);
    return *this;
}

bool IElement::writeFile(const std::string &file_path, const ConfigFormat format,
                       const bool with_comments) noexcept
{
    return WriteFile(*this, file_path, format, with_comments);
}

bool IElement::writeFileJson(const std::string &file_path, const bool with_comments) noexcept {
    return WriteFileJson(*this, file_path, with_comments);
}

bool IElement::writeFileIni(const std::string &file_path, const bool with_comments) noexcept {
    return WriteFileIni(*this, file_path, with_comments);
}

IElement &&IElement::parse(const std::string &content, const ConfigFormat format,
                      const bool with_comments, std::string *error_log)
{
    *this = Parse(content, format, with_comments, error_log);
    return *this;
}

IElement &&IElement::parseJson(const std::string &content, const bool with_comments,
                          std::string *error_log)
{
    *this = ParseJson(content, with_comments, error_log);
    return *this;
}

IElement &&IElement::parseIni(const std::string &content, const bool with_comments,
                         std::string *error_log)
{
    *this = ParseIni(content, with_comments, error_log);
    return *this;
}

void *IElement::begin() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_begin());
    case ValueType::eJson:  return (void*) &*(json_begin());
    //NOTE: вручную учитывать контейнерные типы
 //TODO:    case ValueType::eYaml:
 //TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *IElement::end() {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_end());
    case ValueType::eJson:  return (void*) &*(json_end());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *IElement::cbegin() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_cbegin());
    case ValueType::eJson:  return (void*) &*(json_cbegin());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

void *IElement::cend() const {
    __CHECK_TYPE_IS_CONTAINER__((*this))

    switch(getType()) {
    case ValueType::eArray: return (void*) &*(array_cend());
    case ValueType::eJson:  return (void*) &*(json_cend());
    //NOTE: вручную учитывать контейнерные типы
//TODO:    case ValueType::eYaml:
//TODO:    case ValueType::eXml:
    default:                break;
    }

    return nullptr;
}

VElement::iterator IElement::array_begin() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(this)->begin();
}

VElement::iterator IElement::array_end() {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(this)->end();
}

VElement::const_iterator IElement::array_cbegin() const {
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(this)->cbegin();
}

VElement::const_iterator IElement::array_cend() const{
    __CHECK_TYPE_IS_ARRAY__((*this))
    return dynamic_cast<ElementArray*>(this)->cend();
}

VPairElement::iterator IElement::json_begin() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(this)->begin();
}

VPairElement::iterator IElement::json_end() {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(this)->end();
}

VPairElement::const_iterator IElement::json_cbegin() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(this)->cbegin();
}

VPairElement::const_iterator IElement::json_cend() const {
    __CHECK_TYPE_IS_JSON__((*this))
    return dynamic_cast<ElementJson*>(this)->cend();
}



IElement &ReadFile(const std::string &file_path, const ConfigFormat format,
                const bool with_comments, std::string *error_log)
{
    ...
}

IElement &ReadFileJson(const std::string &file_path, const bool with_comments,
                    std::string *error_log)
{
    ...
}

IElement &ReadFileIni(const std::string &file_path, const bool with_comments,
                   std::string *error_log)
{
    ...
}

bool WriteFile(const IElement &&config, const std::string &file_path,
               const ConfigFormat format, const bool with_comments) noexcept
{
    ...
}

bool WriteFileJson(const IElement &&config, const std::string &file_path,
                   const bool with_comments) noexcept
{
    ...
}

bool WriteFileIni(const IElement &&config, const std::string &file_path,
                  const bool with_comments) noexcept
{
    ...
}

IElement &Parse(const std::string &content, const ConfigFormat format,
             const bool with_comments, std::string *error_log)
{
    ...
}

IElement &ParseJson(const std::string &content, const bool with_comments,
                 std::string *error_log)
{
    ...
}

IElement &ParseIni(const std::string &content, const bool with_comments,
                std::string *error_log)
{
    ...
}
