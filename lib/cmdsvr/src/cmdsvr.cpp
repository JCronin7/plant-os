#include <string.h>
#include <cmdsvr.h>
#include <queue.h>

#define CMDSVR_COMMANDS_MAX 16U

/** Registered commands */
static cmdsvr_cmd_inst_t commands[CMDSVR_COMMANDS_MAX];

/** Total number of registered commands */
static uint32_t cmd_count = 0;

/** Background task handle */
static TaskHandle_t cmdsvr_task_hdl;

/** Message Queue */
static Transport *cmd_pipe;

/**
 * @brief
 *  Splits a string containing words into a list of words
 *
 * @param[in] str    - string to parse
 * @param[out] split - List of words
 *
 * @return
 *  Number of words found
 */
static inline uint8_t splitwords(char * str, char **split)
{
    uint8_t count = 0;

    while (*str != '\0')
    {
        if (*str == ' ')
        {
            str++;
            continue;
        }

        split[count++] = str;
        while (*str > ' ') { str++; }
        *str++ = '\0';
    }

    return count;
}

/**
 * @brief
 *  Command server background thread, parses commands sent
 *  by interface
 *
 * @param[in] arg - thread args, unused
 */
static void cmdsvr_background_thread(void *arg)
{
    transport_payload cmd_pl;
    TickType_t previous_wake = 0;
    char *argv[10] = {NULL};
    uint32_t cmd_return = 0;

    while (true)
    {
        bool valid_cmd = false;

        /* No commands found */
        if (cmd_pipe->request_receive(&cmd_pl) == 0)
        {
            continue;
        }

        /* Decode command string into list of words */
        uint8_t argc = splitwords((char*)cmd_pl.payload, argv);
        if (argc == 0)
        {
            cmd_pl.pl_size = 0;
            cmd_pipe->response_send(&cmd_pl);
            continue;
        }

        /* Search registered commands, print help if not found */
        for (uint32_t i = 0; i < cmd_count; i++)
        {
            const cmdsvr_cmd_inst_t *cmd_inst_ptr = &(commands[i]);

            /* Parse arguments and execute command */
            if (strcmp(argv[0], cmd_inst_ptr->name) == 0)
            {
                cmd_return = cmd_inst_ptr->cmd_func_ptr(argc, argv);
                valid_cmd = true;
            }
            else if (strcmp(argv[0], "help") == 0)
            {
                uint8_t ucNameLen = Serial.print(cmd_inst_ptr->name);
                for (uint8_t j = ucNameLen; j < CMDSVR_NAME_LENGTH_MAX; j++)
                {
                    Serial.write(' ');
                }

                Serial.println(cmd_inst_ptr->help);
                valid_cmd = true;
            }
        }

        /* Print help message if invalid input */
        if (valid_cmd == false)
        {
            Serial.println("Unrecognized command, type \'help\' for options");
        }

        memcpy(cmd_pl.payload, &cmd_return, sizeof(uint32_t));
        cmd_pl.pl_size = sizeof(uint32_t);
        cmd_pipe->response_send(&cmd_pl);

        vTaskDelayUntil(&previous_wake, 10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief
 *  Returns command server background thread handle
 *
 * @return
 *  Task handle to background thread
 */
TaskHandle_t cmdsvr_task_hdl_get(void)
{
    return cmdsvr_task_hdl;
}

/**
 * @brief
 *  Register a new command with command server
 *
 * @param[in] name - Command name
 * @param[in] help - Command help string
 * @param[in] cmd_func_ptr - Command callback function
 *
 * @return
 *  status code
 */
uint32_t cmdsvr_register_cmd(const char * const name,
                             const char * const help,
                             cmdsvr_cmd_cb_t cmd_func_ptr)
{
    if (cmd_count >= (uint32_t)CMDSVR_COMMANDS_MAX)
    {
        return CMDSVR_STATUS_COMMAND_OVF;
    }

    uint32_t idx = cmd_count++;
    cmdsvr_cmd_inst_t *cmd_inst_ptr = &(commands[idx]);

    strcpy(cmd_inst_ptr->name, name);
    strcpy(cmd_inst_ptr->help, help);
    cmd_inst_ptr->cmd_func_ptr = cmd_func_ptr;

    return CMDSVR_STATUS_SUCCESS;
}

/**
 * @brief
 *  Command server init function
 *
 * @param[in] task_priority     - Background task priority
 * @param[in] task_stack_depth  - Background task stack depth
 * @param[in] xCmdPipe          - Message pointer
 *
 * @return
 *  Status, 1 on success, 0 on fail
 */
BaseType_t cmdsvr_init(UBaseType_t task_priority,
                       uint16_t task_stack_depth,
                       Transport *pipe)
{
    BaseType_t status;

    cmd_pipe = pipe;
    status = xTaskCreate(cmdsvr_background_thread,
                         "CMDSVR",
                         task_stack_depth,
                         NULL,
                         task_priority,
                         &cmdsvr_task_hdl);

    return status;
}
