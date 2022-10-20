#include <lsense.h>
#include <string.h>
#include <i2c_driver_wire.h>

#define LSENSE_ADDRESS 0b0100011

#define IS_COMMAND(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

//I2CDriverWire Wire;

union adc_data
{
    uint16_t data;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    } bytes;
};

static uint16_t get_measure(void)
{
    adc_data meas;
    Wire.beginTransmission(LSENSE_ADDRESS);
    Wire.write((uint8_t)((0x10 << 1) | 0));
    Wire.endTransmission();
    delay(200);

    Wire.beginTransmission(LSENSE_ADDRESS);
    Wire.write((uint8_t)((0x10 << 1) | 1));
    meas.bytes.hi = Wire.read();
    meas.bytes.lo = Wire.read();
    Wire.endTransmission();

    return meas.data;
}

BaseType_t lsense::init(void)
{
    Wire.begin();
    return pdTRUE;
}

uint32_t lsense::cmd(uint8_t argc, char *argv[])
{
    return get_measure();
}
