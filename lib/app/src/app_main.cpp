#include <app.h>

#define delay_ms(millis) (vTaskDelayUntil(&previous_wake, (millis) / portTICK_PERIOD_MS))

void app_main(void *args)
{
    static TickType_t previous_wake = 0;

    while (true)
    {
        Serial.println("Hello World!");

        delay_ms(1000);
    }
}
