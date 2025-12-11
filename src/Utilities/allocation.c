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
        // Then set the new value
        void *realPtr;
        realPtr = malloc(ao->AllocSize);
        if (!*(ao->AllocMem))
        {
            for (int a_idx = 0; a_idx < aIdx; a_idx++)
            {
                free(**(ao->AllocMem));
            }

            fprintf(stderr, "(malloc) malloc_std failed \n");
            return false;
        }

        **(ao->AllocMem) = realPtr;
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
        // first allocation
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
        realloc(alloc->Allocs, alloc->AllocCount + 1);
        if (!alloc->Allocs[alloc->AllocCount])
        {
            fprintf(stderr, "(malloc-init) malloc failed \n");
            // !! A null allocator will blow up during allocation and propagate down safely so that we can recover all the memory
            return NULL;
        }
    }

    alloc->Allocs[alloc->AllocCount] = allocObj;
    alloc->AllocCount++;
    return *temp;
};

void FreeAllocObj(AllocObject *alloc)
{
    if (alloc)
    {
    }
}

void FreeAlloc(Allocator *alloc)
{
    if (alloc)
    {
        free(alloc);
    }
}