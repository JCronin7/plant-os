#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>
#include <Adafruit_NeoPixel.h>

#define LED_STATUS_SUCCESS      0U

namespace led
{
    BaseType_t init(uint8_t ring_data);

    void builtin_set(bool state);

    void builtin_blink(uint32_t frequency);

    uint32_t builtin_cmd(uint8_t argc, char *argv[]);

    void ring_set(bool state);
}

#endif /* _LED_H_ */
