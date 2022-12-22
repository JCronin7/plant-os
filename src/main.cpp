#include <Arduino.h>
#include <FreeRTOS_SAMD21.h>
#include <transport.h>
#include <interface.h>
#include <cmdsvr.h>
#include <config.h>
#include <app.h>

#define ERROR_LED_PIN 13
#define ERROR_LED_LIGHTUP_STATE HIGH
#define LED_RING_DATA_IN_PIN 11
#define PRINT_SEP "****************************************************"

static Transport command_pipe(16);

static char ptrTaskList[256];

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

    measurement = uxTaskGetStackHighWaterMark(*pxInterfaceTaskHandleGet());
    Serial.print("Interface thread: ");
    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(cmdsvr_task_hdl_get());
    Serial.print("Command Server Thread: ");
    Serial.println(measurement);

    //measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
    //Serial.print("Monitor Stack: ");
    //Serial.println(measurement);

    Serial.println("****************************************************");
    Serial.flush();

    return 0;
}

void setup()
{
    portBASE_TYPE status = pdPASS;

    Serial.begin(115200);
    flash_init();

    delay(1000);
    while (!Serial)
        ;

    vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);
    vSetErrorSerial(&Serial);

    /* Serial and Wi-Fi interface */
    status &= vInterfaceInit(INTERFACE_TASK_PRIORITY,
                              INTERFACE_TASK_STACK,
                              &command_pipe);
    /* Command server */
    status &= cmdsvr_init(CMDSVR_TASK_PRIORITY,
                           2*CMDSVR_TASK_STACK,
                           &command_pipe);

#if 0
    Cmdsvr::xCmdsvrRegisterCmd("wr",
                               "write to eeprom",
                               (CmdsvrCommandCb_t)ulHalEepromWriteCmd);
    Cmdsvr::xCmdsvrRegisterCmd("rd",
                               "read from eeprom",
                               (CmdsvrCommandCb_t)ulHalEepromReadCmd);
    Cmdsvr::xCmdsvrRegisterCmd("print",
                               "",
                               (CmdsvrCommandCb_t)ulHalEepromPrintCmd);
#endif
    /* Wifi sever */
    status &= webserver_init(80);
    cmdsvr_register_cmd("wifi",
                        "Send command to WiFi subsystem.",
                        (cmdsvr_cmd_cb_t)webserver_cmdsvr);
    status &= app_init(3, 2*CMDSVR_TASK_STACK);
    /* EK1940 Soil moisture sensor */
    cmdsvr_register_cmd("app",
                        "Control user application",
                        (cmdsvr_cmd_cb_t)app_cmdsvr);
    /* Built-in LED */
    status &= led_init(LED_RING_DATA_IN_PIN);
    cmdsvr_register_cmd("led",
                        "Toggle builtin and ring led using \'led on/off/blink \'period / 2\'\'",
                        (cmdsvr_cmd_cb_t)led_cmdsvr);
#if 0
    /* BH1750 Light intensity sensor */
    cmdsvr_register_cmd("light",
                        "Read sensor",
                        (cmdsvr_cmd_cb_t)light_sensor.cmdsvr);
#endif
    /* EK1940 Soil moisture sensor */
    cmdsvr_register_cmd("moist",
                        "Read sensor",
                        (cmdsvr_cmd_cb_t)moist_sensor.cmdsvr);
    /* Water Pump */
    cmdsvr_register_cmd("pump", "Set Water Pump", (cmdsvr_cmd_cb_t)pump.cmdsvr);
    cmdsvr_register_cmd("sys_info", "Print task information", (cmdsvr_cmd_cb_t)sys_info_cmd);

    //ulFlashPrintCmd(0, nullptr);
    vTaskStartScheduler();

    while (1)
    {
        Serial.println("Scheduler Failed! \n");
        delay(1000);
    }
}

void loop() { }