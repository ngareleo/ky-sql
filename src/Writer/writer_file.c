#include <stdlib.h>
#include <stdio.h>
#include "include/writer_file.h"

int HandleWrite(WriteRequest *request, WriterMetadata *meta)
{
    if (!meta || !request || !meta->Writable)
    {
        fprintf(stderr, "(writer-handle-write-err) arguments are NULL \n");
        return -1;
    }

    clearerr(meta->Writable);

    if (fwrite(request->Payload, sizeof(request->Payload), 1, meta->Writable) < 1)
    {
        fprintf(stderr, "(writer-handle-write-err) write failed \n");
        return -1;
    }
    else if (ferror(meta->Writable) != 0)
    {
        fprintf(stderr, "(writer-handle-write-err) write failed \n");
        return -1;
    }
    else if (feof(meta->Writable) != 0)
    {
        fprintf(stderr, "(writer-handle-write-err) write failed with EOF error \n");
        return -1;
    }

    clearerr(meta->Writable);
    fprintf(stdout, "(writer-handle-write-info) write complete \n");
    fflush(meta->Writable);
    return 0;
}

void FreeWriteRequest(WriteRequest *req)
{
    if (req)
    {
        free(req);
    }
}

void FreeWriteMetadata(WriterMetadata *meta)
{
    if (meta)
    {
        fflush(meta->Writable);
        fclose(meta->Writable);
        free(meta);
    }
}