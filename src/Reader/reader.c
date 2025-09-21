#include <stdio.h>
#include <stdlib.h>
#include "reader.h"

int HandleReadRequest(ReadRequest *request, ReaderMetadata *meta)
{
    if (ValidateReadSessionArgs(request, meta) != 0)
    {
        fprintf(stderr, "(handle-read-request-err) incorrect parameters passed \n");
        return -1;
    }

    // Incase we didn't clear errors
    clearerr(meta->Readable);

    if (fseek(meta->Readable, request->StartPosition, SEEK_SET) < 0)
    {
        fprintf(stderr, "(handle-read-request-err) cannot set fp to location \n");
        return -1;
    }

    if (fread(meta->ReadBuffer, request->ReadSize, 1, meta->Readable) < 1)
    {
        fprintf(stderr, "(handle-read-request-err) read failed \n");
        return -1;
    }
    else if (ferror(meta->Readable) != 0)
    {
        fprintf(stderr, "(handle-read-request-err) read failed \n");
        return -1;
    }
    else if (feof(meta->Readable) != 0)
    {
        fprintf(stderr, "(writer-handle-write-err) EOF error \n");
        return -1;
    }

    return 0;
}

// For dev logs
int ValidateReadSessionArgs(ReadRequest *request, ReaderMetadata *meta)
{
    if (!request || !meta)
    {
        fprintf(stderr, "(handle-read-request-err) incorrect parameters passed \n");
        return -1;
    }
    else if (!meta->Readable)
    {
        fprintf(stderr, "(handle-read-request-err) readable in NULL \n");
        return -1;
    }
    else if (!meta->ReadBuffer)
    {
        fprintf(stderr, "(handle-read-request-err) buffer is NULL \n");
        return -1;
    }
    else if (request->ReadSize < 1)
    {
        fprintf(stderr, "(handle-read-request-err) read size is incorrect \n");
        return -1;
    }

    return 0;
}