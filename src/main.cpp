#include <FreeRTOS_TEENSY4.h>
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

    status &= xLedBuiltinInit();
    status &= xCmdsvrInit();
    status &= vLedRingInit(LED_RING_DATA_IN_PIN);
    status &= xLightSensorinit(&xLightSensor, &Wire, LOW);
    status &= ulMoistureSensorInit(&xMoistSensor, 23);

    xCmdsvrRegisterCmd("led",
                       "Toggle builtin led using \'led on/off/blink \'period / 2\'\'",
                       (CmdsvrCommandCb_t)ulLedBuiltinCmdsvr);
    xCmdsvrRegisterCmd("led_ring",
                       "Toggle led ring using \'led_ring on/off\'",
                       (CmdsvrCommandCb_t)ulLedRingCmdsvr);
    xCmdsvrRegisterCmd("light",
                       "Read sensor",
                       (CmdsvrCommandCb_t)ulLightSensorCmdsvr);
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