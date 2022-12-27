#include <Arduino.h>
#include <FreeRTOS_SAMD21.h>
#include <msg.h>
#include <interface.h>
#include <cmdsvr.h>
#include <config.h>
#include <webpage.h>
#include <Wire.h>
#include <app.h>

#define ERROR_LED_PIN           13
#define ERROR_LED_LIGHTUP_STATE HIGH
#define LED_RING_DATA_IN_PIN    11
#define PRINT_SEP               "****************************************************"

static Msg::Pipe command_pipe(4);

static char ptrTaskList[256];

uint32_t fw_ver_cmd(uint8_t argc, char *argv[])
{
    Serial.print(FW_VERSION_MAJOR);
    Serial.print('.');
    Serial.println(FW_VERSION_MINOR);
    return 0;
}

uint32_t sys_info_cmd(uint8_t argc, char *argv[])
{
    int measurement;

    Serial.flush();
    Serial.print('\n');
    Serial.println(PRINT_SEP);

    Serial.print("Free Heap: ");
    Serial.print(xPortGetFreeHeapSize());
    Serial.println(" bytes");

    Serial.print("Min Heap: ");
    Serial.print(xPortGetMinimumEverFreeHeapSize());
    Serial.println(" bytes");
    Serial.flush();

    Serial.println(PRINT_SEP);
    Serial.println("Task            ABS             %Util");
    Serial.println(PRINT_SEP);

    vTaskGetRunTimeStats(ptrTaskList);
    Serial.println(ptrTaskList);
    Serial.flush();

    Serial.println(PRINT_SEP);
    Serial.println("Task            State   Prio    Stack   Num     Core");
    Serial.println(PRINT_SEP);

    vTaskList(ptrTaskList);
    Serial.println(ptrTaskList);
    Serial.flush();

    Serial.println(PRINT_SEP);
    Serial.println("[Stacks Free Bytes Remaining] ");

    measurement = uxTaskGetStackHighWaterMark(Interface::task_hdl_get());
    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Cmdsvr::task_hdl_get());
    Serial.println(measurement);

    //measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
    //Serial.print("Monitor Stack: ");
    //Serial.println(measurement);

    Serial.println(PRINT_SEP);
    Serial.flush();

    return 0;
}

void setup()
{
    portBASE_TYPE status = pdPASS;

    Serial.begin(115200);
    Wire.begin();
    flash_init();

    delay(1000);
    while (!Serial)
        ;

    vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);
    vSetErrorSerial(&Serial);

    /* Command server */
    status &= Cmdsvr::initialize(CMDSVR_TASK_PRIORITY,
                           CMDSVR_TASK_STACK,
                           &command_pipe);
    /* Serial and Wi-Fi interface */
    status &= Interface::initialize(INTERFACE_TASK_PRIORITY,
                                    INTERFACE_TASK_STACK,
                                    &command_pipe);
    Interface::Webserver::initialize(80,
                                     pagedata,
                                     pagesize,
                                     &command_pipe);
    status &= app_init(APP_TASK_PRIORITY, APP_TASK_STACK);
/// @todo Need to get flash/eeprom working
#if 0
    Cmdsvr::register_cmd("wr",
                         "write to eeprom",
                         (CmdsvrCommandCb_t)ulHalEepromWriteCmd);
    Cmdsvr::register_cmd("rd",
                         "read from eeprom",
                         (CmdsvrCommandCb_t)ulHalEepromReadCmd);
    Cmdsvr::register_cmd("print",
                         "",
                         (CmdsvrCommandCb_t)ulHalEepromPrintCmd);
#endif
    /* Wifi sever */
   Cmdsvr::register_cmd("wifi",
                        "Send command to WiFi subsystem.",
                        (Cmdsvr::Command_cb)Interface::Webserver::cmdsvr,
                        'W');
    /* EK1940 Soil moisture sensor */
    Cmdsvr::register_cmd("app",
                         "Control user application",
                         (Cmdsvr::Command_cb)app_cmdsvr,
                         'A');
    /* Built-in LED */
    status &= led_init(LED_RING_DATA_IN_PIN);
    Cmdsvr::register_cmd("led",
                         "Toggle builtin and ring led",
                         (Cmdsvr::Command_cb)led_cmdsvr,
                         'L');
    /* BH1750 Light intensity sensor */
    Cmdsvr::register_cmd("light",
                        "Read sensor",
                        (Cmdsvr::Command_cb)Bh1750Sensor::cmdsvr,
                        'G');
    /* EK1940 Soil moisture sensor */
    Cmdsvr::register_cmd("moist",
                        "Read sensor",
                        (Cmdsvr::Command_cb)Ek1940Sensor::cmdsvr,
                        'M');
    /* Water Pump */
    Cmdsvr::register_cmd("pump",
                         "Set Water Pump",
                         (Cmdsvr::Command_cb)Pump::cmdsvr,
                         'P');
    Cmdsvr::register_cmd("sys_info",
                         "Print task information",
                         (Cmdsvr::Command_cb)sys_info_cmd,
                         'S');
    Cmdsvr::register_cmd("fw_ver",
                         "Print firmware version",
                         (Cmdsvr::Command_cb)fw_ver_cmd,
                         'F');

    while (status == pdFAIL)
    {
        Serial.println("Creation problem.\n");
        delay(1000);
    }

    //ulFlashPrintCmd(0, nullptr);
    vTaskStartScheduler();

    while (1)
    {
        Serial.println("Scheduler Failed! \n");
        delay(1000);
    }
}

void loop() { }