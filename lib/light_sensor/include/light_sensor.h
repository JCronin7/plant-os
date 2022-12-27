#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <Wire.h>

#define MAX_INSTANCES 1

/**
 * @brief
 *  16-bit ADC data conversion
 *
 */
union AdcData_16b
{
    /** Raw 16-bit data */
    uint16_t data;

    /** Data bytes*/
    struct
    {
        uint8_t lower;
        uint8_t upper;
    } bytes;
};

/**
 * @brief
 *  BH1750 light sensitivity sensor
 *
 */
class Bh1750Sensor
{
private:
    /** Tick of last measurement */
    TickType_t last_measure;

    /** I2C address */
    uint8_t i2c_addr;

    /** Current power state */
    uint8_t power_state;

    /** Current aquisition mode */
    uint8_t mode;

    /** Current measure delay setting */
    uint8_t measure_delay;

    /** Instance index*/
    uint8_t inst_idx;

    /**
     * @brief
     *  Send a command to a BH1750 instance over I2C
     *
     * @param[in] op - Command code
     *
     * @return
     *  Acknowledge signal
     */
    uint8_t setOpcode(uint8_t op);

    /**
     * @brief
     *  Read 2-bytes from a BH1750 instance
     *
     * @return
     *  Returned data
     */
    uint16_t read(void);

public:

    /**
     * @brief Construct a new BH1750Sensor object
     *
     * @param[in] address - Address pin state
     */
    Bh1750Sensor(bool address);

    /**
     * @brief
     *  Set BH1750 power state
     *
     * @param[in] state - Power state, high is on
     *
     * @return
     *  Acknowledge signal
     */
    uint8_t setPower(uint8_t state);

    /**
     * @brief
     *  Reset BH1750
     *
     * @return
     *  Acknowledge signal
     */
    uint8_t reset(void);

    /**
     * @brief
     *  Set aquisition mode
     *
     * @param[in] mode - resolution mode
     *
     * @return
     *  Acknowledge signal
     */
    uint8_t setMode(uint8_t mode);

    /**
     * @brief
     *  Set measure delay
     *
     * @param[in] measure_delay - measure delay
     *
     * @return
     *  Acknowledge signal
     */
    uint8_t setMeasureDelay(uint8_t measure_delay);

    /**
     * @brief
     *  Measure light sensitivity from a BH1750 instance
     *
     * @return
     *  Light sensitivity reading
     */
    uint16_t measure(void);

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
    static uint32_t cmdsvr(uint8_t argc, char *argv[]);
};

#endif /* _LIGHT_SENSOR_H_ */
