#ifndef _LED_RING_H_
#define _LED_RING_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>
#include <Adafruit_NeoPixel.h>

BaseType_t vLedRingInit( uint8_t ucRingDataPin );
void vLedRingSetColor( uint8_t ucRed,
                       uint8_t ucGreen,
                       uint8_t ucBlue );
void vLedRingSet( bool ucState );
uint32_t ulLedRingCmdsvr( uint8_t argc,
                          char *argv[] );

#endif /* _LED_RING_H_ */
