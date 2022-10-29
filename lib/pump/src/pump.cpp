#include <pump.h>
#include <string.h>

#define iscommand(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

