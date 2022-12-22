#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <Wire.h>

#define MAX_INSTANCES 1

typedef union Adc16Data
{
    uint16_t usData;
    struct
    {
        uint8_t ucLo;
        uint8_t ucHi;
    } xBytes;
} Adc16Data_t;

class Bh1750Sensor
{
private:
    TickType_t xLastMeasure;
    uint8_t xI2cAddress;
    uint8_t ucPowerState;
    uint8_t ucMode;
    uint8_t ucMeasureDelay;
    uint8_t ucInstIdx;

    uint8_t setOpcode(uint8_t op);
    uint16_t read( void );

public:
    Bh1750Sensor(bool ucAddr);
    uint8_t setPower(uint8_t ucState);
    uint8_t reset(void);
    uint8_t setMode(uint8_t ucMode);
    uint8_t setMeasureDelay(uint8_t ucMeasureDelay);
    int32_t measure(void);

    static uint32_t cmdsvr(uint8_t argc, char *argv[]);
};

#endif /* _LIGHT_SENSOR_H_ */
