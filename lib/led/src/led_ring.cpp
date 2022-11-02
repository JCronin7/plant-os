#include <led_ring.h>
#include <string.h>
#include <util.h>
#include <led_loc.h>

#define LED_RING_PIXEL_COUNT    ( 24U )
#define LED_RING_LOOP_RATE      ( 100ul / portTICK_PERIOD_MS )

enum LedRingState
{
    OFF = 0,
    ON = 1,
};

static Adafruit_NeoPixel xLedRing;
static uint32_t ulLedRingColor = 0;
static LedRingState xState = OFF;
static TaskHandle_t xLedBuiltinBlinkTaskHdl = NULL;

static void vLedRingBackgroundTask( void *arg )
{
    TickType_t xPreviousWake = 0;

    while (true)
    {
        if ( xState == ON)
        {
            vLedRingSet((bool)ON);
            vTaskSuspend(xLedBuiltinBlinkTaskHdl);
        }
        else if ( xState == OFF )
        {
            vLedRingSet((bool)OFF);
            vTaskSuspend(xLedBuiltinBlinkTaskHdl);
        }

        vTaskDelayUntil(&xPreviousWake, LED_RING_LOOP_RATE);
    }
}

BaseType_t vLedRingInit(uint8_t ucRingDataPin)
{
    BaseType_t xStatus;
    xLedRing = Adafruit_NeoPixel(LED_RING_PIXEL_COUNT,
                                 ucRingDataPin,
                                 NEO_GRB + NEO_KHZ800);
    xLedRing.begin();
    xLedRing.setBrightness(10);
    xStatus = xTaskCreate(vLedRingBackgroundTask,
                          NULL,
                          100 * configMINIMAL_STACK_SIZE,
                          NULL,
                          2,
                          &xLedBuiltinBlinkTaskHdl);
    return xStatus;
}

void vLedRingSetColor( uint8_t ucRed,
                       uint8_t ucGreen,
                       uint8_t ucBlue )
{
    ulLedRingColor = xLedRing.Color(ucRed,
                                    ucGreen,
                                    ucBlue);
}

void vLedRingSet( bool ucState )
{
    ( ucState ) ? xLedRing.fill(ulLedRingColor) : xLedRing.clear();
    xLedRing.show();
}

uint32_t ulLedRingCmdsvr( uint8_t argc, char *argv[] )
{
    if (argc < 1)
    {
        return 1;
    }

    if (iscommand(argv[0], "on", sizeof("on") - 1) && argc == 4)
    {
        vLedRingSetColor(atoi(argv[1]),
                         atoi(argv[2]),
                         atoi(argv[3]));
        xState = ON;
        vTaskResume(xLedBuiltinBlinkTaskHdl);
    }
    else if (iscommand(argv[0], "off", sizeof("off") - 1))
    {
        xState = OFF;
        vTaskResume(xLedBuiltinBlinkTaskHdl);
    }

    return LED_STATUS_SUCCESS;
}
