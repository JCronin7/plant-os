#include <hal.h>
#include <string.h>
#include <FlashStorage.h>
#include <Arduino.h>
#include <FlashAsEEPROM.h>

// Use 0-2. Larger for more debugging messages
#define FLASH_DEBUG ( 0 )
//BOARD_NAME
//FLASH_STORAGE_SAMD_VERSION
#define FLASH_LENGTH ( 1U << 5ULL )

#define FLASH_PRINT_COL_NUM ( 8U )

typedef struct Flash
{
    uint8_t data[FLASH_LENGTH];
} Flash_t;

FlashStorage(flash_inst, Flash_t);
static Flash_t flash_data;

/**
 * @brief Force Eeprom init
 *
 */
void flash_init(void)
{
    flash_inst.read(&flash_data);
}

uint32_t flash_size(void)
{
    return (uint32_t)FLASH_LENGTH;
}

bool flash_write(uint32_t base,
                 uint32_t size,
                 const void *bytes)
{
    if (base + size < FLASH_LENGTH)
    {
        memcpy(flash_data.data + base, bytes, size);
        flash_inst.write(flash_data);
        return true;
    }

    return false;
}

bool flash_read(uint32_t base,
                uint32_t size,
                void *bytes)
{
    if (base + size < FLASH_LENGTH)
    {
        memcpy(bytes, flash_data.data + base, size);
        return true;
    }

    return false;
}

uint32_t flast_wr_cmd(uint8_t argc,
                      char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    const uint16_t addr = atoi(argv[0]);
    const uint8_t data  = atoi(argv[1]);

    flash_write(addr, 1, &data);
    return 0;
}

uint32_t flast_rd_cmd(uint8_t argc,
                      char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    const uint16_t addr = atoi(argv[0]);
    uint8_t data = atoi(argv[1]);

    flash_read(addr, 1, &data);
    Serial.println(data);
    return 0;
}

uint32_t flash_print_cmd(uint8_t argc,
                         char *argv[])
{
    uint32_t rows   = FLASH_LENGTH >> 4;
    uint16_t len    = FLASH_LENGTH;
    uint8_t offset  = 0;

    if (argc >= 1)
    {
        offset = atoi(argv[0]);
    }

    if (argc >= 2)
    {
        len = atoi(argv[1]);
        if (offset + len > FLASH_LENGTH)
        {
            return 1;
        }
    }

    Serial.print("      ");
    for (uint8_t i = 0; i < FLASH_PRINT_COL_NUM; i++)
    {
        Serial.print(i, HEX);
    }

    Serial.write('\n');

    for (uint8_t i = 0; i < rows; i++)
    {
        Serial.print(i << 4, HEX);
        for (uint8_t j = 0; j < FLASH_PRINT_COL_NUM; j++)
        {
            Serial.write('\t');
            uint8_t data = 0;
            flash_read((FLASH_PRINT_COL_NUM * i) + j, 1, &data);
            Serial.print(data, HEX);
        }
    }

    Serial.write('\n');

    return 0;
}