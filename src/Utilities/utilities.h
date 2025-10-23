#ifndef UTILITIES_H
#define UTILITIES_H

#include <time.h>
#include "allocation.h"

int Count(void **);
char *NowDefaultValue();
char *NullGuardStr(char *);

Allocator *MallocInit();
bool *VerifyAllocation(Allocator *);
void *Malloc(size_t, Allocator *);

#endif