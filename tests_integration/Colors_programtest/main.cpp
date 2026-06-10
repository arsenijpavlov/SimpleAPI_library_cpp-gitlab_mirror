#include <SimpleAPI.h>
#include <iostream>

using namespace simpleapi;

std::vector<logs::COLOR> ignore_list_bg{
    logs::COLOR::eWHITE_BG,
    logs::COLOR::eBRIGHT_GREEN_BG,
    logs::COLOR::eMAGENTA_BG,
    logs::COLOR::eBRIGHT_MAGENTA_BG,
    logs::COLOR::eCYAN_BG,
    logs::COLOR::eBRIGHT_CYAN_BG,
    logs::COLOR::eRED_BG,
    logs::COLOR::eGREEN_BG,
    logs::COLOR::eBLUE_BG,
    logs::COLOR::eBRIGHT_GRAY_BG,
    logs::COLOR::eBRIGHT_RED_BG,
    logs::COLOR::eBRIGHT_BLUE_BG,
    logs::COLOR::eYELLOW_BG
};
bool not_ignored_bg(uint8_t color) {
    for(logs::COLOR _color : ignore_list_bg)
        if(static_cast<uint8_t>(_color) == color)
            return false;

    return true;
}

std::vector<uint8_t> ignore_list_fg {
};
bool not_ignored_fg(uint8_t color) {
    for(uint8_t _color : ignore_list_fg)
        if(_color == color)
            return false;

    return true;
}

std::vector<std::pair<logs::COLOR, logs::COLOR>> ignore_list_bg_fg {
    {logs::COLOR::eBLACK_BG,            logs::COLOR::eBLACK_FG},
    {logs::COLOR::eBLACK_BG,            logs::COLOR::eBLUE_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eGRAY_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eRED_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eGREEN_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eYELLOW_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eBLUE_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eMAGENTA_FG},
    {logs::COLOR::eGRAY_BG,             logs::COLOR::eBRIGHT_BLUE_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eYELLOW_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eCYAN_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_GRAY_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_RED_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_GREEN_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_YELLOW_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_MAGENTA_FG},
    {logs::COLOR::eBRIGHT_YELLOW_BG,    logs::COLOR::eBRIGHT_CYAN_FG}
};
bool not_ignored_bg_fg(uint8_t color_bg, uint8_t color_fg) {
    using namespace logs;
    std::pair<COLOR, COLOR> pair{static_cast<COLOR>(color_bg), static_cast<COLOR>(color_fg)};
    for(auto _pair : ignore_list_bg_fg)
        if(_pair == pair)
            return false;

    return true;
}

int main(int argc, char** argv) {
    using namespace simpleapi;
    using namespace logs;

    uint8_t minFG = static_cast<uint8_t>(logs::COLOR::eBLACK_FG);
    uint8_t maxFG = static_cast<uint8_t>(logs::COLOR::eBRIGHT_CYAN_FG);
    uint8_t minBG = static_cast<uint8_t>(logs::COLOR::eBLACK_BG);
    uint8_t maxBG = static_cast<uint8_t>(logs::COLOR::eBRIGHT_CYAN_BG);

    for(uint8_t i = minBG; i <= maxBG; i++)
        for(uint8_t j = minFG; j <= maxFG; j++) {
            if(i != j && not_ignored_bg(i) && not_ignored_fg(j) && not_ignored_bg_fg(i, j)) {
                std::vector<Color> colors{(logs::COLOR)i, (logs::COLOR)j};
                std::cout << logs::to_color_string(colors, logs::get_time_string() + "  Simlpe test text")
                          << "\ti:" + std::to_string(i)
                          << "\tj:" + std::to_string(j)
                          << std::endl;
            }
        }

    return 0;
}
