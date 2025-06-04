#include <stdlib.h>
#include "utilities.h"

const int Count(const void **ptr)
{
    if (ptr == NULL)
    {
        return 0;
    }

    int c = 0;
    void *curr = ptr[0];
    for (; curr != NULL; c++, curr++)
        ;
    return c;
}