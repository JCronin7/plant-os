#include <cmdsvr.h>
#include <string.h>
#include <hal.h>
#include <util.h>

#define ASCII_BACKSPACE ( (char)0x08 )
#define ASCII_ESCAPE    ( (char)0x1B )

#define vCmdSvrPrintPrompt( ) ( xHalUartUsbSerialPrint(CMDSVR_PROMPT) )

static CmdsvrCommand_t pxCmdsvrCommands[CMDSVR_COMMANDS_MAX];
static uint32_t ulCmdSvrRegisteredCmdCnt = 0;

static uint32_t ulCmdsvrDecodeCommand(const char * pcCmdStr)
{
    const CmdsvrCommand_t *pxInst = NULL;
    bool ucHelpFlag = false;

    /* trim leading spaces */
    pcCmdStr = pcUtilNextChar(pcCmdStr);

    /* If command string only contained spaces, return */
    if ( *pcCmdStr == '\0' )
    {
        return CMDSVR_STATUS_SUCCESS;
    }

    for ( uint32_t i = 0; i < ulCmdSvrRegisteredCmdCnt; i++ )
    {
        pxInst = &(pxCmdsvrCommands[i]);

        /* Parse arguments and execute command */
        if ( iscommand(pcCmdStr, pxInst->pcCmdName, pxInst->ucCmdNameStrLen) )
        {
            char *pcCmdArgs[CMDSVR_NAME_LENGTH_MAX] = { NULL };
            uint32_t ulArgIdx = 0;

            pcCmdStr = pcUtilNextChar(pcCmdStr + pxInst->ucCmdNameStrLen);

            while (*pcCmdStr != '\0')
            {
                pcCmdArgs[ulArgIdx++] = (char *)pcCmdStr;

                /* skip over valid ascii characters, terminate on space or string end */
                while (*pcCmdStr > ' ')
                {
                    pcCmdStr++;
                }

                pcCmdStr = pcUtilNextChar(pcCmdStr);
            }

            return pxInst->ulCallbackFunc(ulArgIdx, pcCmdArgs);
        }
        else if ( iscommand(pcCmdStr, "help", sizeof("help") - 1) )
        {
            char name_buffer[CMDSVR_NAME_LENGTH_MAX];

            ucHelpFlag = true;

            strcpy(name_buffer, pxInst->pcCmdName);

            for (uint8_t j = pxInst->ucCmdNameStrLen; j < CMDSVR_NAME_LENGTH_MAX; j++)
            {
                name_buffer[j] = ' ';
            }

            xHalUartUsbSerialWrite(name_buffer, CMDSVR_NAME_LENGTH_MAX);
            xHalUartUsbSerialPrintln(pxInst->pcCmdHelp);
        }
    }

    if (ucHelpFlag == false)
    {
        xHalUartUsbSerialPrintln("Unrecognized command, type \'help\' for options");
    }

    return CMDSVR_STATUS_SUCCESS;
}

static CmdsvrAction_e xCmdsvrActionStateMachine(const char cInput)
{
    static CmdsvrAction_e xCmdsvrAction = NO_INPUT;

    switch (cInput)
    {
    case '\r':
        xCmdsvrAction = CARRIAGE;
        break;
    case '\n':
        xCmdsvrAction = (xCmdsvrAction == CARRIAGE) ? ENTER : NEWLINE;
        break;
    case ASCII_BACKSPACE:
        xCmdsvrAction = BACKSPACE;
        break;
    case ASCII_ESCAPE:
        xCmdsvrAction = ESCAPE;
        break;
    default:
        if (cInput == '[' && xCmdsvrAction == ESCAPE)
        {
            xCmdsvrAction = ARROW;
            break;
        }

        if (xCmdsvrAction == ARROW)
        {
            xCmdsvrAction = (cInput <= 'D' && cInput >= 'A') ? cInput - 'A' + UP_ARROW : NO_INPUT;
            break;
        }

        xCmdsvrAction = CHARACTER;
        break;
    }

    return xCmdsvrAction;
}

uint32_t xCmdsvrRegisterCmd(const char *pcCmdName,
                            const char *pcCmdHelp,
                            CmdsvrCommandCb_t ulCallbackFunc)
{
    if ( ulCmdSvrRegisteredCmdCnt >= (uint32_t)CMDSVR_COMMANDS_MAX )
    {
        return CMDSVR_STATUS_COMMAND_OVF;
    }

    uint32_t ulInstIdx = ulCmdSvrRegisteredCmdCnt++;
    CmdsvrCommand_t *pxInst = &(pxCmdsvrCommands[ulInstIdx]);

    strcpy(pxInst->pcCmdName, pcCmdName);
    strcpy(pxInst->pcCmdHelp, pcCmdHelp);
    pxInst->ulCallbackFunc = ulCallbackFunc;
    pxInst->ucCmdNameStrLen = strlen(pcCmdName);

    return CMDSVR_STATUS_SUCCESS;
}

BaseType_t xCmdsvrInit( void )
{
    BaseType_t xStatus;

    xStatus = xTaskCreate(vCmdsvrBackgroundThread,
                          NULL,
                          100 * configMINIMAL_STACK_SIZE,
                          NULL,
                          3,
                          NULL);
    vCmdSvrPrintPrompt();

    return xStatus;
}

void vCmdsvrBackgroundThread( void *arg )
{
    char pcCmdString[CMDSVR_HELP_LENGTH_MAX] = { 0 };
    TickType_t xPreviousWake = 0;
    uint32_t ulCmdReturn;
    uint8_t ucBytes = 0;

    while (true)
    {
        if (xHalUartUsbSerialAvailable() > 0)
        {
            const uint8_t ucNewChar = xHalUartUsbSerialGetChar();

            switch ( xCmdsvrActionStateMachine(ucNewChar) )
            {
            /* Append string terminator to command and decode*/
            case ENTER:
                pcCmdString[ucBytes++] = '\0';
                xHalUartUsbSerialPutChar('\n');
                ulCmdReturn = ulCmdsvrDecodeCommand(pcCmdString);
                if (ulCmdReturn)
                {
                    xHalUartUsbSerialPutChar(ulCmdReturn);
                    xHalUartUsbSerialPutChar('\n');
                }
                vUtilStrClear(pcCmdString, ucBytes);
                ucBytes = 0;
                vCmdSvrPrintPrompt();
                continue;

            /* Rewrite entire line without previously entered character */
            case BACKSPACE:
                if (ucBytes == 0)
                {
                    continue;
                }

                vUtilClearLine(CMDSVR_PROMPT_SIZE + ucBytes);
                vCmdSvrPrintPrompt();
                ucBytes--;
                xHalUartUsbSerialWrite(pcCmdString, ucBytes);
                continue;

            /* Add unhandled char to command buffer */
            case CHARACTER:
                pcCmdString[ucBytes] = ucNewChar;
                xHalUartUsbSerialPutChar(pcCmdString[ucBytes]);
                ucBytes++;
                continue;

            /// @todo implement
            case UP_ARROW:
                xHalUartUsbSerialWrite("up\n", 3);
                vCmdSvrPrintPrompt();
                continue;

            /// @todo implement
            case DOWN_ARROW:
                xHalUartUsbSerialWrite("down\n", 5);
                vCmdSvrPrintPrompt();
                continue;

            /// @todo implement
            case RIGHT_ARROW:
                xHalUartUsbSerialWrite("right\n", 6);
                vCmdSvrPrintPrompt();
                continue;

            /// @todo implement
            case LEFT_ARROW:
                xHalUartUsbSerialWrite("left\n", 5);
                vCmdSvrPrintPrompt();
                continue;

            default:
                continue;
            }
        }

        vTaskDelayUntil(&xPreviousWake, 10 / portTICK_PERIOD_MS);
    }
}