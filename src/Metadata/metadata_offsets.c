#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "metadata_offsets.h"
#include "../Utilities/utilities.h"

TableOffset *NewTableOffset(int id, int offset)
{
    TableOffset *tableOffset;

    tableOffset = malloc(sizeof(TableOffset));
    if (!tableOffset)
    {
        fprintf(stderr, "(new-table-offset-err) malloc failed.\n");
        return NULL;
    }

    tableOffset->Offset = offset;
    tableOffset->Id = id;

    return tableOffset;
}

char *FormatTableOffset(const TableOffset *tableOffset)
{
    FILE *stream;
    char *buffer;
    size_t size;

    if (!tableOffset)
    {
        fprintf(stderr, "(format-table-offset-err) tableOffset is null.\n");
        return NULL;
    }

    stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        fprintf(stderr, "(format-table-offset-err) open_memstream failed.\n");
        return NULL;
    }

    sprintf(stream, "(log) id         = %s.\n", tableOffset->Id);
    sprintf(stream, "(log) offset     = %d.\n", tableOffset->Offset);

    fclose(stream);
    return buffer;
}

char *FormatOffset(const Offset *offset)
{
    FILE *stream;
    char *buffer;
    size_t size;

    if (!offset)
    {
        fprintf(stderr, "(format-offset-err) offset is null.\n");
        return NULL;
    }

    stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        fprintf(stderr, "(format-offset-err) open_memstream failed.\n");
        return NULL;
    }

    sprintf(stream, "(log) immediate-write-buffer-offset = %d.\n", offset->ImwebOffset);

    for (int c = 0; c < offset->TableCount; c++)
    {
        fprintf(stream, "############# Start of table offset log  #############\n");
        fprintf(stdout, "\n");
        fprintf(stdout, "%s\n", FormatTableOffset(offset->Offsets[c]));
        fprintf(stdout, "\n");
        fprintf(stream, "############# End of table offset log    #############\n");
    }

    fclose(stream);
    return buffer;
}

Offset *NewOffset(int imwebOffset, ...)
{
    va_list args;
    Offset *offset;
    TableOffset *curr, *allOffsets[MAX_TABLE_COUNT];
    int count = 0;

    va_start(args, imwebOffset);
    while ((curr = va_arg(args, TableOffset *)) != NULL)
    {
        allOffsets[count++] = curr;
    }
    va_end(args);

    offset = malloc(sizeof(Offset));
    if (!offset)
    {
        fprintf(stderr, "(new-offset-err) malloc failed. \n");
        return NULL;
    }

    offset->Offsets = malloc(sizeof(TableOffset *) * count);
    if (!offset->Offsets)
    {
        fprintf(stderr, "(new-offset-err) malloc failed. \n");
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        offset->Offsets[c] = allOffsets[c];
    }

    offset->TableCount = count;
    offset->ImwebOffset = imwebOffset;

    return offset;
}

Offset *NewOffsetN(int imwebOffset, const TableOffset **offsets)
{
    Offset *offset;
    int count;

    if (!offsets)
    {
        fprintf(stderr, "(new-offset-n-err) offsets is NULL. \n");
        return NULL;
    }

    count = Count(offsets);

    offset = malloc(sizeof(Offset));
    if (!offset)
    {
        fprintf(stderr, "(new-offset-n-err) malloc failed. \n");
        return NULL;
    }

    offset->Offsets = malloc(sizeof(TableOffset *) * count);
    if (!offset->Offsets)
    {
        free(offset);
        fprintf(stderr, "(new-offset-n-err) malloc failed. \n");
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        offset->Offsets[c] = offsets[c];

        // As long as we use Count() we're guranteed this won't hit.
        // Just incase the implementation fails.
        if (!offset->Offsets[c])
        {
            for (int c2 = 0; c2 < c; c2++)
            {
                free(offset->Offsets[c2]);
            }
            free(offset->Offsets);
            free(offset);
            fprintf(stderr, "(new-offset-n-err) one of the offset in arguments is a null value. \n");
            return NULL;
        }
    }

    offset->TableCount = count;
    offset->ImwebOffset = imwebOffset;

    return offset;
}

int AddTableOffset(Offset *offset, const TableOffset *table)
{
    TableOffset **replace;

    if (!offset || !table)
    {
        fprintf(stderr, "(add-offset-err) offset or table is NULL. \n");
        return -1;
    }

    replace = malloc(sizeof(TableOffset *) * offset->TableCount + 1);
    if (!replace)
    {
        fprintf(stderr, "(add-offset-err) malloc failed. \n");
        return -1;
    }

    if (offset->Offsets)
    {
        for (int c = 0; c < offset->TableCount; c++)
        {
            replace[c] = offset->Offsets[c];
        }
    }

    replace[offset->TableCount] = table;
    free(offset->Offsets);
    offset->Offsets = replace;
    offset->TableCount++;

    return 0;
}

void FreeOffset(Offset *offset)
{
    if (offset)
    {
        free(offset->Offsets);
        free(offset);
    }
}
