#include <interface.h>
#include <string.h>
#include <hal.h>

#define NETWORK_NAME_PROMPT ("Network Name: ")
#define NETWORK_PASS_PROMPT ("Network Pass: ")

/* Web Server */
static WiFiServer server(80);
static WiFiClient client;
int32_t connection_status = WL_IDLE_STATUS;

/**
 * @brief print board's IP address
 *
 */
static inline void print_ip(void)
{
    WiFi.localIP().printTo(Serial);
}

/**
 * @brief print the received signal strength
 *
 */
static inline void print_rssi(void)
{
    Serial.print(WiFi.RSSI());
    Serial.println("dBm");
}

static void print_status(void)
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    print_ip();
    print_rssi();
}

static BaseType_t enable_wifi(void)
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

static BaseType_t connect(webserver_network_t *credentials,
                          int8_t ucMaxAttempts=5)
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
        print_status();
        Serial.println("\nStarting connection to server...");
        server.begin();
        return pdPASS;
    }
}

void webserver_background_task(void)
{
    client = server.available();

    if (!client || connection_status != WL_CONNECTED)
    {
        return;
    }

    // if you get a client,
    Serial.println("new client"); // print a message out the serial port
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
        if (client.available())
        {                           // if there's bytes to read from the client,
            char c = client.read(); // read a byte, then
            Serial.write(c);        // print it out the serial monitor
            if (c == '\n')
            { // if the byte is a newline character

                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:
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

                    int randomReading = analogRead(A1);
                    client.print("Random reading from analog pin: ");
                    client.print(randomReading);

                    // The HTTP response ends with another blank line:
                    client.println();
                    // break out of the while loop:
                    break;
                }
                else
                { // if you got a newline, then clear currentLine:
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
    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
}

BaseType_t webserver_init(uint32_t port)
{
    BaseType_t status = pdPASS;
    server = WiFiServer(port);
    status &= enable_wifi();
    return status;
}

uint32_t webserver_cmdsvr(uint8_t argc, char *argv[])
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
    else if (strcmp(argv[0], "ip") == 0)
    {
        print_ip();
    }
    else if (strcmp(argv[0], "rssi") == 0)
    {
        print_rssi();
    }

    return 0;
}