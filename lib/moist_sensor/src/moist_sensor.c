#include <moist_sensor.h>
#include <string.h>
#include <util.h>

static Ek1940Sensor_t *xInstances[MOISTURE_SENSOR_MAX_INSTANCES] = {NULL};
static uint32_t ulInstanceCount = 0;

uint32_t ulMoistureSensorRead(Ek1940Sensor_t *pxInst)
{
    return analogRead(pxInst->ucDataOut);
}

BaseType_t ulMoistureSensorInit(Ek1940Sensor_t *pxInst, uint8_t ucDataOut)
{
    xInstances[ulInstanceCount] = pxInst;
    pxInst->ucDataOut = ucDataOut;
    pxInst->ucInstIdx = ulInstanceCount++;

    pinMode(ucDataOut, INPUT);

    return 1;
}

uint32_t ulMoistureSensorCmdsvr(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return ulMoistureSensorRead(xInstances[0]);
    }

    return ulMoistureSensorRead(xInstances[atoi(argv[0])]);
}