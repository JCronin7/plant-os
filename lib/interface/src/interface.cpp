#include <interface.h>
#include <hal.h>

static TickType_t xPreviousWake = 0;
static TaskHandle_t xInterfaceTaskHdl;

static char xSerialBuffer[USB_SERIAL_LINE_SIZE];

static void vInterfaceTask(void *arg)
{
    while (true)
    {
        if (usb_serial_getline(xSerialBuffer))
        {
        }
        webserver_background_task();

        vTaskDelayUntil(&xPreviousWake, 10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief
 *
 */
void interface_read(uint32_t offset,
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
void interface_write(uint32_t offset,
                     uint32_t length,
                     void *buffer)
{
    if ((offset + length) < MEM_REG_LENGTH_ITRFCE)
    {
        flash_write(MEM_REG_OFFSET_ITRFCE + offset, length, buffer);
    }
}

TaskHandle_t *pxInterfaceTaskHandleGet(void)
{
    return (TaskHandle_t *)&xInterfaceTaskHdl;
}

BaseType_t vInterfaceInit(UBaseType_t uxPriority,
                          uint16_t usStackDepth,
                          Transport *pipe)
{
    BaseType_t xStatus;

    usb_serial_init(pipe);
    xStatus = xTaskCreate(vInterfaceTask,
                          "INTERFACE",
                          usStackDepth,
                          NULL,
                          uxPriority,
                          &xInterfaceTaskHdl);

    return xStatus;
}