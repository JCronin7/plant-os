#ifndef _MOIST_SENSOR_H_
#define _MOIST_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

namespace moist_sensor
{
    class ek1940
    {
    private:
        static int read(void);

    public:
        static BaseType_t init(void);

        static uint32_t cmd(uint8_t argc, char *argv[]);
    };
}

#endif /* _MOIST_SENSOR_H_ */
