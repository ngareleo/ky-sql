#include <stdio.h>
#include "utilities.h"

char *NowDefaultValue()
{
    time_t now;
    char *buff;

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(create-books-table-defintion-info) time failed");
        return NULL;
    }

    buff = malloc(sizeof(time_t));
    if (!buff)
    {
        fprintf(stderr, "(create-books-table-defintion-info) malloc failed");
        return NULL;
    }

    sprintf(buff, "%ld", now);
    return buff;
}