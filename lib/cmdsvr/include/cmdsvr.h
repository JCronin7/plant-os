#ifndef _CMDSVR_H_
#define _CMDSVR_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

#define CMDSVR_COMMANDS_MAX     64U
#define CMDSVR_NAME_LENGTH_MAX  32U
#define CMDSVR_HELP_LENGTH_MAX  128U
#define CMDSVR_PROMPT           "plant-os: $ "
#define CMDSVR_TERMINATOR       "\r\0\n"
#define CMDSVR_PROMPT_SIZE      (sizeof(CMDSVR_PROMPT))

#define CMDSVR_STATUS_SUCCESS       0U
#define CMDSVR_STATUS_COMMAND_OVF   1U

namespace cmdsvr
{
    typedef uint32_t (*command_cb)(uint8_t, char *[]);

    struct command
    {
        char name[CMDSVR_NAME_LENGTH_MAX];
        char help[CMDSVR_HELP_LENGTH_MAX];
        command_cb callback;
        uint8_t name_len;
    };

    uint32_t register_command(const char *name,
                              const char *help,
                              command_cb callback_func);

    void init(usb_serial_class *serial,
              uint32_t baudrate);

    void background_thread(void *arg);
}

#endif /* _CMDSVR_H_ */
