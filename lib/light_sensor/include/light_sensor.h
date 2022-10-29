#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>
#include <i2c_driver_wire.h>
#include <stream_buffer.h>

#define MAX_INSTANCES           1
#define ADC_DATA_BUFFER_SIZE    1024

namespace light_sensor
{
    class bh1750
    {
    private:
        static uint8_t instance_count;

        static bh1750 *instances[MAX_INSTANCES];

        StreamBufferHandle_t adc_buffer;

        TickType_t last_measure;

        I2CDriverWire *i2c_driver;

        uint8_t i2c_address;

        uint8_t power_state;

        uint8_t mode;

        uint8_t measure_delay;

        uint8_t set_opcode(uint8_t op);

        uint16_t read(void);

    public:
        uint8_t inst_idx;

        bh1750(void);

        BaseType_t init(bool address, I2CDriverWire& wire);

        uint8_t set_power(uint8_t state);

        uint8_t reset(void);

        uint8_t set_mode(uint8_t mode);

        uint8_t set_measure_delay(uint8_t measure_delay);

        uint32_t measure(void);

        float convert(uint16_t raw);

        static void measure_task(void *arg);

        static uint32_t cmd(uint8_t argc, char *argv[]);

    };
}
#endif /* _LIGHT_SENSOR_H_ */
