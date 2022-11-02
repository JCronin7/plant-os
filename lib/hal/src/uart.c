#include <hal.h>

size_t xHalUartUsbSerialPrint( const void * const pvStr )
{
    size_t xBytesPut = 0;
    const char * const pcStr = (const char * const)pvStr;

    while ( (char)pcStr[xBytesPut] != '\0' )
    {
        xHalUartUsbSerialPutChar(pcStr[xBytesPut++]);
    }

    return xBytesPut;
}

size_t xHalUartUsbSerialPrintln( const void * const pvStr )
{
    size_t xBytesPut = 0;

    xBytesPut = xHalUartUsbSerialPrint(pvStr);

    return ( xHalUartUsbSerialPutChar('\n') == 0 ) ? xBytesPut + 1 : xBytesPut;
}