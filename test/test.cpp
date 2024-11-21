#include "unity.h"
#include "color.h"

using namespace thermocam::color;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_encode_rgb_to_int(void) {
  TEST_ASSERT_EQUAL_INT32(0, encode_rgb_to_int(0, 0, 0));
  TEST_ASSERT_EQUAL_INT32(16777215, encode_rgb_to_int(255, 255, 255));
  TEST_ASSERT_EQUAL_INT32(65280, encode_rgb_to_int(0, 255, 0));
}

void test_common_color_enum(void) {
  TEST_ASSERT_EQUAL_INT32(0, CommonColor::BLACK);
  TEST_ASSERT_EQUAL_INT32(16777215, CommonColor::WHITE);
  TEST_ASSERT_EQUAL_INT32(16711680, CommonColor::RED);
  TEST_ASSERT_EQUAL_INT32(65280, CommonColor::GREEN);
  TEST_ASSERT_EQUAL_INT32(255, CommonColor::BLUE);
  TEST_ASSERT_EQUAL_INT32(16776960, CommonColor::YELLOW);
  TEST_ASSERT_EQUAL_INT32(65535, CommonColor::CYAN);
  TEST_ASSERT_EQUAL_INT32(16711935, CommonColor::MAGENTA);
}

void test_decode_int_to_rgb(void) {
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


int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_encode_rgb_to_int);
  RUN_TEST(test_common_color_enum);
  RUN_TEST(test_decode_int_to_rgb);
  return UNITY_END();
}

// WARNING!!! PLEASE REMOVE UNNECESSARY MAIN IMPLEMENTATIONS //

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) {
  return runUnityTests();
}
