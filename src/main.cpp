#include <FreeRTOS_TEENSY4.h>
#include <cmdsvr.h>
#include <led.h>
#include <light_sensor.h>
#include <moist_sensor.h>

light_sensor::bh1750 bh1750_senor;

void setup()
{
    portBASE_TYPE status = pdPASS;

    Serial.begin(115200);

    status &= led::init(11);
    status &= cmdsvr::init(&Serial, 115200);
    status &= bh1750_senor.init(LOW, Wire);
    status &= moist_sensor::ek1940::init();

    cmdsvr::register_command("led",
                             "Toggle builtin led using \'led on/off/blink \'period / 2\'\'",
                             led::builtin_cmd);
    cmdsvr::register_command("led_ring",
                             "Toggle led ring using \'led_ring on/off\'",
                             led::ring_cmd);
    cmdsvr::register_command("light",
                             "Read sensor",
                             light_sensor::bh1750::cmd);
    cmdsvr::register_command("moist",
                             "Read sensor",
                             moist_sensor::ek1940::cmd);

    if (status != pdPASS)
    {
        Serial.println("Creation problem");
        while(1);
    }

    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
    while(1);
}

void loop(void) { }