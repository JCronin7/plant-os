#ifndef _HAL_H_
#define _HAL_H_

#include <stdint.h>
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/**
 * @brief
 *  Reset Serial module
 *
 */
EXTERNC void vHalUartUsbSerialReset( void );

/**
 * @brief
 *
 */
EXTERNC void vHalUartUsbSerialConfig( void );

/**
 * @brief
 *
 */
EXTERNC char xHalUartUsbSerialGetChar( void );

/**
 * @brief
 *
 */
EXTERNC char xHalUartUsbSerialPeekChar( void );

/**
 * @brief
 *
 */
EXTERNC uint32_t xHalUartUsbSerialAvailable( void );

/**
 * @brief
 *
 */
EXTERNC uint32_t xHalUartUsbSerialRead( char *ucBuffer, uint32_t ulSize );

/**
 * @brief
 *
 */
EXTERNC void vHalUartUsbSerialFlushInput( void );

/**
 * @brief
 *
 */
EXTERNC uint32_t xHalUartUsbSerialPutChar( char ucByte );

/**
 * @brief
 *
 */
EXTERNC uint32_t xHalUartUsbSerialWrite( char *ucBuffer, uint32_t ulSize );

/**
 * @brief
 *
 */
EXTERNC uint32_t xHalUartUsbSerialWriteBufferFree( void );

/**
 * @brief
 *
 */
EXTERNC void vHalUartUsbSerialFlushOutput( void );

EXTERNC void vHalI2cBeginTransmission( uint8_t ucByte );
EXTERNC uint8_t xHalI2cEndTransmission( void );
EXTERNC uint32_t xHalI2cWrite( uint8_t ucByte );
EXTERNC uint8_t xHalI2cRequestFrom( uint8_t ucAddress,
                                    uint32_t ulBytes);
EXTERNC uint8_t xHalI2cRead( void );
EXTERNC void vHalI2cBegin( void );

EXTERNC void vHalInit( uint32_t baudrate );
EXTERNC uint32_t xHalUartUsbSerialPrint( const void * const pvStr );
EXTERNC uint32_t xHalUartUsbSerialPrintln( const void * const pvStr );

#undef EXTERNC

#endif /* _HAL_H_ */
