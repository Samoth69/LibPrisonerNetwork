#include "net_prisoner.h"

/**
 * @brief show debug message for the net lib
 * in this lib, net_dbg should be used insteadof printf directly.
 * this allow the #define NETDEBUG to easily turn on/off all message
 * from this library
 * 
 * @param format work exactly like printf
 * @param ... 
 */
void net_dbg(const char *format, ...)
{
    if (NETDEBUG)
    {
        printf("NET: ");

        //see: https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=3321010338384ecdc6633a8b032bb0ed6aa9b19a
        va_list arg;

        va_start(arg, format);

        //https://www.cplusplus.com/reference/cstdio/vfprintf/
        vfprintf(stdout, format, arg);
        
        va_end(arg);
    }
}