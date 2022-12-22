#include <interface_loc.h>

uint32_t CommandHistory::offset(uint16_t idx)
{
    return relative_offset + (idx * entry_size);
}

uint16_t CommandHistory::increment(uint16_t value, uint16_t inc)
{
    return (value + inc) % elements;
}

void CommandHistory::read(int16_t idx, char *buffer)
{
    return interface_read(offset(idx), entry_size, buffer);
}

void CommandHistory::write(int16_t idx, char *buffer)
{
    return interface_write(offset(idx), entry_size, buffer);
}

CommandHistory::CommandHistory(uint32_t relative_offset,
                               uint16_t elements,
                               uint16_t entry_size)
{
    this->relative_offset = relative_offset;
    this->elements = elements;
    this->entry_size = entry_size;

    interface_read(relative_offset,
                   sizeof(int16_t),
                   &front);
    interface_read(relative_offset,
                   sizeof(int16_t),
                   &back);
    itr = back;
}

void CommandHistory::reset(void)
{
    itr = back;
}

char *CommandHistory::next(void)
{
    char *ret = nullptr;
    read(itr, ret);
    if (ret != nullptr)
    {
        itr = increment(itr, 1);
    }

    return ret;
}

char *CommandHistory::previous(void)
{
    char *ret = nullptr;
    read(itr, ret);
    if (ret != nullptr)
    {
        itr = increment(itr, -1);
    }

    return ret;
}

void CommandHistory::insert(char *str)
{
    write(back, str);
    if (back == front)
    {
        front = increment(front, 1);
        interface_write(HISTORY_BUFFER_START,
                       sizeof(int16_t),
                       &front);
    }

    back = increment(back, 1);
    interface_write(HISTORY_BUFFER_END,
                   sizeof(int16_t),
                   &back);
}
