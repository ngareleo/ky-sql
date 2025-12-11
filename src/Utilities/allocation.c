#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/allocation.h"

Allocator *MallocInit()
{
    Allocator *alloc;
    alloc = malloc(sizeof(Allocator));
    if (!alloc)
    {
        fprintf(stderr, "(malloc-init) malloc failed \n");
        // !! A null allocator will blow up during allocation and propagate down safely so that we can recover all the memory
        return NULL;
    }

    alloc->Allocs = NULL;
    alloc->AllocCount = 0;
    return alloc;
};

bool *VerifyAlloc(Allocator *alloc)
{
    if (!alloc)
    {
        fprintf(stderr, "(malloc) Alloc object is invalid \n");
        return false;
    }

    for (int aIdx = 0; aIdx < alloc->AllocCount; aIdx++)
    {
        AllocObject *ao = alloc->Allocs[aIdx];
        if (!ao)
        {
            fprintf(stderr, "(verify-alloc) Allocation object is invalid \n");
            return false;
        }

        // Then set the new value
        int *realPtr;
        realPtr = malloc(ao->AllocSize);

        if (!*(ao->AllocMem))
        {
            fprintf(stderr, "(malloc) malloc_std failed \n");
            return false;
        }

        **(ao->AllocMem) = *realPtr;
    }

    return true;
};

void *Malloc(size_t size, Allocator *alloc)
{
    if (!alloc)
    {
        fprintf(stderr, "(malloc) Alloc object is invalid \n");
        return NULL;
    }

    AllocObject *allocObj;
    allocObj = malloc(sizeof(allocObj));
    if (!allocObj)
    {
        fprintf(stderr, "(malloc) malloc failed \n");
        return NULL;
    }

    int **temp;
    *temp = malloc(sizeof(int *));
    if (!temp)
    {
        free(allocObj);
        fprintf(stderr, "(malloc) malloc failed \n");
        return NULL;
    }

    **temp = NULL;
    allocObj->AllocMem = temp;
    allocObj->AllocSize = size;

    if (alloc->AllocCount == 0)
    {
        // first allocation so we allocate the allocs
        alloc->Allocs = malloc(sizeof(AllocObject *));
        if (!alloc->Allocs)
        {
            fprintf(stderr, "(malloc-init) malloc failed \n");
            // !! A null allocator will blow up during allocation and propagate down safely so that we can recover all the memory
            return NULL;
        }
    }
    else
    {
        // create space just for one more alloc object
        realloc(alloc->Allocs, sizeof(AllocObject) + sizeof(alloc->Allocs));
        if (!alloc->Allocs[alloc->AllocCount])
        {
            fprintf(stderr, "(malloc-init) malloc failed \n");
            // !! A null allocator will blow up during allocation and propagate down safely so that we can recover all the memory
            return NULL;
        }
    }

    alloc->Allocs[alloc->AllocCount] = allocObj;
    alloc->AllocCount++;

    // null terminate the new array
    alloc->Allocs[alloc->AllocCount] = NULL;
    return *temp;
};

void FreeAllocObj(AllocObject *alloc)
{
    if (alloc)
    {
        if (alloc->AllocMem)
        {
            free(alloc->AllocMem);
        }
    }
}

void FreeAlloc(Allocator *alloc)
{
    if (alloc)
    {
        if (alloc->Allocs)
        {
            for (int idx = 0; idx < alloc->AllocCount; idx++)
            {
                if (alloc->Allocs[idx])
                {
                    free(alloc->Allocs[idx]);
                }
            }
            free(alloc->Allocs);
        }

        free(alloc);
    }
}