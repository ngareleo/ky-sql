#ifndef LOG_H
#define LOG_H

// Color definitions
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define MAX_PREFIX_WIDTH 9 // We wrap a max of 10 characters for each log

typedef enum LOGTYPE
{
    DEBUG,
    INFO,
    LOG,
    WARN,
    ERROR
} logType;

int pf(logType type, const char *str, ...);

#define error(...) pf(ERROR, __VA_ARGS__)
#define info(...) pf(INFO, __VA_ARGS__)
#define debug(...) pf(DEBUG, __VA_ARGS__)

#endif