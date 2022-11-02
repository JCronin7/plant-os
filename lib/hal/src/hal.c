#include <hal.h>

void vHalInit( uint32_t ulBaudrate )
{
    const uint32_t ulCurrentSysTick = systick_millis_count;

    (void)ulBaudrate;
    /// @todo this breaks serial, figure out how to use.
    // vHalUartUsbSerialConfig();
    while ( (systick_millis_count - ulCurrentSysTick) < 2000 )
        ;
}