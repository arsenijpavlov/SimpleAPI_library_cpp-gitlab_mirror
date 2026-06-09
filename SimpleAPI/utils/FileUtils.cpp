#include "FileUtils.h"
#include <dirent.h>
#include <regex>


namespace simpleapi {

void RecursiveCheckFiles(std::vector<std::string>& list, const std::string& dir_path,
                         const std::regex& regex, int level) noexcept
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path.c_str());
    if(dir == nullptr) {
        //путь не существует, файлов тем более не найдено
        return;
    }

    //p.s. readdir(dir) поочерёдно даст порабоать с каждым файлом/директорией по текущему пути
    while((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        //пропускаем ссылки на текущую директорию и на предыдущую
        if(name == "." || name == "..")
            continue;

        if(entry->d_type == DT_DIR) {
            if(level == -1 || level > 0) {
                std::string new_path = dir_path + "/" + entry->d_name;
                int new_level = (level == -1) ? level : level - 1;
                RecursiveCheckFiles(list, new_path, regex, new_level);
            }
        } else {
            //проверка маски по regex
            if(std::regex_match(name, regex))
                list.push_back(dir_path + "/" + name);
        }
    }
}

std::vector<std::string> utils::GetAllFilesByMask(const std::string &path_to_dir, const std::string &regex,
                                                  const int& max_level) noexcept
{
    std::vector<std::string> ret;
    std::regex reg(regex);

    //проверка валидности пути (если передан путь до файла, то взять директорию файла за основу поиска)
    //для каждого файла в папке проверить маску (одно вхождение в рамках этой функции)
    RecursiveCheckFiles(ret, path_to_dir, reg, max_level);

    return ret;
}

} // namespace simpleapi
