#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdint.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <FreeRTOS_SAMD21.h>
#include <transport.h>
#include <interface_config.h>

#define HISTORY_BUFFER_START    (USB_SERIAL_SAVED_CMDS * USB_SERIAL_LINE_SIZE)
#define HISTORY_BUFFER_END      ((USB_SERIAL_SAVED_CMDS * USB_SERIAL_LINE_SIZE) + sizeof(int16_t))

#define HTTP_CLIENT_MEM_REGION_OFFSET   ( 0U )
#define HTTP_CLIENT_MEM_REGION_SIZE     ( sizeof(webserver_network_t) )

#define HTTP_CLIENT_WIFI_NETWORK_SSID_LEN   ( 33U )
#define HTTP_CLIENT_WIFI_NETWORK_PASS_LEN   ( 32U )

typedef struct webserver_network
{
    char *ssid;
    char *pass;
    bool valid;
} webserver_network_t;

void webserver_background_task(void);
BaseType_t webserver_init(uint32_t port);
uint32_t webserver_cmdsvr(uint8_t argc, char *argv[]);

bool usb_serial_getline(char *buffer);
void usb_serial_init(Transport *pipe);

void interface_read(uint32_t offset,
                    uint32_t length,
                    void *buffer);
void interface_write(uint32_t offset,
                     uint32_t length,
                     void *buffer);
TaskHandle_t *pxInterfaceTaskHandleGet(void);
BaseType_t vInterfaceInit(UBaseType_t uxPriority,
                          uint16_t usStackDepth,
                          Transport *pipe);

#endif /* _INTERFACE_H_ */
