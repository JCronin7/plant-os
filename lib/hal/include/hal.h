#ifndef _HAL_H_
#define _HAL_H_

#include <stdint.h>
#include <usb_serial.h>

/**
 * @brief
 *  Reset Serial module
 *
 */
#define vHalUartUsbSerialReset( )               ( usb_serial_reset() )

/**
 * @brief
 *
 */
#define vHalUartUsbSerialConfig( )              ( usb_serial_configure() )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialGetChar( )             ( usb_serial_getchar() )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialPeekChar( )            ( usb_serial_peekchar() )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialAvailable( )           ( usb_serial_available() )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialRead(buffer, size)     ( usb_serial_read(buffer, size) )

/**
 * @brief
 *
 */
#define vHalUartUsbSerialFlushInput( )          ( usb_serial_flush_input() )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialPutChar(c)             ( usb_serial_putchar(c) )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialWrite(buffer, size)    ( usb_serial_write(buffer, size) )

/**
 * @brief
 *
 */
#define xHalUartUsbSerialWriteBufferFree()      ( usb_serial_write_buffer_free() )

/**
 * @brief
 *
 */
#define vHalUartUsbSerialFlushOutput()          ( usb_serial_flush_output() )

#ifdef __cplusplus
extern "C" {
#endif

void vHalInit(uint32_t baudrate);
size_t xHalUartUsbSerialPrint(const void * const pvStr);
size_t xHalUartUsbSerialPrintln(const void * const pvStr);

#ifdef __cplusplus
}
#endif
#endif /* _HAL_H_ */
