#ifndef _CMDSVR_H_
#define _CMDSVR_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMDSVR_COMMANDS_MAX         ( 64U )
#define CMDSVR_NAME_LENGTH_MAX      ( 32U )
#define CMDSVR_HELP_LENGTH_MAX      ( 128U )
#define CMDSVR_PROMPT               ( "plant-os: $ " )
#define CMDSVR_PROMPT_SIZE          ( sizeof(CMDSVR_PROMPT) )

#define CMDSVR_STATUS_SUCCESS       ( 0U )
#define CMDSVR_STATUS_COMMAND_OVF   ( 1U )

typedef uint32_t ( *CmdsvrCommandCb_t )( uint8_t, char *[] );

typedef struct CmdsvrCommand
{
    char pcCmdName[CMDSVR_NAME_LENGTH_MAX];
    char pcCmdHelp[CMDSVR_HELP_LENGTH_MAX];
    CmdsvrCommandCb_t ulCallbackFunc;
    uint8_t ucCmdNameStrLen;
} CmdsvrCommand_t;

typedef enum CmdsvrAction
{
    CHARACTER = 0,  /* a-z, A-Z, 0-9 and space */
    ESCAPE,         /* Arrow key */
    BACKSPACE,      /* Backspace key */
    CARRIAGE,       /* Carriage return */
    NEWLINE,        /* New line */
    ENTER,          /* Enter key */
    ARROW,          /* Arrow key, '[' character after esc */
    UP_ARROW,       /* Up arrow key */
    DOWN_ARROW,     /* Down arrow key */
    RIGHT_ARROW,    /* Right arrow key */
    LEFT_ARROW,     /* Left arrow key */
    NO_INPUT,
} CmdsvrAction_e;

uint32_t xCmdsvrRegisterCmd( const char * const pcCmdName,
                             const char * const pcCmdHelp,
                             CmdsvrCommandCb_t ulCallbackFunc );
BaseType_t xCmdsvrInit( void );
void vCmdsvrBackgroundThread( void *arg );

#ifdef __cplusplus
}
#endif
#endif /* _CMDSVR_H_ */
