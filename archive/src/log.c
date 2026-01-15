// #include <string.h>
// #include <stdio.h>
// #include <stdarg.h>
// #include <stdbool.h>
// #include "headers/log.h"

// // Todo: Use flags to run debug conditionally by compiling this conditionally
// int pf(logType type, const char *str, ...)
// {
//     char *color = COLOR_RESET, in[strlen(str) + MAX_PREFIX_WIDTH], prefix[6];
//     va_list args;
//     int result;

//     switch (type)
//     {
//     case DEBUG:
//         color = COLOR_BLUE;
//         strcpy(prefix, "DEBUG");
//         break;
//     case ERROR:
//         color = COLOR_RED;
//         strcpy(prefix, "ERROR");
//         break;
//     case INFO:
//         color = COLOR_CYAN;
//         strcpy(prefix, "INFO");
//         break;
//     case WARN:
//         color = COLOR_MAGENTA;
//         strcpy(prefix, "WARN");
//         break;
//     default:
//         break;
//     }

//     // add the color
//     printf("%s", color);

//     // wrap with extra info
//     sprintf(in, "\n[%s] %s\n", prefix, str);

//     // output
//     va_start(args, str);
//     result = vprintf(in, args);
//     va_end(args);

//     // reset
//     printf("%s", COLOR_RESET);
//     return result;

//     return 0;
// }
