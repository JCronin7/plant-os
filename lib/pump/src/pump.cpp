#include <pump.h>

#define MAX_INSTANCES ( 2 )

static Pump *pxInstances[MAX_INSTANCES];
static uint32_t ulInstanceCount = 0;

Pump::Pump(uint8_t ucPin)
{
    this->ucPin = ucPin;
    this->ucState = false;
    pinMode(ucPin, OUTPUT);

    pxInstances[ulInstanceCount++] = this;
}

void Pump::set(bool ucState)
{
    this->ucState = ucState;
    digitalWrite(this->ucPin, ucState);
}

uint32_t Pump::cmdsvr(uint8_t argc, char *argv[])
{
    Pump *pxInst = NULL;

    if (  argc == 0 )
    {
        return 1;
    }

    pxInst = (argc == 1) ? pxInstances[0] : pxInstances[atoi(argv[0])];

    if (strcmp(argv[1], "on") == 0)
    {
        pxInst->set(HIGH);
        return 0;
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        pxInst->set(LOW);
        return 0;
    }

    return 1;
}
