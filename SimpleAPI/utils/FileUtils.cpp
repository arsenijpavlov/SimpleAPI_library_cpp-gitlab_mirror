#include "FileUtils.h"
#include <dirent.h>
#include <regex>


namespace simpleapi {

std::vector<std::string> utils::GetAllFilesByMask(const std::string &path_to_dir, const std::string &regex,
                                                  const int& max_level) noexcept
{
    std::vector<std::string> ret;

    //проверка валидности пути (если передан путь до файла, то взять директорию файла за основу поиска)

    struct dirent *entry;
    DIR *dir = opendir(path_to_dir.c_str());
    if(dir == nullptr) {
        //путь не существует, файлов тем более не найдено
        return {};
    }

    std::regex reg(regex);

    //для каждого файла в папке проверить маску (одно вхождение в рамках этой функции)
    //p.s. readdir(dir) поочерёдно даст порабоать с каждым файлом/директорией по текущему пути
    while((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        //пропускаем ссылки на текущую директорию и на предыдущую
        if(name == "." || name == "..")
            continue;

        //TODO: нужны рекурсивные вызовы

        //проверка маски по regex
        if(std::regex_match(name, reg) && entry->d_type != DT_DIR)
            ret.push_back(path_to_dir + "/" + name);
    }

    return ret;
}

} // namespace simpleapi
