#ifndef _APP_H_
#define _APP_H_

#include <stdint.h>
#include <hal.h>
#include <light_sensor.h>
#include <moist_sensor.h>
#include <pump.h>
#include <led.h>
#include <FreeRTOS_SAMD21.h>

extern Ek1940Sensor moist_sensor;
extern Pump pump;

void app_main(void *args);

TaskHandle_t *app_task_hdl_get(void);

uint32_t app_cmdsvr(uint8_t argc, char *argv[]);

BaseType_t app_init(UBaseType_t task_priority,
                    uint16_t task_stack_depth);

#endif /* _APP_H_ */