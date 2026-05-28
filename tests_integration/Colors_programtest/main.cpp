#include <SimpleAPI.h>
#include <iostream>

using namespace simpleapi;

std::vector<uint8_t> ignore_list_bg {
    logs::eWHITE_BG,
    logs::eBRIGHT_GREEN_BG,
    logs::eMAGENTA_BG,
    logs::eBRIGHT_MAGENTA_BG,
    logs::eCYAN_BG,
    logs::eBRIGHT_CYAN_BG,
    logs::eRED_BG,
    logs::eGREEN_BG,
    logs::eBLUE_BG,
    logs::eBRIGHT_GRAY_BG,
    logs::eBRIGHT_RED_BG,
    logs::eBRIGHT_BLUE_BG,
    logs::eYELLOW_BG
};
bool not_ignored_bg(uint8_t color) {
    for(uint8_t _color : ignore_list_bg)
        if(_color == color)
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

std::vector<std::pair<uint8_t, uint8_t>> ignore_list_bg_fg {
    {logs::eBLACK_BG,            logs::eBLACK_FG},
    {logs::eBLACK_BG,            logs::eBLUE_FG},
    {logs::eGRAY_BG,             logs::eGRAY_FG},
    {logs::eGRAY_BG,             logs::eRED_FG},
    {logs::eGRAY_BG,             logs::eGREEN_FG},
    {logs::eGRAY_BG,             logs::eYELLOW_FG},
    {logs::eGRAY_BG,             logs::eBLUE_FG},
    {logs::eGRAY_BG,             logs::eMAGENTA_FG},
    {logs::eGRAY_BG,             logs::eBRIGHT_BLUE_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eYELLOW_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eCYAN_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_GRAY_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_RED_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_GREEN_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_YELLOW_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_MAGENTA_FG},
    {logs::eBRIGHT_YELLOW_BG,    logs::eBRIGHT_CYAN_FG}
};
bool not_ignored_bg_fg(uint8_t color_bg, uint8_t color_fg) {
    std::pair<uint8_t, uint8_t> pair{color_bg, color_fg};
    for(auto _pair : ignore_list_bg_fg)
        if(_pair == pair)
            return false;

    return true;
}

int main(int argc, char** argv) {
    uint8_t minFG = logs::COLOR::eBLACK_FG;
    uint8_t maxFG = logs::COLOR::eBRIGHT_CYAN_FG;
    uint8_t minBG = logs::COLOR::eBLACK_BG;
    uint8_t maxBG = logs::COLOR::eBRIGHT_CYAN_BG;

    for(uint8_t i = minBG; i <= maxBG; i++)
        for(uint8_t j = minFG; j <= maxFG; j++) {
            if(i != j && not_ignored_bg(i) && not_ignored_fg(j) && not_ignored_bg_fg(i, j)) {
                std::vector<logs::COLOR> color{(logs::COLOR)i, (logs::COLOR)j};
                std::cout << logs::to_color_string(color, logs::get_time_string() + "  Simlpe test text")
                          << "\ti:" + std::to_string(i)
                          << "\tj:" + std::to_string(j)
                          << std::endl;
            }
        }

    return 0;
}
