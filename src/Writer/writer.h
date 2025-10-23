#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>

typedef struct
{
    int StartPosition;
    char *Payload;
} WriteRequest;

typedef struct
{
    FILE *Writable;
} WriterMetadata;

/** This function will be called each time a request is ready to be queued */
int HandleWrite(WriteRequest *, WriterMetadata *);

void FreeWriteRequest(WriteRequest *);

#endif