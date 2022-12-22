#include <moist_sensor.h>
#include <string.h>

static Ek1940Sensor *xInstances[MOISTURE_SENSOR_MAX_INSTANCES] = { NULL };
static uint32_t ulInstanceCount = 0;

Ek1940Sensor::Ek1940Sensor(uint8_t ucDataOut)
{
    xInstances[ulInstanceCount] = this;
    ucDataOut = ucDataOut;
    ucInstIdx = ulInstanceCount++;

    pinMode(ucDataOut, INPUT);
}

uint32_t Ek1940Sensor::read(void)
{
    return analogRead(ucDataOut);
}

uint32_t Ek1940Sensor::cmdsvr(uint8_t argc, char *argv[])
{
    int32_t usData;
    Ek1940Sensor *pxInst = (argc < 1) ? xInstances[0] : xInstances[atoi(argv[1])];

    usData = pxInst->read();
    Serial.println(usData);
    return 0;
}