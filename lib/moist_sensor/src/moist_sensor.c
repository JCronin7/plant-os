#include <moist_sensor.h>
#include <string.h>
#include <stdio.h>
#include <util.h>
#include <hal.h>

static Ek1940Sensor_t *xInstances[MOISTURE_SENSOR_MAX_INSTANCES] = { NULL };
static uint32_t ulInstanceCount = 0;

uint32_t ulMoistureSensorRead( Ek1940Sensor_t *pxInst )
{
    return analogRead(pxInst->ucDataOut);
}

BaseType_t ulMoistureSensorInit( Ek1940Sensor_t *pxInst,
                                 uint8_t ucDataOut )
{
    xInstances[ulInstanceCount] = pxInst;
    pxInst->ucDataOut = ucDataOut;
    pxInst->ucInstIdx = ulInstanceCount++;

    pinMode(ucDataOut, INPUT);

    return 1;
}

uint32_t ulMoistureSensorCmdsvr( uint8_t argc, char *argv[] )
{
    char cBuffer[64];
    int32_t usData;

    if ( argc < 1 )
    {
        usData = ulMoistureSensorRead(xInstances[0]);
        sprintf(cBuffer, "%ld       \n", usData);
        xHalUartUsbSerialPrint(cBuffer);
        return 0;
    }

    usData = ulMoistureSensorRead(xInstances[atoi(argv[0])]);
    sprintf(cBuffer, "%ld       \n", usData);
    xHalUartUsbSerialPrint(cBuffer);
    return 0;
}