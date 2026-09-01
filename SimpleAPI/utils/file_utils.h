#pragma once

#include <string>
#include <vector>


namespace simpleapi {
namespace utils {

//получение списка всех файлов в указанной директории с возможностью указать regex-маску поиска
//рекурсивный поиск всех путей до файлов по указанной маске, начиная с указанной директории до глубины N (-1 - бесконечная глубина поиска)
/** @brief GetAllFilesByMask()
 * //TODO: написать документацию
 *
*/
std::vector<std::string> GetAllFilesByMask(const std::string& path_to_dir, const std::string& regex,
                                           const int& max_level = 0) noexcept;

} // namespace utils
} // namespace simpleapi
