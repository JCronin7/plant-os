#include <app.h>

#define delay_ms(millis) (vTaskDelayUntil(&previous_wake, (millis) / portTICK_PERIOD_MS))

//Bh1750Sensor light_sensor(LOW);
Ek1940Sensor moist_sensor(PIN_A1);
Pump pump(1);

static TickType_t previous_wake = 0;

void app_main(void *args)
{
    while (true)
    {
        Serial.println("Hello World!");

        delay_ms(1000);
    }
}
