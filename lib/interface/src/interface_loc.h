#ifndef _INTERFACE_LOC_H_
#define _INTERFACE_LOC_H_

#include <interface.h>

class CommandHistory
{
private:
    uint32_t relative_offset;
    uint16_t elements;
    uint16_t entry_size;
    int16_t itr;
    int16_t front;
    int16_t back;

    uint32_t offset(uint16_t idx);
    uint16_t increment(uint16_t value, uint16_t inc);
    void read(int16_t idx, char *buffer);
    void write(int16_t idx, char *buffer);

public:
    CommandHistory(uint32_t relative_offset,
                   uint16_t elements,
                   uint16_t entry_size);
    void reset(void);
    char *next(void);
    char *previous(void);
    void insert(char *str);
};

#endif /* _INTERFACE_LOC_H_ */