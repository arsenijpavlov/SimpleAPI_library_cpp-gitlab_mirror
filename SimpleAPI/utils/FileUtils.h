#pragma once

#include <string>
#include <vector>


namespace simpleapi {
namespace utils {

//TODO: получение списка всех файлов в указанной директории с возможностью указать маску поиска
/** @brief GetAllFilesByMask()
 * //TODO: написать документацию
*/
std::vector<std::string> GetAllFilesByMask(const std::string& path_to_dir, const std::string& regex) noexcept;

//TODO: рекурсивный поиск всех путей до файлов по указанной маской начиная с указанной директории до глубины N (-1 - бесконечная глубина поиска)
//TODO: выполнение функции-лямбды для каждого файла из списка (?)

} // namespace utils
} // namespace simpleapi
