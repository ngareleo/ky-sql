
#ifndef READER_FILE_H
#define READER_FILE_H

#include <stdio.h>

typedef struct
{
    int StartPosition; // Where to start the read
    size_t ReadSize;   // How much to read
} ReadRequest;

typedef struct
{
    char *File;       // The name of the file
    char *ReadBuffer; // A buffer to write to
} ReaderMetadata;

int HandleRead(ReadRequest *, ReaderMetadata *);
void FreeReadRequest(ReadRequest *);

#endif