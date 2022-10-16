#include <FreeRTOS_TEENSY4.h>
#include <Adafruit_NeoPixel.h>
#include <cmdsvr.h>
#include <led.h>

Adafruit_NeoPixel led_ring(24, 9, NEO_GRB + NEO_KHZ800);

void setup()
{
    portBASE_TYPE status;

    Serial.begin(115200);
    led_ring.begin();
    led_ring.setBrightness(10);

    led::builtin_init();

    cmdsvr::init(&Serial, 115200);

    cmdsvr::register_command("led",
                             "Toggle builtin led using \'led <on/off>\'",
                             led::builtin_cmd);

    // while (true)
    // {
    //     led_ring.clear();
    //     for (int i = 0; i < 24; i++)
    //     {
    //         led_ring.setPixelColor(i, led_ring.Color(0xFF, 0x50, 0x5B));
    //         led_ring.show();
    //         delay(50);
    //     }
    // }

    status = xTaskCreate(cmdsvr::background_thread,
                         NULL,
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         3,
                         NULL);

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