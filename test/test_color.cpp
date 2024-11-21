#include "color.h"
#include "unity.h"

using namespace thermocam::color;

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_encode_rgb_to_int(void)
{
    TEST_ASSERT_EQUAL_INT32(0, encode_rgb_to_int(0, 0, 0));
    TEST_ASSERT_EQUAL_INT32(16777215, encode_rgb_to_int(255, 255, 255));
    TEST_ASSERT_EQUAL_INT32(65280, encode_rgb_to_int(0, 255, 0));
}

void test_common_color_enum(void)
{
    TEST_ASSERT_EQUAL_INT32(0, CommonColor::BLACK);
    TEST_ASSERT_EQUAL_INT32(16777215, CommonColor::WHITE);
    TEST_ASSERT_EQUAL_INT32(16711680, CommonColor::RED);
    TEST_ASSERT_EQUAL_INT32(65280, CommonColor::GREEN);
    TEST_ASSERT_EQUAL_INT32(255, CommonColor::BLUE);
    TEST_ASSERT_EQUAL_INT32(16776960, CommonColor::YELLOW);
    TEST_ASSERT_EQUAL_INT32(65535, CommonColor::CYAN);
    TEST_ASSERT_EQUAL_INT32(16711935, CommonColor::MAGENTA);
}

void test_decode_int_to_rgb(void)
{
    uint8_t array_white[] = {0, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_white, &decode_int_to_rgb(0)[0], 3);

    uint8_t array_black[] = {255, 255, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_black, &decode_int_to_rgb(16777215)[0], 3);

    uint8_t array_blue[] = {0, 0, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_blue, &decode_int_to_rgb(255)[0], 3);

    uint8_t array_green[] = {0, 255, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_green, &decode_int_to_rgb(65280)[0], 3);

    uint8_t array_magenta[] = {255, 0, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_magenta, &decode_int_to_rgb(16711935)[0], 3);
}

void test_color_enum_factory_and_rgb_getter(void)
{
    auto white_color = RGB8Color::create_from_enum(CommonColor::WHITE);
    TEST_ASSERT_EQUAL_UINT8(255, white_color.r());
    TEST_ASSERT_EQUAL_UINT8(255, white_color.g());
    TEST_ASSERT_EQUAL_UINT8(255, white_color.b());

    auto black_color = RGB8Color::create_from_enum(CommonColor::BLACK);
    TEST_ASSERT_EQUAL_UINT8(0, black_color.r());
    TEST_ASSERT_EQUAL_UINT8(0, black_color.g());
    TEST_ASSERT_EQUAL_UINT8(0, black_color.b());

    auto green_color = RGB8Color::create_from_enum(CommonColor::GREEN);
    TEST_ASSERT_EQUAL_UINT8(0, green_color.r());
    TEST_ASSERT_EQUAL_UINT8(255, green_color.g());
    TEST_ASSERT_EQUAL_UINT8(0, green_color.b());
}

void test_color_rgb_factory_and_rgb_array_getter(void)
{
    auto white_color = RGB8Color::create_from_rgb(0, 0, 0);
    uint8_t array_white[] = {0, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_white, &white_color.rgb_array()[0], 3);

    auto black_color = RGB8Color::create_from_rgb(255, 255, 255);
    uint8_t array_black[] = {255, 255, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_black, &black_color.rgb_array()[0], 3);

    auto green_color = RGB8Color::create_from_rgb(0, 255, 0);
    uint8_t array_green[] = {0, 255, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(array_green, &green_color.rgb_array()[0], 3);
}

void test_lerp(void)
{
    auto low_frac_color = RGB8Color::lerp(RGB8Color::create_from_enum(CommonColor::BLACK),
                                          RGB8Color::create_from_enum(CommonColor::RED), 0.0);
    uint8_t low_frac_array[] = {0, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(low_frac_array, &low_frac_color.rgb_array()[0], 3);

    auto high_frac_color = RGB8Color::lerp(RGB8Color::create_from_enum(CommonColor::BLACK),
                                           RGB8Color::create_from_enum(CommonColor::RED), 1.0);
    uint8_t high_frac_array[] = {255, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(high_frac_array, &high_frac_color.rgb_array()[0], 3);

    auto mid_frac_color = RGB8Color::lerp(RGB8Color::create_from_enum(CommonColor::BLACK),
                                          RGB8Color::create_from_enum(CommonColor::RED), 0.5);
    uint8_t mid_frac_array[] = {127, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(mid_frac_array, &mid_frac_color.rgb_array()[0], 3);

    auto mid_frac_color2 = RGB8Color::lerp(RGB8Color::create_from_enum(CommonColor::BLACK),
                                           RGB8Color::create_from_enum(CommonColor::WHITE), 0.5);
    uint8_t mid_frac_array2[] = {127, 127, 127};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(mid_frac_array2, &mid_frac_color2.rgb_array()[0], 3);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_encode_rgb_to_int);
    RUN_TEST(test_common_color_enum);
    RUN_TEST(test_decode_int_to_rgb);
    RUN_TEST(test_color_enum_factory_and_rgb_getter);
    RUN_TEST(test_color_rgb_factory_and_rgb_array_getter);
    RUN_TEST(test_lerp);
    return UNITY_END();
}

// WARNING!!! PLEASE REMOVE UNNECESSARY MAIN IMPLEMENTATIONS //

/**
 * For native dev-platform or for some embedded frameworks
 */
int main(void)
{
    return runUnityTests();
}
