#include <Arduino.h>
#include <interface.h>
#include <interface_loc.h>
#include <hal.h>

#define ASCII_BACKSPACE ((char)0x08)
#define ASCII_ESCAPE    ((char)0x1B)
#define PROMPT          ("plant-os: $ ")
#define PROMPT_SIZE     (sizeof(PROMPT))
#define PRINT_PROMPT()  (Serial.print(PROMPT))

#define HISTORY_BUFFER_OFFSET           (0U)

using namespace Interface;

static CommandHistory cmds(HISTORY_BUFFER_OFFSET,
                           USB_SERIAL_SAVED_CMDS,
                           USB_SERIAL_LINE_SIZE);

/** Asynchronous message queue*/
Msg::Client UsbSerial::msg_client = Msg::Client();

/**
 * @brief
 *  Clear the current line of UART terminal.
 *
 * @param[in] size - Number of bytes to clear
 */
static void clearline(uint32_t size)
{
    Serial.write('\r');
    for (uint32_t i = 0; i < size; i++)
    {
        Serial.write(' ');
    }

    Serial.write('\r');
}

/**
 * @brief
 *  Check if the input char corresponds to the last character
 *  in an arrow key sequence
 *
 * @param[in] input - input char
 *
 * @return
 *  Enum for the output arrow key, NO_INPUT if invalid input
 */
static inline usb_serial_input is_arrow(char input)
{
    if (input <= 'D' && input >= 'A')
    {
        return (usb_serial_input)(input - 'A' + UP_ARROW);
    }

    return NO_INPUT;
}

/**
 * @brief
 *  State machine determining the key input given, (some keys
 *  correspond to 2-3 inputs)
 *
 * @param[in] previous_action - Previous state
 * @param[in] input - new input
 *
 * @return
 *  New state
 */
usb_serial_input UsbSerial::receive(usb_serial_input previous_action,
                                    char input)
{
    switch (input)
    {
    case '\r':
        return CARRIAGE;
    case '\n':
        return (previous_action == CARRIAGE) ? ENTER : NEWLINE;
    case ASCII_BACKSPACE:
        return BACKSPACE;
    case ASCII_ESCAPE:
        return ESCAPE;
    default:
        if (input == '[' && previous_action == ESCAPE)
        {
            return ARROW;
        }
        else if (previous_action == ARROW)
        {
            return is_arrow(input);
        }

        return CHARACTER;
    }
}

/**
 * @brief
 *  Initialize USB Serial class
 *
 * @param[in] pipe - pipe object for async. message passing
 */
void UsbSerial::initialize(Msg::Pipe *pipe)
{
    msg_client = Msg::Client(pipe);
    msg_client.idSet(0);
    PRINT_PROMPT();
}

/**
 * @brief
 *  Function to monitor serial port and wait for commands
 */
void UsbSerial::spin(void)
{
    static usb_serial_input state = NO_INPUT;
    static String buffer = "";
    uint32_t response;

    if (msg_client.receive(&response))
    {
        if (response != 0)
        {
            Serial.println("Unrecognized command, type \'help\' for options");
        }

        PRINT_PROMPT();
    }

    if (Serial.available() > 0)
    {
        char new_char = Serial.read();
        state = receive(state, new_char);

        switch (state)
        {
        /* Append string terminator to command and return */
        case ENTER:
            if (buffer.length() != 0)
            {
                buffer += '\0';
                //cmds.insert(buffer.c_str());
                cmds.reset();
            }
            Serial.write('\n');
            msg_client.send((void*)buffer.c_str(), buffer.length());
            buffer = "";
            break;

        /* Remove last entered character */
        case BACKSPACE:
            clearline(PROMPT_SIZE + buffer.length());
            buffer.remove(buffer.length() - 1);
            PRINT_PROMPT();
            Serial.print(buffer);
            break;

        /* Add char to buffer */
        case CHARACTER:
            buffer += new_char;
            Serial.write(new_char);
            break;

        case UP_ARROW:
            clearline(PROMPT_SIZE + buffer.length());
            //strcpy(buffer, cmds.previous());
            //length = strlen(buffer);
            PRINT_PROMPT();
            Serial.print(buffer);
            break;

        case DOWN_ARROW:
            clearline(PROMPT_SIZE + buffer.length());
            //strcpy(buffer, previous_cmd_next());
            //length = strlen(buffer);
            PRINT_PROMPT();
            Serial.print(buffer);
            break;

        /// @todo implement
        case RIGHT_ARROW:
            Serial.write('\n');
            PRINT_PROMPT();
            break;

        /// @todo implement
        case LEFT_ARROW:
            Serial.write('\n');
            PRINT_PROMPT();
            break;

        default:
            break;
        }
    }
}
