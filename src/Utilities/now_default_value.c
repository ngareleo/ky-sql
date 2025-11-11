#include <stdio.h>
#include <time.h>
#include "now_default_value.h"

char *NowDefaultValue()
{
    time_t now;
    char *buff;

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(now-default-value-err) now default value failed");
        return NULL;
    }

    buff = malloc(sizeof(time_t));
    if (!buff)
    {
        fprintf(stderr, "(now-default-value-err) malloc failed");
        return NULL;
    }

    sprintf(buff, "%ld", now);
    return buff;
}