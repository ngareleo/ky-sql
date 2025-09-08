#include <stdlib.h>
#include "utilities.h"

char *NullGuardStr(char *str)
{
    return str ? str : "NULL";
}