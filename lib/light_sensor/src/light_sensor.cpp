#include <light_sensor.h>
#include <string.h>
#include <hal.h>

/**
 * @brief
 *  Power Down (0000_0000) - No active state.
 *
 */
#define POWER_DOWN          0x00

/**
 * @brief
 *  Power On (0000_0001) - Waiting for measurement command.
 *
 */
#define POWER_ON            0x01

/**
 * @brief
 *  Reset (0000_0111) - Reset Data register value.
 *                      Reset command is not acceptable in Power Down mode.
 *
 */
#define RESET               0x7

/**
 * @brief
 *  Continuously H-Resolution Mode (0001_0000) - Start measurement at 1lx resolution.
 *                                               Measurement Time is typically 120ms.
 *
 */
#define CONT_HRES_MD1       0x10

/**
 * @brief
 *  Continuously H-Resolution Mode2 (0001_0001) - Start measurement at 0.5lx resolution.
 *                                                Measurement Time is typically 120ms.
 *
 */
#define CONT_HRES_MD2       0x11

/**
 * @brief
 *  Continuously L-Resolution Mode (0001_0011) -  Start measurement at 4lx resolution.
 *                                                Measurement Time is typically 16ms
 *
 */
#define CONT_LRES_MD        0x13

/**
 * @brief
 *  One Time H-Resolution Mode (0010_0000) - Start measurement at 1lx resolution.
 *                                           Measurement Time is typically 120ms.
 *                                           It is automatically set to Power Down mode after measurement.
 *
 */
#define OT_HRES_MD1         0x20

/**
 * @brief
 *  One Time H-Resolution Mode2 (0010_0001) - Start measurement at 0.5lx resolution.
 *                                            Measurement Time is typically 120ms.
 *                                            It is automatically set to Power Down mode after measurement.
 *
 */
#define OT_HRES_MD2         0x21

/**
 * @brief
 *  One Time L-Resolution Mode (0010_0011) - Start measurement at 4lx resolution.
 *                                           Measurement Time is typically 16ms.
 *                                           It is automatically set to Power Down mode after measurement.
 *
 */
#define OT_LRES_MD          0x23

/**
 * @brief
 *  Change Measurement time  ( High bit ) (01000_MT[7,6,5]) - Change measurement time.
 *                                                            Please refer "adjust measurement result for influence of optical window."
 *
 */
#define CHG_MTIME_HI(MT)    ( 0x40 | ((MT) >> 5) )

/**
 * @brief
 *  Change Masurement time  ( Low bit ) (011_MT[4,3,2,1,0]) - Change measurement time.
 *                                                            Please refer "adjust measurement result for influence of optical window."
 *
 */
#define CHG_MTIME_LO(MT)    ( 0x60 | ((MT) & 0x1F) )

#define LSENSE_BH1750_ADDRESS_LOW   ( 0x23 )
#define LSENSE_BH1750_ADDRESS_HIGH  ( 0x5C )
#define LSENSE_BH1750_LOOP_RATE     ( 120ul / portTICK_PERIOD_MS )

#define LSENSE_BH1750_ADDRESS(PIN)  ( (PIN) ? LSENSE_BH1750_ADDRESS_HIGH : LSENSE_BH1750_ADDRESS_LOW )

static Bh1750Sensor *xInstances[MAX_INSTANCES] = { nullptr };
static uint32_t ulInstanceCount = 0;

uint8_t Bh1750Sensor::setOpcode(uint8_t op)
{
    uint8_t ack;

    Wire.beginTransmission(xI2cAddress);
    Wire.write(op);
    ack = Wire.endTransmission();

    return ack;
}

uint16_t Bh1750Sensor::read(void)
{
    Adc16Data_t xReadData;
    Wire.requestFrom(xI2cAddress, sizeof(uint16_t));
    xReadData.xBytes.ucHi = Wire.read();
    xReadData.xBytes.ucLo = Wire.read();
    Wire.endTransmission();

    return xReadData.usData;
}

static float convert( uint16_t usData )
{
    return (float)usData / 1.2f;
}

Bh1750Sensor::Bh1750Sensor(bool ucAddr) : xLastMeasure(0)
{
    if ( ulInstanceCount == MAX_INSTANCES )
    {
        return;
    }

    xInstances[ulInstanceCount] = this;

    xI2cAddress = LSENSE_BH1750_ADDRESS(ucAddr);
    ucInstIdx = ulInstanceCount;

    setMode(CONT_HRES_MD1);
    setMeasureDelay(69);

    ulInstanceCount++;
}

uint8_t Bh1750Sensor::setPower( uint8_t ucState )
{
    ucPowerState = ucState;
    return setOpcode((ucState) ? POWER_ON : POWER_DOWN);
}

uint8_t Bh1750Sensor::reset( void )
{
    return setOpcode(RESET);
}

uint8_t Bh1750Sensor::setMode( uint8_t ucMode )
{
    ucMode = ucMode;
    return setOpcode(ucMode);
}

uint8_t Bh1750Sensor::setMeasureDelay( uint8_t ucMeasureDelay )
{
    uint8_t ack;

    ucMeasureDelay = ucMeasureDelay;
    ack = setOpcode(CHG_MTIME_HI(ucMeasureDelay));
    ack |= setOpcode(CHG_MTIME_LO(ucMeasureDelay));

    return ack;
}

int32_t Bh1750Sensor::measure( void )
{
    const TickType_t xCurrentTick = xTaskGetTickCount();

    if ( (xCurrentTick - xLastMeasure) >= ucMeasureDelay )
    {
        xLastMeasure = xCurrentTick;
        uint16_t usData = read();
        return (int32_t)convert(usData);
    }

    return -1;
}

uint32_t Bh1750Sensor::cmdsvr( uint8_t argc, char *argv[] )
{
    Bh1750Sensor *pxInst = (argc < 1) ? xInstances[0] : xInstances[atoi(argv[1])];
    uint16_t usData;

    do
    {
        usData = pxInst->measure();
    } while (usData == -1);
    Serial.print(usData);

    return 0;
}
