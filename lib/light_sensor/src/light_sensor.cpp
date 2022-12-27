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

/**
 * @brief
 *  Set BH1750 address based on address pin state
 *
 */
#define BH1750_ADDRESS(PIN) ( (PIN) ? 0x5C : 0x23 )

/** Array containing BH1750 instances used by command server */
static Bh1750Sensor *instances[MAX_INSTANCES] = { nullptr };

/** BH1750 instance count */
static uint32_t inst_cnt = 0;

/**
 * @brief
 *  Send a command to a BH1750 instance over I2C
 *
 * @param[in] op - Command code
 *
 * @return
 *  Acknowledge signal
 */
uint8_t Bh1750Sensor::setOpcode(uint8_t op)
{
    uint8_t ack;

    Wire.beginTransmission(i2c_addr);
    Wire.write(op);
    ack = Wire.endTransmission();

    return ack;
}

/**
 * @brief
 *  Read 2-bytes from a BH1750 instance
 *
 * @return
 *  Returned data
 */
uint16_t Bh1750Sensor::read(void)
{
    AdcData_16b read_data;
    Wire.requestFrom(i2c_addr, sizeof(uint16_t));
    read_data.bytes.upper = Wire.read();
    read_data.bytes.lower = Wire.read();
    //Wire.endTransmission();

    return read_data.data;
}

/**
 * @brief Construct a new BH1750Sensor object
 *
 * @param[in] address - Address pin state
 */
Bh1750Sensor::Bh1750Sensor(bool address) : last_measure(0), i2c_addr(BH1750_ADDRESS(address))
{
    inst_idx = -1;
    if (inst_cnt <= MAX_INSTANCES)
    {
        inst_idx = inst_cnt;
        instances[inst_cnt] = this;
        inst_cnt++;
    }

    setPower(POWER_ON);
    setMode(CONT_HRES_MD1);
    setMeasureDelay(69);
}

/**
 * @brief
 *  Set BH1750 power state
 *
 * @param[in] state - Power state, high is on
 *
 * @return
 *  Acknowledge signal
 */
uint8_t Bh1750Sensor::setPower(uint8_t state)
{
    power_state = state;
    return setOpcode(state);
}

/**
 * @brief
 *  Reset BH1750
 *
 * @return
 *  Acknowledge signal
 */
uint8_t Bh1750Sensor::reset(void)
{
    return setOpcode(RESET);
}

/**
 * @brief
 *  Set aquisition mode
 *
 * @param[in] mode - resolution mode
 *
 * @return
 *  Acknowledge signal
 */
uint8_t Bh1750Sensor::setMode(uint8_t mode)
{
    this->mode = mode;
    return setOpcode(mode);
}

/**
 * @brief
 *  Set measure delay
 *
 * @param[in] measure_delay - measure delay
 *
 * @return
 *  Acknowledge signal
 */
uint8_t Bh1750Sensor::setMeasureDelay(uint8_t measure_delay)
{
    uint8_t ack;

    this->measure_delay = measure_delay;
    ack = setOpcode(CHG_MTIME_HI(measure_delay));
    ack |= setOpcode(CHG_MTIME_LO(measure_delay));

    return ack;
}

/**
 * @brief
 *  Measure light sensitivity from a BH1750 instance
 *
 * @return
 *  Light sensitivity reading
 */
uint16_t Bh1750Sensor::measure(void)
{
    const TickType_t current_tick = xTaskGetTickCount();

    if ((current_tick - last_measure) >= measure_delay)
    {
        last_measure = current_tick;
        return read();
    }

    return -1;
}

/**
 * @brief
 *  BH1750 command server
 *
 * @param[in] argc - Argument count
 * @param[in] argv - Arguments
 *
 * @return
 *  Command status
 */
uint32_t Bh1750Sensor::cmdsvr(uint8_t argc, char *argv[])
{
    Bh1750Sensor *inst = (argc < 1) ? instances[0] : instances[atoi(argv[1])];
    uint16_t data;

    do
    {
        data = inst->measure();
    } while (data == -1);

    Serial.print("BH1750 instance ");
    Serial.print(inst->inst_idx);
    Serial.print(" reads ");
    Serial.println(data);

    return 0;
}
