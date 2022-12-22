#ifndef _MOIST_SENSOR_H_
#define _MOIST_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>

#define MOISTURE_SENSOR_MAX_INSTANCES ( 1 )

class Ek1940Sensor
{
private:
    uint8_t ucDataOut;
    uint8_t ucInstIdx;

public:
    Ek1940Sensor(uint8_t ucDataOut);
    uint32_t read(void);
    static uint32_t cmdsvr(uint8_t argc,
                           char *argv[] );
};

#endif /* _MOIST_SENSOR_H_ */
