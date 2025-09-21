
#ifndef READER_H
#define READER_H

#include <stdio.h>

typedef struct
{
    int StartPosition; // Where to start the read
    size_t ReadSize;   // How much to read
} ReadRequest;

typedef struct
{
    FILE *Readable;
    char *ReadBuffer;
} ReaderMetadata;

int HandleReadRequest(ReadRequest *, ReaderMetadata *);

#endif