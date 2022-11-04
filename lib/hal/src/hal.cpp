#include <hal.h>
#include <Arduino.h>

void vHalInit( uint32_t ulBaudrate )
{
#if 0
    const uint32_t ulCurrentSysTick = systick_millis_count;

    (void)ulBaudrate;
    /// @todo this breaks serial, figure out how to use.
    // vHalUartUsbSerialConfig();
    while ( (systick_millis_count - ulCurrentSysTick) < 2000 )
        ;
#else
    Serial.begin(ulBaudrate);
    vHalI2cBegin();
#endif
}