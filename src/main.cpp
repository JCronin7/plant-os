#include <FreeRTOS_TEENSY4.h>
#include <cmdsvr.h>
#include <led.h>
#include <lsense.h>

lsense::bh1750 light_sensor;

void setup()
{
    portBASE_TYPE status = pdPASS;

    Serial.begin(115200);

    status &= led::init(11);
    status &= cmdsvr::init(&Serial, 115200);
    status &= light_sensor.init(LOW, Wire);

    cmdsvr::register_command("led",
                             "Toggle builtin led using \'led on/off/blink \'period / 2\'\'",
                             led::builtin_cmd);
    cmdsvr::register_command("led_ring",
                             "Toggle led ring using \'led_ring on/off\'",
                             led::ring_cmd);
    cmdsvr::register_command("lsense",
                             "Read sensor",
                             lsense::bh1750::cmd);

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