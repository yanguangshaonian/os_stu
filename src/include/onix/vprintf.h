#ifndef ONIX_VPRINTF_H
#define ONIX_VPRINTF_H

#include <onix/stdarg.h>

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif