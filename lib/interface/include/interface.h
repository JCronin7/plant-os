#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdint.h>
#include <msg.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <FreeRTOS_SAMD21.h>
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

namespace Interface
{
    class Webserver
    {
    private:
        static WiFiServer server;

        static WiFiClient client;

        static uint32_t connection_status;

        static void status(void);

    public:
        static void initialize(uint16_t port);

        static BaseType_t enable(void);

        static BaseType_t connect(webserver_network_t *credentials,
                                  int8_t ucMaxAttempts=5);

        static void spin(void);

        static uint32_t cmdsvr(uint8_t argc, char *argv[]);
    };

    bool usb_serial_getline(char *buffer);
    void usb_serial_init(Msg::Pipe *pipe);

    void interface_read(uint32_t offset,
                        uint32_t length,
                        void *buffer);
    void interface_write(uint32_t offset,
                        uint32_t length,
                        void *buffer);

    TaskHandle_t task_hdl_get(void);
    BaseType_t initialize(UBaseType_t uxPriority,
                          uint16_t usStackDepth,
                          Msg::Pipe *pipe);
}


#endif /* _INTERFACE_H_ */
