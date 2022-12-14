#include <app.h>

Bh1750Sensor *light_sensor;
Ek1940Sensor *moist_sensor;
Pump *pump;
static TaskHandle_t app_task_hdl;

TaskHandle_t *app_task_hdl_get(void)
{
    return (TaskHandle_t *)&app_task_hdl;
}

uint32_t app_cmdsvr(uint8_t argc, char *argv[])
{
    if (argc < 1)
    {
        return 1;
    }

    if (strcmp(argv[1], "start") == 0)
    {
        vTaskResume(app_task_hdl);
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        vTaskSuspend(app_task_hdl);
    }

    return 0;
}

BaseType_t app_init(UBaseType_t task_priority,
                    uint16_t task_stack_depth)
{
    BaseType_t status = pdPASS;
    status = xTaskCreate(app_main,
                         "APP",
                         task_stack_depth,
                         NULL,
                         task_priority,
                         &app_task_hdl);
    vTaskSuspend(app_task_hdl);

    light_sensor = new Bh1750Sensor(LOW);
    moist_sensor = new Ek1940Sensor(PIN_A1);
    pump = new Pump(1);

    return status;
}