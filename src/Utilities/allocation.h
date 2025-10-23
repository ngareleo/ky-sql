#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>

typedef enum ALLOCATION_STATUS
{
    SUCCESS,
    FAILED
} AllocationStatus;

typedef struct
{
    AllocationStatus Status;
} Allocator;

#endif
