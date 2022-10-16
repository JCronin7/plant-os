#include <led.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)  ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

void led::builtin_init(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void led::builtin_set(bool state)
{
    digitalWrite(LED_BUILTIN, state);
}

void led::builtin_blink(float frequency)
{
    analogWriteFrequency(LED_BUILTIN, frequency);
    analogWrite(LED_BUILTIN, 128);
}

uint32_t led::builtin_cmd(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if (iscommand(argv[0], "on", sizeof("on") - 1))
    {
        led::builtin_set(HIGH);
    }
    else if (iscommand(argv[0], "off", sizeof("off") - 1))
    {
        led::builtin_set(LOW);
    }
    else if (iscommand(argv[0], "blink", sizeof("blink") - 1) && argc == 2)
    {
        led::builtin_blink(atof(argv[1]));
    }

    return LED_STATUS_SUCCESS;
}