#include <stdlib.h>
#include "utilities.h"

char *NullGuardStr(char *str)
{
    if (!str)
    {
        return "null-guard-null";
    }

    return str;
}