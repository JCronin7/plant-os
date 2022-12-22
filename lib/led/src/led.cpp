#include <led.h>

#define LED_RING_PIXEL_COUNT    ( 24U )

static Adafruit_NeoPixel led_ring;
static uint32_t ring_color = 0;

void led_builtin_set(bool state)
{
    digitalWrite(LED_BUILTIN, state);
}

void led_ring_set(bool state)
{
    (state) ? led_ring.fill(ring_color) : led_ring.clear();
    led_ring.show();
}

BaseType_t led_init(uint8_t led_ring_data_pin)
{
    pinMode(LED_BUILTIN, OUTPUT);
    led_ring = Adafruit_NeoPixel(LED_RING_PIXEL_COUNT,
                                 led_ring_data_pin,
                                 NEO_GRB + NEO_KHZ800);

    led_builtin_set(LOW);
    led_ring.begin();
    led_ring.setBrightness(10);

    return 1;
}

uint32_t led_cmdsvr(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if (strcmp(argv[1], "builtin") == 0)
    {
        if (strcmp(argv[2], "on") == 0)
        {
            led_builtin_set(HIGH);
        }
        else if (strcmp(argv[2], "off") == 0)
        {
            led_builtin_set(LOW);
        }
        else
        {
            return LED_STATUS_UNKNOWN_CMD;
        }
    }
    else if (strcmp(argv[1], "ring") == 0)
    {
        if (strcmp(argv[2], "on") == 0)
        {
            led_ring_set(HIGH);
        }
        else if (strcmp(argv[2], "off") == 0)
        {
            led_ring_set(LOW);
        }
        else
        {
            return LED_STATUS_UNKNOWN_CMD;
        }
    }

    return LED_STATUS_SUCCESS;
}
