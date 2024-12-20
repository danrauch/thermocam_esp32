#include "fixed_matrix.h"
#include "unity.h"

using namespace thermocam;

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_instantiation_and_access(void)
{
    FixedSizeMatrix<int, 3, 2> matrix ( {1, 2, 3, 4, 5, 6} );
    TEST_ASSERT_EQUAL_INT64(matrix.rows(), 3);
    TEST_ASSERT_EQUAL_INT64(matrix.cols(), 2);
    TEST_ASSERT_EQUAL_INT(matrix(0, 0), 1);
    TEST_ASSERT_EQUAL_INT(matrix(0, 1), 2);
    TEST_ASSERT_EQUAL_INT(matrix(1, 0), 3);
    TEST_ASSERT_EQUAL_INT(matrix(1, 1), 4);
    TEST_ASSERT_EQUAL_INT(matrix(2, 0), 5);
    TEST_ASSERT_EQUAL_INT(matrix(2, 1), 6);
}


int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_instantiation_and_access);
    return UNITY_END();
}


int main(void)
{
    return runUnityTests();
}
