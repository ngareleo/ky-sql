/**
 * This is a set of helpers for allocating memory for huge structs
 *
 * The guideline for using this allocation.
 * - At the start of the function definition, you should allocate all the memory you need
 * - Then check if all the memory was properly allocated.
 * - During the check, we do a single batched allocation and update the pointers
 * - If any allocation failed, you cleanup all the memory you've allocated and exit the function.
 *
 * For those situations that need memory conditionally, you should compute the conditon first, then allocate the memory using this allocator.
 * Remember to free the allocator and all the memory you've borrowed.
 */
#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <stdbool.h>

typedef struct
{
    size_t AllocSize;
    int **AllocMem;
} AllocObject;

typedef enum ALLOCATION_STATUS
{
    SUCCESS,
    FAILED
} AllocationStatus;

typedef struct
{
    AllocationStatus Status;
    AllocObject **Allocs;
    int AllocCount;
} Allocator;

void FreeAlloc(Allocator *alloc);
void FreeAllocObj(AllocObject *alloc);
Allocator *MallocInit();
bool *VerifyAlloc(Allocator *);
AllocationStatus *BatchAlloc(Allocator *);
void ReleaseAlloc(Allocator *alloc);
void *Malloc(size_t, Allocator *);

#endif
