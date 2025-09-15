#include <stdlib.h>
#include <stdio.h>
#include "writer.h"

int HandleWrite(WriteRequest *request, WriterMetadata *meta)
{
    if (!meta || !request || !meta->Writable)
    {
        fprintf(stderr, "(writer-handle-write-err) arguments are NULL \n");
        return -1;
    }

    // Incase we didn't clear errors
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
        fprintf(stderr, "(writer-handle-write-err) write failed \n");
        return -1;
    }

    fprintf(stdout, "(writer-handle-write-info) write complete \n");
    fflush(meta->Writable);
    return 0;
}