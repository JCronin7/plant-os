#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>
#include <i2c_driver_wire.h>

#define MAX_INSTANCES           1
#define ADC_DATA_BUFFER_SIZE    1

namespace light_sensor
{
    class bh1750
    {
    private:
        static uint8_t instance_count;

        static bh1750 *instances[MAX_INSTANCES];

        uint16_t adc_data_buffer[ADC_DATA_BUFFER_SIZE];

        I2CDriverWire *i2c_driver;

        uint8_t i2c_address;

        uint8_t set_opcode(uint8_t op);

        uint16_t read(void);

    public:
        bh1750(void);

        BaseType_t init(bool address, I2CDriverWire& wire);

        float measure(void);

        static void measure_task(void *arg);

        static uint32_t cmd(uint8_t argc, char *argv[]);

    };
}
#endif /* _LIGHT_SENSOR_H_ */
