#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define iscommand(str1, str2, str2_len)    ((strstr(str1, str2) == str1) && (*(str1 + str2_len) <= ' '))

char *pcUtilNextChar( const char *cpStr );
void vUtilClearLine(uint32_t ulLineSize);
void vUtilStrClear( char * cpStr, uint32_t ulLen );

#ifdef __cplusplus
}
#endif
#endif /* _UTIL_H_ */