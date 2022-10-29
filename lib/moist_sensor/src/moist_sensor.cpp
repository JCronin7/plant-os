#include <moist_sensor.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

#define PIN 23

using namespace moist_sensor;

int ek1940::read(void)
{
    return analogRead(PIN);
}

BaseType_t ek1940::init(void)
{
    pinMode(PIN, INPUT);
    return 1;
}

uint32_t ek1940::cmd(uint8_t argc, char *argv[])
{
    return read();
}