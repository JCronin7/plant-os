#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <Adafruit_NeoPixel.h>

#define LED_STATUS_SUCCESS      0U
#define LED_STATUS_UNKNOWN_CMD  1U

void led_builtin_set(bool state);
void led_ring_set(bool state);
BaseType_t led_init(uint8_t led_ring_data_pin);
uint32_t led_cmdsvr(uint8_t argc, char *argv[]);

#endif /* _LED_H_ */