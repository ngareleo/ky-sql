#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "allocation.h"

Allocator *MallocInit()
{
    Allocator *alloc;
    alloc = malloc(sizeof(Allocator));
    if (!alloc)
    {
        fprintf(stderr, "(malloc-init) malloc failed \n");
        // a null allocator will blow up during allocation and propagate down safely so that we can recover all the memory
        return NULL;
    }
    return alloc;
};

bool *VerifyAllocation(Allocator *alloc)
{
    return alloc->Status == FAILED ? false : true;
};

void *Malloc(size_t size, Allocator *alloc)
{
    const mem = malloc(size);
    if (!mem)
    {
        alloc->Status = FAILED;
        fprintf(stderr, "(malloc-init) malloc failed \n");
        return NULL;
    }
    return mem;
};
