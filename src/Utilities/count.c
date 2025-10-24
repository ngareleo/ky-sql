#include <stdlib.h>
#include <stdio.h>
#include "include/count.h"

int Count(void **ptr)
{
    if (!ptr)
    {
        return 0;
    }

    int c = 0;
    void *curr;
    while ((curr = ptr[c++]))
        ;

    return c - 1;
}