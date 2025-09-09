#include <stdlib.h>
#include "utilities.h"

int Count(void **ptr)
{
    if (!ptr)
    {
        return 0;
    }

    int c = 0;
    void *curr = ptr[0];
    for (; curr != NULL; c++, curr++)
        ;
    return c;
}