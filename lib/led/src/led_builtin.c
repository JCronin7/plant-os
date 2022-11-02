#include <led_builtin.h>
#include <led_loc.h>
#include <util.h>

#define LED_BUILTIN_LOOP_RATE   ( 100ul / portTICK_PERIOD_MS )
#define vLedBuiltinSetLed(VAL)            \
    do                                    \
    {                                     \
        vPortEnterCritical();             \
        digitalWrite(LED_BUILTIN, (VAL)); \
        vPortExitCritical();              \
    } while (0);

static uint32_t ulLedBuiltinBlinkFreq = 0;
static TaskHandle_t xLedBuiltinBlinkTaskHdl = NULL;

static void vLedBuiltinBlinkTask(void *arg)
{
    TickType_t xPreviousWake = 0;
    static uint32_t ulCount = 0;
    static bool ucState = false;

    while (true)
    {
        if (ulLedBuiltinBlinkFreq == 0)
        {
            vTaskSuspend(xLedBuiltinBlinkTaskHdl);
        }

        if (ulCount++ >= ulLedBuiltinBlinkFreq)
        {
            ulCount = 0;
            vLedBuiltinSetLed(ucState);
            ucState = ucState ? false : true;
        }

        vTaskDelayUntil(&xPreviousWake, LED_BUILTIN_LOOP_RATE);
    }
}

BaseType_t xLedBuiltinInit(void)
{
    BaseType_t xStatus;

    pinMode(LED_BUILTIN, OUTPUT);
    vLedBuiltinSetLed(LOW);

    ulLedBuiltinBlinkFreq = 0;
    xStatus = xTaskCreate(vLedBuiltinBlinkTask,
                         NULL,
                         100 * configMINIMAL_STACK_SIZE,
                         NULL,
                         2,
                         &xLedBuiltinBlinkTaskHdl);

    return xStatus;
}

void vLedBuiltinSet(bool ucState)
{
    vTaskSuspend(xLedBuiltinBlinkTaskHdl);
    vLedBuiltinSetLed(ucState);
}

void vLedBuiltinBlink(uint32_t ulFrequency)
{
    ulLedBuiltinBlinkFreq = ulFrequency / 100;
    vTaskResume(xLedBuiltinBlinkTaskHdl);
}

uint32_t ulLedBuiltinCmdsvr(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if ( iscommand(argv[0], "on", sizeof("on") - 1) )
    {
        vLedBuiltinSet(HIGH);
    }
    else if ( iscommand(argv[0], "off", sizeof("off") - 1) )
    {
        vLedBuiltinSet(LOW);
    }
    else if ( iscommand(argv[0], "blink", sizeof("blink") - 1) && argc == 2 )
    {
        vLedBuiltinBlink((uint32_t)atoi(argv[1]));
    }
    else
    {
        return LED_STATUS_UNKNOWN_CMD;
    }

    return LED_STATUS_SUCCESS;
}
