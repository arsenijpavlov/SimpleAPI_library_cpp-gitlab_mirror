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

TEST(JSON_SETTINGS, write_to_file) {
    JsonSettings json_settings;
    json_settings.setOrUpdate("key_no_groupped", "value", "azaza");
    json_settings.getOrDefault("key_no_groupped", "000");
    json_settings.beginGroup("my first group");
    {
        std::string key;
        std::string value;
        std::string comment;
        json_settings.setOrUpdate("key_no_groupped", "value", "azaza");

    }
    json_settings.endGroup();

}
