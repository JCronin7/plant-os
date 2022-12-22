#ifndef _HAL_H_
#define _HAL_H_

#include <stdint.h>

void flash_init(void);
uint32_t flash_size(void);
bool flash_write(uint32_t base,
                 uint32_t size,
                 const void *bytes);
bool flash_read(uint32_t base,
                uint32_t size,
                void *bytes);

uint32_t flast_wr_cmd(uint8_t argc,
                      char *argv[]);
uint32_t flast_rd_cmd(uint8_t argc,
                      char *argv[]);
uint32_t flash_print_cmd(uint8_t argc,
                         char *argv[]);

#endif /* _HAL_H_ */
