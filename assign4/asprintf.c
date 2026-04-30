#include "printf.h"
#include "malloc.h"

int vasprintf(char **p_str, const char *format, va_list ap) {
#if 0
    va_list copy;

    va_copy(copy, ap);
    int len = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
#else
    int len = vsnprintf(NULL, 0, format, ap);
#endif
    char *result = malloc(len + 1);
    vsnprintf(result, len+1, format, ap);
    *p_str = result;
    return len;
}

int asprintf(char **p_str, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    int len = vasprintf(p_str, format, ap);
    va_end(ap);
    return len;
}