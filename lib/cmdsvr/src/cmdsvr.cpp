#include <cmdsvr.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)  (strstr(str1, str2) == str1 && *(str1 + str2_len) <= ' ')

static uint32_t cmdsvr_registered_cmds = 0;

static usb_serial_class *cmdsvr_serial_ptr = nullptr;

static cmdsvr::command cmdsvr_commands[CMDSVR_COMMANDS_MAX];

static char* nextchar(char *str)
{
    while (*str == ' ') { str++; }
    return str;
}

static void print_prompt(void)
{
    cmdsvr_serial_ptr->print(CMDSVR_PROMPT);
}

static void clearline(uint32_t line_size)
{
    cmdsvr_serial_ptr->write('\r');
    for (uint32_t i = 0; i < line_size; i++)
    {
        cmdsvr_serial_ptr->write(' ');
    }

    cmdsvr_serial_ptr->write('\r');
}

static void decode_command(char *cmd)
{
    // trim leading spaces
    cmd = nextchar(cmd);

    if (iscommand(cmd, CMDSVR_TERMINATOR, sizeof(CMDSVR_TERMINATOR)))
    {
        return;
    }

    for (uint8_t i = 0; i < cmdsvr_registered_cmds; i++)
    {
        if (iscommand(cmd, cmdsvr_commands[i].name, cmdsvr_commands[i].name_len))
        {
            char *cmd_args[CMDSVR_NAME_LENGTH_MAX] = {nullptr};
            int arg_idx = 0;

            cmd = nextchar(cmd + cmdsvr_commands[i].name_len);

            while (iscommand(cmd, CMDSVR_TERMINATOR, sizeof(CMDSVR_TERMINATOR)) == false)
            {
                cmd_args[arg_idx++] = cmd;
                /* skip over valid ascii characters, terminate on space or string end */
                while (*cmd > ' ')
                {
                    cmd++;
                }

                cmd = nextchar(cmd);
            }

            cmdsvr_commands[i].callback(arg_idx, cmd_args);
        }
        else if (iscommand(cmd, "help", sizeof("help")))
        {
            char name_buffer[CMDSVR_NAME_LENGTH_MAX];

            strcpy(name_buffer, cmdsvr_commands[i].name);

            for (uint8_t j = cmdsvr_commands[i].name_len; j < CMDSVR_NAME_LENGTH_MAX; j++)
            {
                name_buffer[j] = ' ';
            }

            cmdsvr_serial_ptr->write(name_buffer, CMDSVR_NAME_LENGTH_MAX);
            cmdsvr_serial_ptr->println(cmdsvr_commands[i].help);
        }
        else
        {
            cmdsvr_serial_ptr->println("Unrecognized command, type \'help\' for options");
        }
    }
}

uint32_t cmdsvr::register_command(const char *name,
                                  const char *help,
                                  cmdsvr::command_cb callback_func)
{
    if (cmdsvr_registered_cmds != CMDSVR_COMMANDS_MAX)
    {
        strcpy(cmdsvr_commands[cmdsvr_registered_cmds].name, name);
        strcpy(cmdsvr_commands[cmdsvr_registered_cmds].help, help);
        cmdsvr_commands[cmdsvr_registered_cmds].callback = callback_func;
        cmdsvr_commands[cmdsvr_registered_cmds].name_len = strlen(name);

        cmdsvr_registered_cmds++;
        return CMDSVR_STATUS_SUCCESS;
    }

    return CMDSVR_STATUS_COMMAND_OVF;
}

void cmdsvr::init(usb_serial_class *serial_ptr,
                  uint32_t baudrate)
{
    cmdsvr_serial_ptr = serial_ptr;
    cmdsvr_serial_ptr->begin(115200);
    print_prompt();
}

void cmdsvr::background_thread(void *arg)
{
    char command_str[CMDSVR_HELP_LENGTH_MAX] = {0};
    TickType_t previous_wake = 0;
    uint8_t bytes = 0;

    while (true)
    {
        if (cmdsvr_serial_ptr->available() > 0)
        {
            uint8_t new_char = cmdsvr_serial_ptr->read();
            switch (new_char)
            {
            case '\n':
                command_str[bytes++] = '\0';
                cmdsvr_serial_ptr->println();
                decode_command(command_str);
                bytes = 0;
                // Move later
                print_prompt();
                continue;
            case 0x08: // backspace
                clearline(CMDSVR_PROMPT_SIZE + bytes);
                print_prompt();
                if (bytes > 0)
                {
                    bytes--;
                }
                cmdsvr_serial_ptr->write(command_str, bytes);
                continue;
            default:
                command_str[bytes] = new_char;
                cmdsvr_serial_ptr->write(command_str[bytes]);
                bytes++;
                continue;
            }
        }

        vTaskDelayUntil(&previous_wake, 100 / portTICK_PERIOD_MS);
    }
}