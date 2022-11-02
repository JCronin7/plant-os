#include <light_sensor.h>
#include <string.h>
#include <util.h>
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

#define LSENSE_BH1750_ADDRESS(PIN)  ((PIN) ? LSENSE_BH1750_ADDRESS_HIGH : LSENSE_BH1750_ADDRESS_LOW)

static Bh1750Sensor_t *xInstances[MAX_INSTANCES] = {nullptr};
static uint32_t ulInstanceCount = 0;

static uint8_t ucLightSensorSetOpcode(Bh1750Sensor_t *pxInst, uint8_t op)
{
    uint8_t ack;

    pxInst->xI2cDriver->beginTransmission(pxInst->xI2cAddress);
    pxInst->xI2cDriver->write(op);
    ack = pxInst->xI2cDriver->endTransmission();

    return ack;
}

static uint16_t usLightSensorRead(Bh1750Sensor_t *pxInst)
{
    Adc16Data_t xReadData;

    pxInst->xI2cDriver->requestFrom(pxInst->xI2cAddress,
                                    sizeof(uint16_t));
    xReadData.xBytes.ucHi = pxInst->xI2cDriver->read();
    xReadData.xBytes.ucLo = pxInst->xI2cDriver->read();

    return xReadData.usData;
}

static void vLightSensorMeasureTask( void *arg )
{
    const uint8_t ucSensorIndex = *(uint32_t*)arg;
    Bh1750Sensor_t * const pxInst = xInstances[ucSensorIndex];
    TickType_t xPreviousWake = 0;
    uint16_t usData;

    while (true)
    {
        vTaskDelayUntil(&xPreviousWake, LSENSE_BH1750_LOOP_RATE);
        usData = ulLightSensorMeasure(pxInst);
        uint8_t bytes = xStreamBufferSend(pxInst->xAdcDataBufferHdl,
                                          (uint8_t *)&usData,
                                          sizeof(uint16_t),
                                          0);
        if (bytes != sizeof(uint8_t))
        {
            // error
            (void)bytes;
        }
    }
}

static float xLightSensorConvert(uint16_t usData)
{
    return (float)usData / 1.2f;
}

BaseType_t xLightSensorinit(Bh1750Sensor_t *pxInst, I2CDriverWire *pxWire, bool ucAddr)
{
    BaseType_t xStatus = pdFAIL;
    const uint32_t ulStreamBufferSize = sizeof(uint16_t) * ADC_DATA_BUFFER_SIZE;

    if (ulInstanceCount == MAX_INSTANCES)
    {
        return xStatus;
    }

    xInstances[ulInstanceCount] = pxInst;

    pxInst->xI2cAddress = LSENSE_BH1750_ADDRESS(ucAddr);
    pxInst->xI2cDriver = pxWire;
    pxInst->ucInstIdx = ulInstanceCount;

    pxInst->xAdcDataBufferHdl = xStreamBufferCreate(ulStreamBufferSize,
                                                    (size_t)1);

    pxInst->xI2cDriver->begin();

    ucLightSensorSetMode(pxInst, CONT_HRES_MD1);
    ucLightSensorSetMeasureDelay(pxInst, 69);


    xStatus = xTaskCreate(vLightSensorMeasureTask,
                          NULL,
                          100 * configMINIMAL_STACK_SIZE,
                          &(pxInst->ucInstIdx),
                          2,
                          NULL);

    ulInstanceCount++;

    return xStatus;
}

uint8_t ucLightSensorSetPower(Bh1750Sensor_t *pxInst, uint8_t ucState)
{
    pxInst->ucPowerState = ucState;
    return ucLightSensorSetOpcode(pxInst,
                                 (ucState) ? POWER_ON : POWER_DOWN);
}

uint8_t ucLightSensorReset(Bh1750Sensor_t *pxInst)
{
    return ucLightSensorSetOpcode(pxInst, RESET);
}

uint8_t ucLightSensorSetMode(Bh1750Sensor_t *pxInst, uint8_t ucMode)
{
    pxInst->ucMode = ucMode;
    return ucLightSensorSetOpcode(pxInst, ucMode);
}

uint8_t ucLightSensorSetMeasureDelay(Bh1750Sensor_t *pxInst, uint8_t ucMeasureDelay)
{
    uint8_t ack;

    pxInst->ucMeasureDelay = ucMeasureDelay;
    ack = ucLightSensorSetOpcode(pxInst, CHG_MTIME_HI(ucMeasureDelay));
    ack |= ucLightSensorSetOpcode(pxInst, CHG_MTIME_LO(ucMeasureDelay));

    return ack;
}

int32_t ulLightSensorMeasure(Bh1750Sensor_t *pxInst)
{
    const TickType_t xCurrentTick = xTaskGetTickCount();

    if (xCurrentTick - pxInst->xLastMeasure >= pxInst->ucMeasureDelay)
    {
        pxInst->xLastMeasure = xCurrentTick;
        return usLightSensorRead(pxInst);
    }

    return -1;
}

uint32_t ulLightSensorCmdsvr(uint8_t argc, char *argv[])
{
    char cBuffer[sizeof(uint16_t) * ADC_DATA_BUFFER_SIZE];
    uint16_t usData;
    Bh1750Sensor_t *pxInst;

    if (argc < 1)
    {
        pxInst = xInstances[0];
        do
        {
            usData = ulLightSensorMeasure(pxInst);
        } while (usData < 0);
        sprintf(cBuffer, "%d       \n", usData);
        xHalUartUsbSerialPrint(cBuffer);
        return 0;
    }

#if 0
    if (iscommand(argv[0], "stream", sizeof("stream") - 1))
    {
        size_t xBytes = xStreamBufferReceive(pxInst->xAdcDataBufferHdl,
                                             cBuffer,
                                             sizeof(uint16_t) * ADC_DATA_BUFFER_SIZE,
                                             0);
        xHalUartUsbSerialWrite((const uint8_t * const)cBuffer,
                               sizeof(uint16_t) * ADC_DATA_BUFFER_SIZE);
    }
#endif

    for (uint8_t i = 0; i < argc; i++)
    {
        pxInst = xInstances[atoi(argv[i])];
        do
        {
            usData = ulLightSensorMeasure(pxInst);
        } while (usData < 0);
        sprintf(cBuffer, "%d       \n", usData);
        xHalUartUsbSerialPrint(cBuffer);
    }

    return 0;
}
