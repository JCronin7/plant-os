#ifndef _LSENSE_H_
#define _LSENSE_H_

#include <stdint.h>
#include <FreeRTOS_TEENSY4.h>

namespace lsense
{
    BaseType_t init(void);

    uint32_t cmd(uint8_t argc, char *argv[]);
}
#endif /* _LSENSE_H_ */
