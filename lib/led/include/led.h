#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

#define LED_STATUS_SUCCESS      0U

namespace led
{
    void builtin_init(void);

    void builtin_set(bool state);

    void builtin_blink(float frequency);

    uint32_t builtin_cmd(uint8_t argc, char *argv[]);
}

#endif /* _LED_H_ */
