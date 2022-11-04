#include <hal.h>
#if defined ( teensy40 )
#include <i2c_driver_wire.h>
static I2CDriverWire *xWire = &Wire;
#elif defined ( mkrwifi1010 )
#include <Arduino.h>
#include <Wire.h>
static TwoWire *xWire = &Wire;
#else
#error "Unsupported hardware platform"
#endif

void vHalI2cBeginTransmission( uint8_t ucByte )
{
    xWire->beginTransmission(ucByte);
}

uint8_t xHalI2cEndTransmission( void )
{
    return xWire->endTransmission();
}

uint32_t xHalI2cWrite( uint8_t ucByte )
{
    return xWire->write(ucByte);
}

uint8_t xHalI2cRequestFrom( uint8_t ucAddress,
                            uint32_t ulBytes)
{
    return xWire->requestFrom(ucAddress,
                             ulBytes);
}

uint8_t xHalI2cRead( void )
{
    return xWire->read();
}

void vHalI2cBegin( void )
{
    xWire->begin();
}