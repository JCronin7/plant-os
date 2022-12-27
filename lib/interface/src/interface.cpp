#include <interface.h>
#include <hal.h>

static TaskHandle_t xInterfaceTaskHdl;

static void task_entry(void *arg)
{
    static TickType_t xPreviousWake = 0;

    while (true)
    {
        Interface::UsbSerial::spin();
        Interface::Webserver::spin();
        vTaskDelayUntil(&xPreviousWake, 10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief
 *
 */
void Interface::interface_read(uint32_t offset,
                               uint32_t length,
                               void *buffer)
{
    if ((offset + length) < MEM_REG_LENGTH_ITRFCE)
    {
        flash_read(MEM_REG_OFFSET_ITRFCE + offset, length, buffer);
    }
}

/**
 * @brief
 *
 */
void Interface::interface_write(uint32_t offset,
                                uint32_t length,
                                void *buffer)
{
    if ((offset + length) < MEM_REG_LENGTH_ITRFCE)
    {
        flash_write(MEM_REG_OFFSET_ITRFCE + offset, length, buffer);
    }
}

TaskHandle_t Interface::task_hdl_get(void)
{
    return xInterfaceTaskHdl;
}

BaseType_t Interface::initialize(UBaseType_t uxPriority,
                                 uint16_t usStackDepth,
                                 Msg::Pipe *pipe)
{
    BaseType_t status;

    UsbSerial::initialize(pipe);
    status = xTaskCreate(task_entry,
                         "INTERFACE",
                         usStackDepth,
                         NULL,
                         uxPriority,
                         &xInterfaceTaskHdl);
    return status;
}
