#ifndef _CMDSVR_H_
#define _CMDSVR_H_

#include <stdint.h>
#include <FreeRTOS_SAMD21.h>
#include <transport.h>

#define CMDSVR_NAME_LENGTH_MAX  32U
#define CMDSVR_HELP_LENGTH_MAX  64U

#define CMDSVR_STATUS_SUCCESS       0U
#define CMDSVR_STATUS_COMMAND_OVF   1U

/** Callback function type */
typedef uint32_t ( *cmdsvr_cmd_cb_t )( uint8_t, char *[] );

/** Message payload structure */
typedef struct cmdsvr_cmd
{
    char name[CMDSVR_NAME_LENGTH_MAX];
    uint8_t argc;
    char *argv[];
} cmdsvr_cmd_t;

/** Command struct */
typedef struct cmdsvr_cmd_inst
{
    char name[CMDSVR_NAME_LENGTH_MAX];
    char help[CMDSVR_HELP_LENGTH_MAX];
    cmdsvr_cmd_cb_t cmd_func_ptr;
} cmdsvr_cmd_inst_t;

TaskHandle_t cmdsvr_task_hdl_get(void);
uint32_t cmdsvr_register_cmd(const char * const name,
                             const char * const help,
                             cmdsvr_cmd_cb_t cmd_func_ptr);
BaseType_t cmdsvr_init(UBaseType_t task_priority,
                       uint16_t task_stack_depth,
                       Transport *pipe);

#endif /* _CMDSVR_H_ */
