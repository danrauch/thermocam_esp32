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

void test_if_size_still_trivially_copyable(void)
{
    TEST_ASSERT_EQUAL_INT64(3, sizeof(RGB8Color));
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

void test_operator_add(void)
{
    auto color1 = RGB8Color::create_from_rgb(100, 90, 50);
    auto color2 = RGB8Color::create_from_rgb(1, 2, 3);
    uint8_t result_array1[] = {101, 92, 53};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array1, &(color1 + color2).rgb_array()[0], 3);

    auto color3 = RGB8Color::create_from_rgb(20, 14, 190);
    auto color4 = RGB8Color::create_from_rgb(10, 13, 100);
    uint8_t result_array2[] = {30, 27, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array2, &(color3 + color4).rgb_array()[0], 3);
}

void test_operator_minus(void)
{
    auto color1 = RGB8Color::create_from_rgb(100, 90, 50);
    auto color2 = RGB8Color::create_from_rgb(1, 2, 3);
    uint8_t result_array1[] = {99, 88, 47};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array1, &(color1 - color2).rgb_array()[0], 3);

    auto color3 = RGB8Color::create_from_rgb(20, 14, 190);
    auto color4 = RGB8Color::create_from_rgb(10, 13, 200);
    uint8_t result_array2[] = {10, 1, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array2, &(color3 - color4).rgb_array()[0], 3);
}

void test_operator_multiply(void)
{
    auto color1 = RGB8Color::create_from_rgb(100, 90, 50);
    auto color2 = RGB8Color::create_from_rgb(1, 2, 3);
    uint8_t result_array1[] = {100, 180, 150};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array1, &(color1 * color2).rgb_array()[0], 3);

    auto color3 = RGB8Color::create_from_rgb(20, 14, 190);
    auto color4 = RGB8Color::create_from_rgb(10, 13, 15);
    uint8_t result_array2[] = {200, 182, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result_array2, &(color3 * color4).rgb_array()[0], 3);
}

void test_operator_string(void)
{
    auto color1 = RGB8Color::create_from_rgb(100, 90, 50);
    TEST_ASSERT_EQUAL_STRING(std::string(color1).c_str(), "(100 90 50)");
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

void test_discrete_blend(void)
{
    auto color_vector = RGB8Color::discrete_blend(RGB8Color::create_from_enum(CommonColor::BLACK),
                                                  RGB8Color::create_from_enum(CommonColor::WHITE),
                                                  3);
    TEST_ASSERT_EQUAL_INT32(3, color_vector.size());

    uint8_t color1_array[] = {0, 0, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(color1_array, &color_vector[0].rgb_array()[0], 3);
    uint8_t color2_array[] = {127, 127, 127};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(color2_array, &color_vector[1].rgb_array()[0], 3);
    uint8_t color3_array[] = {255, 255, 255};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(color3_array, &color_vector[2].rgb_array()[0], 3);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_size_still_trivially_copyable);
    RUN_TEST(test_encode_rgb_to_int);
    RUN_TEST(test_common_color_enum);
    RUN_TEST(test_decode_int_to_rgb);
    RUN_TEST(test_color_enum_factory_and_rgb_getter);
    RUN_TEST(test_color_rgb_factory_and_rgb_array_getter);
    RUN_TEST(test_operator_add);
    RUN_TEST(test_operator_minus);
    RUN_TEST(test_operator_multiply);
    RUN_TEST(test_operator_string);
    RUN_TEST(test_lerp);
    RUN_TEST(test_discrete_blend);
    return UNITY_END();
}


int main(void)
{
    return runUnityTests();
}
