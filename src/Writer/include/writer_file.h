#ifndef WRITER_FILE_H
#define WRITER_FILE_H

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