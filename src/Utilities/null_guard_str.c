#include <stdlib.h>
#include "include/null_guard_str.h"

char *NullGuardStr(char *str)
{
    if (!str)
    {
        return "null-guard-null";
    }

    return str;
}