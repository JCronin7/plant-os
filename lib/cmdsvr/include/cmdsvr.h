#ifndef _CMDSVR_H_
#define _CMDSVR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <msg.h>

#define CMDSVR_NAME_LENGTH_MAX  16U
#define CMDSVR_HELP_LENGTH_MAX  32U

#define CMDSVR_STATUS_SUCCESS       0U
#define CMDSVR_STATUS_CMD_NOT_FOUND 1U

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
        char href;
    };

    TaskHandle_t task_hdl_get(void);
    void register_cmd(const char * const name,
                      const char * const help,
                      Command_cb cmd_func_ptr,
                      char href);
    BaseType_t initialize(UBaseType_t task_priority,
                          uint16_t task_stack,
                          Msg::Pipe *pipe);
}

#endif /* _CMDSVR_H_ */
