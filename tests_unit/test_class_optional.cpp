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

// инициализация
TEST(OPTIONAL, constructors) {
    using namespace simpleapi;

    Optional<int> oi_1;
    EXPECT_FALSE(oi_1);
    EXPECT_FALSE(oi_1.isValid()); // альтернативный вызов

    Optional<int> oi_2(100);
    EXPECT_TRUE(oi_2);
    EXPECT_EQ(oi_2.value(), 100);

    oi_2 = 101;
    EXPECT_TRUE(oi_2);
    EXPECT_EQ(oi_2.value(), 101);

    oi_2.set(102);
    EXPECT_EQ(oi_2.value(), 102);
}

// "выключение" и "включение" переменной
TEST(OPTIONAL, enable_disable) {
    using namespace simpleapi;

    Optional<int> oi;
    EXPECT_FALSE(oi);

    oi = 15;
    EXPECT_TRUE(oi);

    oi.unset();
    EXPECT_FALSE(oi);
}

// переменная как условие выполнения ветки кода
TEST(OPTIONAL, if_then) {
    using namespace simpleapi;
    int res;

    Optional<int> oi_1(100);
    if(oi_1)
        res = 200;
    else
        res = 300;
    EXPECT_EQ(res, 200);

    Optional<int> oi_2(100);
    if(!oi_2)
        res = 200;
    else
        res = 300;
    EXPECT_EQ(res, 300);
}

// выключение переменной с сохранением предыдущего значения
TEST(OPTIONAL, disable_value_getter) {
    using namespace simpleapi;

    Optional<int> oi;
    oi = 15;
    oi.unset();

    EXPECT_EQ(oi.value(), 15);
}
