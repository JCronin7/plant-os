#ifndef _CMDSVR_H_
#define _CMDSVR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <msg.h>

#define CMDSVR_NAME_LENGTH_MAX  32U
#define CMDSVR_HELP_LENGTH_MAX  64U
#define CMDSVR_COMMANDS_MAX 16U

#define CMDSVR_STATUS_SUCCESS       0U
#define CMDSVR_STATUS_COMMAND_OVF   1U

namespace Cmdsvr
{
    /** Callback function type */
    typedef uint32_t ( *Command_cb )( uint8_t, char *[] );

    /** Command struct */
    struct CommandInst
    {
        char name[CMDSVR_NAME_LENGTH_MAX];
        char help[CMDSVR_HELP_LENGTH_MAX];
        Command_cb cmd_func_ptr;
    };

    TaskHandle_t task_hdl_get(void);
    uint32_t register_cmd(const char * const name,
                          const char * const help,
                          Command_cb cmd_func_ptr);
    BaseType_t initialize(UBaseType_t task_priority,
                          uint16_t task_stack,
                          Msg::Pipe *pipe);
}

#endif /* _CMDSVR_H_ */
