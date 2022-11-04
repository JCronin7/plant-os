#ifndef _MOIST_SENSOR_H_
#define _MOIST_SENSOR_H_

#include <stdint.h>
#if defined ( teensy40 )
#include <FreeRTOS_TEENSY4.h>
#elif defined ( mkrwifi1010 )
#include <FreeRTOS_SAMD21.h>
#else
#error "Unsupported hardware platform"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MOISTURE_SENSOR_MAX_INSTANCES ( 1 )

typedef struct Ek1940Sensor
{
    uint8_t ucDataOut;
    uint8_t ucInstIdx;
} Ek1940Sensor_t;

uint32_t ulMoistureSensorRead( Ek1940Sensor_t *pxInst );
BaseType_t ulMoistureSensorInit( Ek1940Sensor_t *pxInst,
                                 uint8_t ucDataOut );
uint32_t ulMoistureSensorCmdsvr( uint8_t argc,
                                 char *argv[] );

#ifdef __cplusplus
}
#endif
#endif /* _MOIST_SENSOR_H_ */
