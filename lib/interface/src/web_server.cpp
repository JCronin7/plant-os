#include <interface.h>
#include <string.h>
#include <hal.h>

#define NETWORK_NAME_PROMPT "Network Name: "
#define NETWORK_PASS_PROMPT "Network Pass: "

using namespace Interface;

/** Wifi server class */
WiFiServer Webserver::server = WiFiServer(80);

/** Wifi connection client */
WiFiClient Webserver::client = WiFiClient();

/** Asynchronous message queue*/
Msg::Client Webserver::msg_client = Msg::Client();

/** Connection status */
uint32_t Webserver::connection_status = WL_IDLE_STATUS;

const char **Webserver::pagedata = nullptr;

uint32_t Webserver::pagesize = 0;

void Webserver::status(void)
{
    Serial.print("Connection Status: ");
    Serial.println((uint32_t)connection_status);

    // print the SSID of the network you're attached to:
    if (connection_status == WL_CONNECTED)
    {
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());

        WiFi.localIP().printTo(Serial);
        Serial.print(WiFi.RSSI());
        Serial.println("dBm");
    }
}

void Webserver::initialize(uint16_t port,
                           const char *pagedata[],
                           uint32_t pagesize,
                           Msg::Pipe *pipe)
{
    server = WiFiServer(port);
    Webserver::pagedata = pagedata;
    Webserver::pagesize = pagesize;
    msg_client = Msg::Client(pipe);
    msg_client.idSet(2);
    enable();
}

BaseType_t Webserver::enable(void)
{
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed");
        return pdFAIL;
    }

    const char *pcFirmwareVer = WiFi.firmwareVersion();
    if (pcFirmwareVer[0] < '1')
    {
        Serial.println("Please upgrade the firmware");
        return pdFAIL;
    }

    return pdPASS;
}

BaseType_t Webserver::connect(webserver_network_t *credentials,
                              int8_t ucMaxAttempts)
{
    int8_t ucAttempts = ucMaxAttempts;

    while (connection_status != WL_CONNECTED && ucAttempts-- > 0)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(credentials->ssid);

        connection_status = WiFi.begin(credentials->ssid,
                                       credentials->pass);
        for (uint8_t i = 0; i < 3; i++)
        {
            Serial.write('.');
            vTaskDelay(1000);
        }
        Serial.write('\n');
    }

    switch (ucAttempts)
    {
    case -1:
        Serial.println("Failed to connect");
        return pdFAIL;
    case 5:
        return pdPASS;
    default:
        client = server.available();
        status();
        Serial.println("\nStarting connection to server...");
        server.begin();
        return pdPASS;
    }
}

void Webserver::spin(void)
{
    client = server.available();

    if (!client || connection_status != WL_CONNECTED)
    {
        return;
    }

    String request = "";

    while (client.connected())
    {
        if (!client.available())
        {
            continue;
        }

        char c = client.read();
        if (c == '\n')
        {
            if (request.length() == 0)
            {
                for (uint32_t i = 0; i < pagesize; i++)
                {
                    client.println(pagedata[i]);
                }

                break;
            }

            request = "";
        }
        else if (c != '\r')
        {
            request += c;
        }

        if (request.endsWith("HTTP/1.1"))
        {
            uint8_t end = request.indexOf("HTTP/1.1") - 1;
            uint8_t start = request.indexOf("GET ") + sizeof("GET ") - 1;
            char *href = &request[start];
            href[end - start] = '\0';
            if (strcmp(href, "/favicon.ico") != 0)
            {
                msg_client.send(href, end - start + 1);
            }
            request = "";
        }
    }

    client.stop();
}

uint32_t Webserver::cmdsvr(uint8_t argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println("No command specified.");
        return 0;
    }

    if (strcmp(argv[1], "login") == 0)// && argc >= 4)
    {
        webserver_network_t credentials;
        credentials.ssid = "SHAW-990B"; //argv[2];
        credentials.pass = "famous8618cover"; //argv[3];
        return (connect(&credentials, 5) != pdPASS);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        status();
    }

    return 0;
}
