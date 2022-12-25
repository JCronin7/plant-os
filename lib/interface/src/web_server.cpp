#include <interface.h>
#include <string.h>
#include <hal.h>

#define NETWORK_NAME_PROMPT ("Network Name: ")
#define NETWORK_PASS_PROMPT ("Network Pass: ")

using namespace Interface;

WiFiServer Webserver::server = WiFiServer(80);
WiFiClient Webserver::client = WiFiClient();
uint32_t Webserver::connection_status = WL_IDLE_STATUS;

void Webserver::status(void)
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    WiFi.localIP().printTo(Serial);
    Serial.print(WiFi.RSSI());
    Serial.println("dBm");
}

void Webserver::initialize(uint16_t port)
{
    server = WiFiServer(port);
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

    String currentLine = "";

    while (client.connected())
    {
        if (!client.available())
        {
            continue;
        }

        char c = client.read();
        if (c == '\n')
        {
            if (currentLine.length() == 0)
            {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();

                // create the buttons
                client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
                client.print("Click <a href=\"/L\">here</a> turn the LED off<br><br>");

                // The HTTP response ends with another blank line:
                client.println();
                // break out of the while loop:
                break;
            }
            else
            {
                currentLine = "";
            }
        }
        else if (c != '\r')
        {                     // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /H"))
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        if (currentLine.endsWith("GET /L"))
        {
            digitalWrite(LED_BUILTIN, LOW);
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
    else if (strcmp(argv[0], "status") == 0)
    {
        status();
    }

    return 0;
}