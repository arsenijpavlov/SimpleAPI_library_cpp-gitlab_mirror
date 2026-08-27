# 6. Дополнительные функции-утилиты

В ходе разработки библиотеки было сформировано множество полезных функций, которые могут показаться полезными пользователю. Данная статья рассказывает о некоторых из них.

Пространство имён: `simpleapi::utils`.

## 1. CharInString
### Сигнатура:
`bool CharInString(const char ch, std::string symbols) noexcept`

### Суть:
Определяет, есть ли указанный символ ch среди строки symbols.

## 2. ToHexString
### Сигнатура:
`std::string ToHexString(const std::vector<uint8_t>& data) noexcept`

### Суть:
Преобразует вектор байтов в строку формата HEX.

## 3. FromHexString
### Сигнатура:
`std::vector<uint8_t> FromHexString(std::string str) noexcept`

### Суть:
Преобразует строку формата HEX в вектор байтов.

## 4. GetStringCharCount
### Сигнатура:
`size_t GetStringCharCount(const std::string &str, bool only_visible)`

### Суть:
Указывает количество символов в строке формата UTF-8. Опционально можно узнать количество видимых символов.

## 5. GetStringCharCount
### Сигнатура:
`std::string GetNormalizeString(const std::string &input) noexcept`

### Суть:
Соберёт новую строку без пробелов и переносов строк.

## 6. RemoveIllegalSpaces*
### Сигнатуры:
`void RemoveFrontIllegalSpaces(std::string& string) noexcept`
`void RemoveEndIllegalSpaces(std::string& string) noexcept`
`void RemoveIllegalSpaces(std::string& string) noexcept`

### Суть:
Удаляют пробелы и табуляции в начале/конце строки.

## 7. SplitWithoutColumned
### Сигнатура:
`SplittedLines SplitWithoutColumned(const std::string& input_string) noexcept`

### Суть:
Обрежет входную строку на список строк (SplittedLines). Учитываются только пользовательские переносы строк.

## 8. SplitToColumns
### Сигнатура:
`SplittedLines SplitToColumns(const std::string& input_string, const size_t column_size) noexcept`

### Суть:
Обрежет строку на подстроки с заданной шириной.

Если хотя бы одна строка неделима и превышает предел, то остальные строки будут выровнены по новому пределу.

Многоточие считается частью слова и не переносится на другую строку.

Пользовательские переносы строк будут сохранены.

## 9. VStringToString
### Сигнатура:
`std::string VStringToString(const VString& input_vec, const bool need_quotes = false) noexcept`

### Суть:
Преобразует вектор строк в одну строку, при этом изначальные строки будут соединены знаком переноса '\n'.

## 10. SplitString
### Сигнатура:
`SplittedLines SplitString(const std::string input_string, char split_char, bool with_empty_strings = true) noexcept`

### Суть:
Разделяет строку на подстроки с указанным символом-разделителем.

## 11. GetAllFilesByMask
### Сигнатура:
`std::vector<std::string> GetAllFilesByMask(const std::string& path_to_dir, const std::string& regex, const int& max_level = 0) noexcept`

### Суть:
Выдаёт список всех файлов в указанной директории с возможностью указать regex-маску поиска. Выполняет рекурсивный поиск всех путей до файлов по указанной маске, начиная с указанной директории до глубины N (-1 - бесконечная глубина поиска).

