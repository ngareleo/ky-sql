#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>

typedef struct
{
    int startPosition;
    char *payload;
} WriteRequest;

typedef struct
{
    FILE *writable;
} WriterMetadata;

/** This function will be called each time a request is ready to be queued */
int HandleWrite(WriteRequest *, WriterMetadata *);

#endif