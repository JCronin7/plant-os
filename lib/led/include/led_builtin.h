#ifndef _LED_BUILTIN_H_
#define _LED_BUILTIN_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

#ifdef __cplusplus
extern "C" {
#endif

BaseType_t xLedBuiltinInit(void);
void vLedBuiltinSet(bool ucState);
void vLedBuiltinBlink(uint32_t ulFrequency);
uint32_t ulLedBuiltinCmdsvr(uint8_t argc, char *argv[]);

#ifdef __cplusplus
}
#endif
#endif /* _LED_BUILTIN_H_ */