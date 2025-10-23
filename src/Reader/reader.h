
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
    char *Readable;
    char *ReadBuffer;
} ReaderMetadata;

int HandleRead(ReadRequest *, ReaderMetadata *);

#endif