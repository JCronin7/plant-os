#include <util.h>
#include <hal.h>

char *pcUtilNextChar( const char *cpStr )
{
    while ( *cpStr == ' ' ) { cpStr++; }
    return (char *)cpStr;
}

void vUtilClearLine(uint32_t ulLineSize)
{
    xHalUartUsbSerialPutChar('\r');
    for (uint32_t i = 0; i < ulLineSize; i++)
    {
        xHalUartUsbSerialPutChar(' ');
    }

    xHalUartUsbSerialPutChar('\r');
}

void vUtilStrClear( char * cpStr, uint32_t ulLen )
{
    while ( ulLen-- ) { *cpStr++ = 0; }
}
