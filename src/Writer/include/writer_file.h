#ifndef WRITER_FILE_H
#define WRITER_FILE_H

#include <stdio.h>
#include "write_size.h"

typedef struct
{
    int StartPosition;
    char *Payload;
} WriteRequest;

typedef struct
{
    FILE *Writable;
} WriterMetadata;

int HandleWrite(WriteRequest *, WriterMetadata *);
void FreeWriteRequest(WriteRequest *);

#endif