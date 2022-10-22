#ifndef _LSENSE_H_
#define _LSENSE_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>
#include <i2c_driver_wire.h>

#define MAX_INSTANCES   5

namespace lsense
{
    class bh1750
    {
    private:
        static uint8_t instance_count;

        static bh1750 *instances[MAX_INSTANCES];

        I2CDriverWire *i2c_driver;

        uint8_t i2c_address;

        uint8_t set_opcode(uint8_t op);

        uint16_t read(void);

    public:
        bh1750(void);

        BaseType_t init(bool address, I2CDriverWire& wire);

        static uint32_t cmd(uint8_t argc, char *argv[]);

    };
}
#endif /* _LSENSE_H_ */
