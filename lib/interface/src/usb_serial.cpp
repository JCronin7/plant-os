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

typedef enum usb_serial_input
{
    NO_INPUT = 0,
    CHARACTER,   /* a-z, A-Z, 0-9 and space */
    ESCAPE,      /* Arrow key */
    CARRIAGE,    /* Carriage return */
    NEWLINE,     /* New line */
    ARROW,       /* Arrow key, '[' character after esc */
    UP_ARROW,    /* Up arrow key */
    DOWN_ARROW,  /* Down arrow key */
    RIGHT_ARROW, /* Right arrow key */
    LEFT_ARROW,  /* Left arrow key */
    BACKSPACE,   /* Backspace key */
    ENTER,       /* Enter key */
} usb_serial_input_enum;

static CommandHistory cmds(HISTORY_BUFFER_OFFSET,
                           USB_SERIAL_SAVED_CMDS,
                           USB_SERIAL_LINE_SIZE);

static Msg::Client client;

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

static inline usb_serial_input_enum is_arrow(char input)
{
    if (input <= 'D' && input >= 'A')
    {
        return (usb_serial_input_enum)(input - 'A' + UP_ARROW);
    }

    return NO_INPUT;
}

static void usb_serial_receive(usb_serial_input_enum *previous_action,
                               char *input)
{
    *input = (char)Serial.read();

    switch (*input)
    {
    case '\r':
        *previous_action = CARRIAGE;
        break;
    case '\n':
        *previous_action = (*previous_action == CARRIAGE) ? ENTER : NEWLINE;
        break;
    case ASCII_BACKSPACE:
        *previous_action = BACKSPACE;
        break;
    case ASCII_ESCAPE:
        *previous_action = ESCAPE;
        break;
    default:
        if (*input == '[' && *previous_action == ESCAPE)
        {
            *previous_action = ARROW;
        }
        else if (*previous_action == ARROW)
        {
            *previous_action = is_arrow(*input);
        }
        else
        {
            *previous_action = CHARACTER;
        }
    }
}

/**
 * @brief
 *
 * @param buffer
 * @return true
 * @return false
 */
bool Interface::usb_serial_getline(char *buffer)
{
    static usb_serial_input_enum state = NO_INPUT;
    static uint16_t length = 0;
    uint32_t response;

    if (client.receive(&response))
    {
        if (response != 0)
        {
            Serial.println("Command Error!");
        }

        PRINT_PROMPT();
    }

    if (Serial.available() > 0)
    {
        char ucNewChar = 0;
        usb_serial_receive(&state, &ucNewChar);

        switch (state)
        {
        /* Append string terminator to command and return */
        case ENTER:
            if (length != 0)
            {
                buffer[length++] = '\0';
                cmds.insert(buffer);
                cmds.reset();
            }
            Serial.write('\n');
            client.send(buffer, length);
            length = 0;
            break;

        /* Remove last entered character */
        case BACKSPACE:
            if (length > 0)
            {
                --length;
            }

            clearline(PROMPT_SIZE + length + 1);
            PRINT_PROMPT();
            Serial.write(buffer, length);
            break;

        /* Add char to buffer */
        case CHARACTER:
            buffer[length] = ucNewChar;
            Serial.write(buffer[length++]);
            break;

        case UP_ARROW:
            clearline(PROMPT_SIZE + length + 1);
            //strcpy(buffer, cmds.previous());
            //length = strlen(buffer);
            PRINT_PROMPT();
            Serial.print(buffer);
            break;

        case DOWN_ARROW:
            clearline(PROMPT_SIZE + length + 1);
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

    return state == ENTER;
}

void Interface::usb_serial_init(Msg::Pipe *pipe)
{
    client = Msg::Client(pipe);
    client.idSet(0);
    PRINT_PROMPT();
}
