#include <Arduino.h>
#include <unity.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test(void)
{
    TEST_ASSERT_EQUAL(true, true);
}

void setup(void)
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test);
}

void loop(void)
{
    UNITY_END(); // stop unit testing
}