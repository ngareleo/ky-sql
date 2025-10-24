#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/reader_file.h"

int HandleRead(ReadRequest *req, ReaderMetadata *meta)
{
    if (ValidateReadSessionArgs(req, meta) != 0)
    {
        fprintf(stderr, "(handle-read-request) Incorrect parameters passed \n");
        return -1;
    }

    FILE *readable;
    readable = fopen(meta->File, "r");
    if (!readable)
    {
        fprintf(stderr, "(handle-read-request) Could not open file \n");
        return -1;
    }

    if (fseek(readable, req->StartPosition, SEEK_SET) < 0)
    {
        fprintf(stderr, "(handle-read-request) Cannot set fp to location \n");
        return -1;
    }

    if (fread(meta->ReadBuffer, req->ReadSize, 1, readable) < 1)
    {
        fprintf(stderr, "(handle-read-request) Read failed \n");
        return -1;
    }
    else if (ferror(readable) != 0)
    {
        fprintf(stderr, "(handle-read-request) Read failed \n");
        return -1;
    }
    else if (feof(readable) != 0)
    {
        fprintf(stderr, "(writer-handle-write) EOF error \n");
        return -1;
    }

    fclose(readable);
    return 0;
}

int ValidateReadSessionArgs(ReadRequest *req, ReaderMetadata *meta)
{
    if (!req || !meta)
    {
        fprintf(stderr, "(handle-read-request-err) incorrect parameters passed \n");
        return -1;
    }
    else if (!meta->File || strlen(meta->File) < 1)
    {
        fprintf(stderr, "(handle-read-request-err) readable in NULL \n");
        return -1;
    }
    else if (!meta->ReadBuffer)
    {
        fprintf(stderr, "(handle-read-request-err) buffer is NULL \n");
        return -1;
    }
    else if (req->ReadSize < 1)
    {
        fprintf(stderr, "(handle-read-request-err) read size is incorrect \n");
        return -1;
    }

    return 0;
}

void FreeReadRequest(ReadRequest *req)
{
    if (req)
    {
        free(req);
    }
}