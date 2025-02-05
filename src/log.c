#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

// Color definitions
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define MAX_PREFIX_WIDTH 10 // We wrap a max of 10 characters for each log

typedef enum LOGTYPE
{
    DEBUG,
    INFO,
    LOG,
    WARN,
    ERROR
} logType;

int pf(logType type, const char *str, ...)
{
    char *color = COLOR_RESET, in[strlen(str) + MAX_PREFIX_WIDTH], prefix[6];
    va_list args;
    int result;

    switch (type)
    {
    case DEBUG:
        color = COLOR_BLUE;
        strcpy(prefix, "DEBUG");
        break;
    case ERROR:
        color = COLOR_RED;
        strcpy(prefix, "ERROR");
        break;
    case INFO:
        color = COLOR_CYAN;
        strcpy(prefix, "INFO");
        break;
    case WARN:
        color = COLOR_MAGENTA;
        strcpy(prefix, "WARN");
        break;
    default:
        break;
    }

    // add the color
    printf("%s", color);

    // wrap with extra info
    sprintf(in, "\n[%s] %s\n", prefix, str);

    // output
    va_start(args, str);
    result = vprintf(in, args);
    va_end(args);

    // reset
    printf("%s", COLOR_RESET);
    return result;
}

#define error_print(...) pf(ERROR, __VA_ARGS__)
#define info_print(...) pf(INFO, __VA_ARGS__)
#define debug_print(...) pf(DEBUG, __VA_ARGS__)
