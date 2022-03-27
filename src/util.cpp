#include "util.h"

#include <stdarg.h>

void Logf(FILE* fp, const char* file, int32_t line, const char* func, const char* format, ...)
{
    va_list ap;
    fprintf(fp, "%s: ", func);
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
    fprintf(fp, " (%s:%d)\n", file, line);
}
