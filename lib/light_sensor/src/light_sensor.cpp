#include <light_sensor.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

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

using namespace light_sensor;

union adc16b_data
{
    uint16_t data;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    } bytes;
};

uint8_t bh1750::instance_count = 0;

bh1750* bh1750::instances[MAX_INSTANCES] = {nullptr};

uint8_t bh1750::set_opcode(uint8_t op)
{
    uint8_t ack;

    i2c_driver->beginTransmission(this->i2c_address);
    i2c_driver->write(op);
    ack = i2c_driver->endTransmission();

    return ack;
}

uint16_t bh1750::read(void)
{
    adc16b_data data;

    i2c_driver->requestFrom(this->i2c_address, sizeof(uint16_t));
    data.bytes.hi = i2c_driver->read();
    data.bytes.lo = i2c_driver->read();

    return data.data;
}

bh1750::bh1750(void)
{
    i2c_driver = nullptr;
    i2c_address = 0xFF;
    last_measure = 0;
}

BaseType_t bh1750::init(bool address, I2CDriverWire &wire)
{
    BaseType_t status = pdFAIL;

    if (bh1750::instance_count == MAX_INSTANCES)
    {
        return status;
    }

    bh1750::instances[bh1750::instance_count++] = this;

    this->i2c_address = LSENSE_BH1750_ADDRESS(address);
    this->i2c_driver = &wire;

    this->adc_buffer = xStreamBufferCreate(sizeof(uint16_t) * ADC_DATA_BUFFER_SIZE,
                                           (size_t)1);

    i2c_driver->begin();

    set_mode(CONT_HRES_MD1);
    set_measure_delay(69);

    inst_idx = bh1750::instance_count - 1;

    status = xTaskCreate(measure_task,
                         NULL,
                         100 * configMINIMAL_STACK_SIZE,
                         &(this->inst_idx),
                         2,
                         NULL);
    return status;
}

uint8_t bh1750::set_power(uint8_t state)
{
    this->power_state = state;
    return set_opcode((state) ? POWER_ON : POWER_DOWN);
}

uint8_t bh1750::reset(void)
{
    return set_opcode(RESET);
}

uint8_t bh1750::set_mode(uint8_t mode)
{
    this->mode = mode;
    return set_opcode(CONT_HRES_MD1);
}

uint8_t bh1750::set_measure_delay(uint8_t measure_delay)
{
    uint8_t ack;

    this->measure_delay = measure_delay;
    ack = set_opcode(CHG_MTIME_HI(measure_delay));
    ack |= set_opcode(CHG_MTIME_LO(measure_delay));

    return ack;
}

uint32_t bh1750::measure(void)
{
    const TickType_t current_tick = xTaskGetTickCount();

    if (current_tick - last_measure >= measure_delay)
    {
        last_measure = current_tick;
        return bh1750::read();
    }

    return 0.0f;
}

float bh1750::convert(uint16_t raw)
{
    return (float)raw / 1.2f;
}

void bh1750::measure_task(void *arg)
{
    const uint8_t sensor_idx = *(uint16_t*)arg;
    TickType_t previous_wake = 0;
    bh1750  *inst_ptr = bh1750::instances[sensor_idx];
    uint16_t data_raw;

    while (true)
    {
        vTaskDelayUntil(&previous_wake, 120);
        data_raw = inst_ptr->measure();
        uint8_t bytes = xStreamBufferSend(inst_ptr->adc_buffer,
                                          (uint8_t *)&data_raw,
                                          sizeof(uint16_t),
                                          0);
        if (bytes != sizeof(uint8_t))
        {
            // error
            (void)bytes;
        }
    }
}

uint32_t bh1750::cmd(uint8_t argc, char *argv[])
{
    uint16_t data_raw;
    bh1750 *inst_ptr;

    if (argc < 1)
    {
        inst_ptr = bh1750::instances[0];
    }

    for (uint8_t i = 0; i < argc; i++)
    {
        inst_ptr = bh1750::instances[atoi(argv[i])];
    }

    size_t bytes = xStreamBufferReceive(inst_ptr->adc_buffer,
                                        (uint8_t *)&data_raw,
                                        sizeof(uint16_t),
                                        0);
    return (bytes == sizeof(uint16_t)) ? data_raw : 0;
}
