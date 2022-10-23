#include <lsense.h>
#include <string.h>

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


#define LSENSE_BH1750_ADDRESS_LOW  0x23
#define LSENSE_BH1750_ADDRESS_HIGH 0x5C

#define LSENSE_BH1750_ADDRESS(PIN)          ((PIN) ? LSENSE_BH1750_ADDRESS_HIGH : LSENSE_BH1750_ADDRESS_LOW)
#define IS_COMMAND(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

union adc16b_data
{
    uint16_t data;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    } bytes;
};

uint8_t lsense::bh1750::instance_count = 0;

lsense::bh1750* lsense::bh1750::instances[MAX_INSTANCES] = {nullptr};

uint8_t lsense::bh1750::set_opcode(uint8_t op)
{
    uint8_t ack;

    i2c_driver->beginTransmission(this->i2c_address);
    i2c_driver->write(op);
    ack = i2c_driver->endTransmission();

    return ack;
}

uint16_t lsense::bh1750::read(void)
{
    adc16b_data data;

    i2c_driver->requestFrom(this->i2c_address, sizeof(uint16_t));
    data.bytes.hi = i2c_driver->read();
    data.bytes.lo = i2c_driver->read();

    return data.data;
}

lsense::bh1750::bh1750(void)
{
    i2c_driver = nullptr;
    i2c_address = 0xFF;
}

BaseType_t lsense::bh1750::init(bool address, I2CDriverWire &wire)
{
    BaseType_t status = pdFAIL;

    if (lsense::bh1750::instance_count == MAX_INSTANCES)
    {
        return status;
    }

    lsense::bh1750::instances[lsense::bh1750::instance_count++] = this;

    this->i2c_address = LSENSE_BH1750_ADDRESS(address);
    this->i2c_driver = &wire;

    i2c_driver->begin();

    set_opcode(CONT_HRES_MD1);
    set_opcode(CHG_MTIME_HI(69));
    set_opcode(CHG_MTIME_LO(69));

    status = xTaskCreate(measure_task,
                         NULL,
                         100 * configMINIMAL_STACK_SIZE,
                         NULL,
                         2,
                         NULL);
    return status;
}

float lsense::bh1750::measure(void)
{
    return (float)lsense::bh1750::read() / 1.2f;
}

void lsense::bh1750::measure_task(void *arg)
{
    TickType_t previous_wake = 0;
    uint8_t sensor_idx = 0;//*(uint8_t*)arg;
    lsense::bh1750  *inst_ptr = lsense::bh1750::instances[sensor_idx];

    while (true)
    {
        vTaskDelayUntil(&previous_wake, 120);
        inst_ptr->adc_data_buffer[0] = inst_ptr->read();
    }
}

uint32_t lsense::bh1750::cmd(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return lsense::bh1750::instances[0]->measure();
    }

    for (uint8_t i = 0; i < argc; i++)
    {
        uint8_t idx = atoi(argv[i]);
        lsense::bh1750 *inst_ptr = lsense::bh1750::instances[idx];
        return inst_ptr->measure();
    }

    return 0;
}
