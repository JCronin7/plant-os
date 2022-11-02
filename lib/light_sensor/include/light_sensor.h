#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <stdint.h>
#ifdef teensy40
#include <FreeRTOS_TEENSY4.h>
#include <i2c_driver_wire.h>
#endif
#include <stream_buffer.h>

#define MAX_INSTANCES           1
#define ADC_DATA_BUFFER_SIZE    1024

union Adc16Data_t
{
    uint16_t usData;
    struct
    {
        uint8_t ucLo;
        uint8_t ucHi;
    } xBytes;
};

struct Bh1750Sensor_t
{
    StreamBufferHandle_t xAdcDataBufferHdl;
    TickType_t xLastMeasure;
    I2CDriverWire *xI2cDriver;
    uint8_t xI2cAddress;
    uint8_t ucPowerState;
    uint8_t ucMode;
    uint8_t ucMeasureDelay;
    uint8_t ucInstIdx;
};

BaseType_t xLightSensorinit(Bh1750Sensor_t *pxInst, I2CDriverWire *pxWire, bool ucAddr);
uint8_t ucLightSensorSetPower(Bh1750Sensor_t *pxInst, uint8_t ucState);
uint8_t ucLightSensorReset(Bh1750Sensor_t *pxInst);
uint8_t ucLightSensorSetMode(Bh1750Sensor_t *pxInst, uint8_t ucMode);
uint8_t ucLightSensorSetMeasureDelay(Bh1750Sensor_t *pxInst, uint8_t ucMeasureDelay);
int32_t ulLightSensorMeasure(Bh1750Sensor_t *pxInst);
uint32_t ulLightSensorCmdsvr(uint8_t argc, char *argv[]);

#endif /* _LIGHT_SENSOR_H_ */
