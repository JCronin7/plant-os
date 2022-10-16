#include <led.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)  (strstr(str1, str2) == str1 && *(str1 + str2_len) <= ' ')

void led::builtin_init(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void led::builtin_set(bool state)
{
    digitalWrite(LED_BUILTIN, state);
}

void led::builtin_blink(uint32_t frequency)
{
    (void)frequency;
}

uint32_t led::builtin_cmd(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if (iscommand(argv[0], "on", sizeof("on")))
    {
        led::builtin_set(HIGH);
    }
    else if (iscommand(argv[0], "off", sizeof("off")))
    {
        led::builtin_set(LOW);
    }
    else if (iscommand(argv[0], "blink", sizeof("blink")))
    {
        led::builtin_blink(0);
    }

    return LED_STATUS_SUCCESS;
}