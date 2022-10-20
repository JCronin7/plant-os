#include <led.h>
#include <string.h>

#define LED_RING_PIXEL_COUNT    24U

#define IS_COMMAND(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))
#define SET_BUILTIN_LED(VAL) \
do \
{ \
    vPortEnterCritical(); \
    digitalWrite(LED_BUILTIN, (VAL)); \
    vPortExitCritical(); \
} while (0);

Adafruit_NeoPixel led_ring;

static uint32_t builtin_blink_frequency = 0;
static TaskHandle_t led_builtin_blink_hdl = nullptr;

static void led_builtin_blink_task(void *arg)
{
    TickType_t previous_wake = 0;
    static uint32_t count = 0;
    static bool state = false;

    while (true)
    {
        if (builtin_blink_frequency == 0)
        {
            vTaskSuspend(led_builtin_blink_hdl);
        }

        if (count++ >= builtin_blink_frequency)
        {
            count = 0;
            SET_BUILTIN_LED(state);
            state = state ? false : true;
        }

        vTaskDelayUntil(&previous_wake, 1);
    }
}

BaseType_t led::init(uint8_t ring_data)
{
    BaseType_t status;

    pinMode(LED_BUILTIN, OUTPUT);

    SET_BUILTIN_LED(LOW);

    builtin_blink_frequency = 0;
    status = xTaskCreate(led_builtin_blink_task,
                         NULL,
                         100 * configMINIMAL_STACK_SIZE,
                         NULL,
                         2,
                         &led_builtin_blink_hdl);

    led_ring = Adafruit_NeoPixel(LED_RING_PIXEL_COUNT, ring_data, NEO_GRB + NEO_KHZ800);
    led_ring.begin();
    led_ring.setBrightness(10);

    return status;
}

void led::builtin_set(bool state)
{
    vTaskSuspend(led_builtin_blink_hdl);
    SET_BUILTIN_LED(state);
}

void led::builtin_blink(uint32_t frequency)
{
    builtin_blink_frequency = frequency;
    vTaskResume(led_builtin_blink_hdl);
}

uint32_t led::builtin_cmd(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if (IS_COMMAND(argv[0], "on", sizeof("on") - 1))
    {
        led::builtin_set(HIGH);
    }
    else if (IS_COMMAND(argv[0], "off", sizeof("off") - 1))
    {
        led::builtin_set(LOW);
    }
    else if (IS_COMMAND(argv[0], "blink", sizeof("blink") - 1) && argc == 2)
    {
        led::builtin_blink((uint32_t)atoi(argv[1]));
    }

    return LED_STATUS_SUCCESS;
}

void led::ring_set(bool state)
{
    while (true)
    {
        led_ring.clear();
        for (uint32_t i = 0; i < LED_RING_PIXEL_COUNT; i++)
        {
            led_ring.setPixelColor(i, led_ring.Color(0xFF, 0x50, 0x5B));
            led_ring.show();
            delay(50);
        }
    }
}