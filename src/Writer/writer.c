#include <stdlib.h>
#include <stdio.h>
#include "writer.h"

int HandleWrite(WriteRequest *request, WriterMetadata *meta)
{
    if (!meta || !request || !meta->writable)
    {
        fprintf(stderr, "(writer-handle-write) arguments are NULL \n");
        return -1;
    }

    // Incase we didn't clear errors
    clearerr(meta->writable);
    if (fwrite(request->payload, sizeof(request->payload), 1, meta->writable) < 1)
    {
        fprintf(stderr, "(writer-handle-write) write failed \n");
        return -1;
    }
    else if (ferror(meta->writable) != 0)
    {
        fprintf(stderr, "(writer-handle-write) write failed \n");
        return -1;
    }
    else if (feof(meta->writable) != 0)
    {
        fprintf(stderr, "(writer-handle-write) write failed \n");
        return -1;
    }

    fflush(meta->writable);
    return 0;
}