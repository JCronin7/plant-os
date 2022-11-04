#if defined ( teensy40 )
#include <FreeRTOS_TEENSY4.h>
#elif defined ( mkrwifi1010 )
#include <FreeRTOS_SAMD21.h>
#include <Arduino.h>
#include <Wire.h>
#else
#error "Unsupported hardware platform"
#endif
#include <light_sensor.h>
#include <moist_sensor.h>
#include <led_ring.h>
#include <cmdsvr.h>
#include <hal.h>
#include <util.h>
#include <config.h>
#include <led_builtin.h>

Bh1750Sensor_t xLightSensor;
Ek1940Sensor_t xMoistSensor;

void setup()
{
    portBASE_TYPE status = pdPASS;

    vHalInit(115200);

    /* Command server */
    status &= xCmdsvrInit();

    /* Built-in LED */
    status &= xLedBuiltinInit();
    xCmdsvrRegisterCmd("led",
                       "Toggle builtin led using \'led on/off/blink \'period / 2\'\'",
                       (CmdsvrCommandCb_t)ulLedBuiltinCmdsvr);

    /* NeoPixel 24-pixel LED ring */
    status &= vLedRingInit(LED_RING_DATA_IN_PIN);
    xCmdsvrRegisterCmd("led_ring",
                       "Toggle led ring using \'led_ring on/off\'",
                       (CmdsvrCommandCb_t)ulLedRingCmdsvr);

    /* BH1750 Light intensity sensor */
    status &= xLightSensorinit(&xLightSensor, /* &Wire, */ LOW);
    xCmdsvrRegisterCmd("light",
                       "Read sensor",
                       (CmdsvrCommandCb_t)ulLightSensorCmdsvr);

    /* EK1940 Soil moisture sensor */
    status &= ulMoistureSensorInit(&xMoistSensor, 23);
    xCmdsvrRegisterCmd("moist",
                       "Read sensor",
                       (CmdsvrCommandCb_t)ulMoistureSensorCmdsvr);

    if (status != pdPASS)
    {
        Serial.println("Creation problem");
        while (1);
    }

    vTaskStartScheduler();

    Serial.println("Insufficient RAM");
    while (1);
}

void loop(void) { }