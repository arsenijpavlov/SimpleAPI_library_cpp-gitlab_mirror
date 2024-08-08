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
    Settings settings;
    //пользователь задаёт перевод строки каждые 10 символов комментария
    //(без учёта начала комментария и пробела)
    settings.setColumnWidth(10);

    std::string comment = "eat some more of these soft French rolls and drink some tea";
    settings.setOrUpdate("key_no_groupped", "value", comment);
    settings.getOrDefault("key_no_groupped", "000");


    //работа с группой значений
    SettingsGroup settingsGroup("group_1");
    settingsGroup.put("comment_key", comment);
    settingsGroup.setOrUpdate("key_0", "value");
    settingsGroup.setOrUpdate("key_1", "value", comment);
    settingsGroup.setOrUpdate("key_2", "value", comment,
                         CommentType::eBeforeValueMultiLine);
    settingsGroup.setOrUpdate("key_in_group_3", "value", comment,
                         CommentType::eAfterValueOneLine);

    settings.setOrUpdate("group_name_1", inner_json);
    JArray inner_array;
    settings.setOrUpdate("group_name_2", inner_array);


    //такая запись эквивалентна вложению Json или Array
    settings.beginGroup("my first group");
    {
        settings.setOrUpdate("key_in_group_0", "value");
        settings.setOrUpdate("key_in_group_1", "value", comment);
        settings.setOrUpdate("key_in_group_2", "value", comment,
                             CommentType::eBeforeValueMultiLine);
        settings.setOrUpdate("key_in_group_3", "value", comment,
                             CommentType::eAfterValueOneLine);
    }
    settings.endGroup();

}
