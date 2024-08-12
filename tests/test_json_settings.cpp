#include <SimpleAPI.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

//========================================================================================

//TEST(JSON_SETTINGS, write_to_file) {
//    Settings settings;
//    //пользователь задаёт перевод строки каждые 10 символов комментария (без учёта начала комментария и пробела)
//    settings.setCommentColumnSize(10);

//    std::string comment = "eat some more of these soft French rolls and drink some tea";

//    settings.setOrUpdate("key_no_groupped", "value", comment);
//    settings.getOrDefault("key_no_groupped", "000");

//    settings.writeToFile("../tests/settings.ini",  ConfigType::eIni);
//    settings.writeToFile("../tests/settings.json", ConfigType::eJson);
//    settings.writeToFile("../tests/settings.yaml", ConfigType::eYaml);
//    settings.writeToFile("../tests/settings.cfg",  ConfigType::eSimpleApiConfig);

//}
