#ifndef _PUMP_H_
#define _PUMP_H_

#include <stdint.h>
#include <string.h>
#include <Arduino.h>

class Pump
{
private:
    uint8_t ucPin;
    bool ucState;

public:
    Pump(uint8_t ucPin);
    void set(bool ucState);
    static uint32_t cmdsvr(uint8_t argc, char *argv[]);
};

#endif /* _PUMP_H_ */
