#include <hal.h>
// #if defined teensy40
// #include <usb_serial.h>
// #endif
#include <Arduino.h>

void vHalUartUsbSerialReset( void )
{
#if 0
    usb_serial_reset();
#endif
}

void vHalUartUsbSerialConfig( void )
{
#if 0
    usb_serial_configure();
#endif
}

char xHalUartUsbSerialGetChar( void )
{
#if 0
    return usb_serial_getchar();
#else
    return Serial.read();
#endif
}

char xHalUartUsbSerialPeekChar( void )
{
#if 0
    return usb_serial_peekchar();
#else
    return Serial.peek();
#endif
}

uint32_t xHalUartUsbSerialAvailable( void )
{
#if 0
    return usb_serial_available();
#else
    return Serial.available();
#endif
}

uint32_t xHalUartUsbSerialRead( char *ucBuffer, uint32_t ulSize )
{
#if 0
    return usb_serial_read(ucBuffer, ulSize);
#else
    return Serial.readBytes(ucBuffer, ulSize);
#endif
}

void vHalUartUsbSerialFlushInput( void )
{
#if 0
    usb_serial_flush_input();
#else
    return Serial.flush();
#endif
}

uint32_t xHalUartUsbSerialPutChar( char ucByte )
{
#if 0
    return usb_serial_putchar(ucByte);
#else
    return Serial.write(ucByte);
#endif
}

uint32_t xHalUartUsbSerialWrite( char *ucBuffer, uint32_t ulSize )
{
#if 0
    return usb_serial_write(ucBuffer, ulSize);
#else
    return Serial.write(ucBuffer, ulSize);
#endif
}

uint32_t xHalUartUsbSerialWriteBufferFree( void )
{
#if 0
    return usb_serial_write_buffer_free();
#else
    return 0;
#endif
}

void vHalUartUsbSerialFlushOutput( void )
{
#if 0
    usb_serial_flush_output();
#endif
}

uint32_t xHalUartUsbSerialPrint( const void * const pvStr )
{
    size_t xBytesPut = 0;
    const char * const pcStr = (const char * const)pvStr;

    while ( (char)pcStr[xBytesPut] != '\0' )
    {
        xHalUartUsbSerialPutChar(pcStr[xBytesPut++]);
    }

    return xBytesPut;
}

uint32_t xHalUartUsbSerialPrintln( const void * const pvStr )
{
    size_t xBytesPut = 0;

    xBytesPut = xHalUartUsbSerialPrint(pvStr);

    return ( xHalUartUsbSerialPutChar('\n') == 0 ) ? xBytesPut + 1 : xBytesPut;
}