#include <string.h>
#include <cmdsvr.h>
#include <vector>

#define CMDSVR_BUFFER_SIZE 32U

#define cmd_match(input, command)   (strcmp(input, command) == 0)
#define href_match(input, href)     ((input)[0] == '/' && (input)[1] == href)

/** Registered commands */
static std::vector<Cmdsvr::CommandInst> commands;

/** Background task handle */
static TaskHandle_t cmdsvr_task_hdl;

/** Message Queue */
static Msg::Server server;

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
static inline uint8_t splitwords(char * str,
                                 char *split[])
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
 *  Command server thread, parses commands sent
 *  by interface
 *
 * @param[in] arg - thread args, unused
 */
static void thread_entry(void *arg)
{
    char *argv[10] = {NULL};
    char cmd_buffer[CMDSVR_BUFFER_SIZE] = {0};

    while (true)
    {
        bool valid_cmd = false;
        uint32_t cmd_return = CMDSVR_STATUS_SUCCESS;

        /* No commands found */
        if (server.receive(cmd_buffer) == 0)
        {
            server.send(&cmd_return, sizeof(uint32_t));
            vTaskSuspend(cmdsvr_task_hdl);
            continue;
        }

        /* Decode command string into list of words */
        uint8_t argc = splitwords((char*)cmd_buffer, argv);
        if (argc == 0)
        {
            server.send(&cmd_return, sizeof(uint32_t));
            vTaskSuspend(cmdsvr_task_hdl);
            continue;
        }

        /* Search registered commands, print help if not found */
        for (auto cmd_inst : commands)
        {
            /* Parse arguments and execute command */
            if (cmd_match(argv[0], cmd_inst.name))
            {
                cmd_return = cmd_inst.cmd_func_ptr(argc, argv);
                valid_cmd = true;
            }
            /* If msg begins with /, parse as href */
            else if (href_match(argv[0], cmd_inst.href))
            {
                cmd_return = cmd_inst.cmd_func_ptr(1, argv);
                valid_cmd = true;
            }
            else if (cmd_match(argv[0], "help"))
            {
                uint8_t cursor_pos = Serial.print(cmd_inst.name);
                while (cursor_pos++ < CMDSVR_NAME_LENGTH_MAX)
                {
                    Serial.write(' ');
                }

                Serial.println(cmd_inst.help);
                valid_cmd = true;
            }
        }

        /* Print help message if invalid input */
        if (valid_cmd == false)
        {
            cmd_return = CMDSVR_STATUS_CMD_NOT_FOUND;
        }

        server.send(&cmd_return, sizeof(uint32_t));
        vTaskSuspend(cmdsvr_task_hdl);
    }
}

/**
 * @brief
 *  Returns command server background thread handle
 *
 * @return
 *  Task handle to background thread
 */
TaskHandle_t Cmdsvr::task_hdl_get(void)
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
 * @param[in] href - Command HREF
 *
 * @return
 *  status code
 */
void Cmdsvr::register_cmd(const char * const name,
                          const char * const help,
                          Command_cb cmd_func_ptr,
                          char href)
{
    CommandInst cmd_inst;

    strcpy(cmd_inst.name, name);
    strcpy(cmd_inst.help, help);
    cmd_inst.cmd_func_ptr = cmd_func_ptr;
    cmd_inst.href = href;
    commands.push_back(cmd_inst);
}

/**
 * @brief
 *  Command server init function
 *
 * @param[in] task_priority - Background task priority
 * @param[in] task_stack    - Background task stack depth
 * @param[in] pipe          - Message queue pointer
 *
 * @return
 *  Status, 1 on success, 0 on fail
 */
BaseType_t Cmdsvr::initialize(UBaseType_t task_priority,
                              uint16_t task_stack,
                              Msg::Pipe *pipe)
{
    BaseType_t status;

    server = Msg::Server(pipe);
    status = xTaskCreate(thread_entry,
                         "CMDSVR",
                         task_stack,
                         NULL,
                         task_priority,
                         &cmdsvr_task_hdl);
    vTaskSuspend(cmdsvr_task_hdl);
    server.registerServer(cmdsvr_task_hdl);
    return status;
}
