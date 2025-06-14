#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "metadata_offsets.h"
#include "../Utilities/utilities.h"

TableOffset *NewTableOffset(const char *tableName, int offset)
{
    TableOffset *tableOffset;

    if (!tableName)
    {
        fprintf(stderr, "(new-table-offset-err) tablename is NULL.\n");
        return NULL;
    }

    tableOffset = malloc(sizeof(TableOffset));
    if (!tableOffset)
    {
        fprintf(stderr, "(new-table-offset-err) malloc failed.\n");
        return NULL;
    }

    tableOffset->Offset = offset;

    tableOffset->TableName = malloc(strlen(tableName) + 1);
    if (!tableOffset->TableName)
    {
        fprintf(stderr, "(new-table-offset-err) malloc failed.\n");
        free(tableOffset);
        return NULL;
    }

    strcpy(tableOffset->TableName, tableName);

    return tableOffset;
}

void FreeTableOffset(TableOffset *offset)
{
    if (offset)
    {
        free(offset->TableName);
        free(offset);
    }
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

    sprintf(stream, "(log) table-name = %s.\n", tableOffset->TableName);
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
    sprintf(stream, "(log) offset-count                  = %d.\n", offset->OffsetCount);

    for (int c = 0; c < offset->OffsetCount; c++)
    {
        sprintf(stream, "(log) table-offset[%d] = %s\n", c, FormatTableOffset(offset->Offsets[c]));
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

    offset->OffsetCount = count;
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

    offset->OffsetCount = count;
    offset->ImwebOffset = imwebOffset;

    return offset;
}

int AddTableOffset(Offset *offset, const TableOffset *table)
{
    TableOffset **newOff;

    if (!offset || !table)
    {
        fprintf(stderr, "(add-offset-err) offset or table is NULL. \n");
        return -1;
    }

    newOff = malloc(sizeof(TableOffset *) * offset->OffsetCount + 1);
    if (!newOff)
    {
        fprintf(stderr, "(add-offset-err) malloc failed. \n");
        return -1;
    }

    if (offset->Offsets)
    {
        for (int c = 0; c < offset->OffsetCount; c++)
        {
            newOff[c] = offset->Offsets[c];
        }
    }

    newOff[offset->OffsetCount] = table;
    free(offset->Offsets);
    offset->Offsets = newOff;
    offset->OffsetCount++;

    return 0;
}

void FreeOffset(Offset *offset)
{
    if (offset)
    {
        for (int c = 0; c < offset->OffsetCount; c++)
        {
            FreeTableOffset(offset->Offsets[c]);
        }
        free(offset->Offsets);
        free(offset);
    }
}
