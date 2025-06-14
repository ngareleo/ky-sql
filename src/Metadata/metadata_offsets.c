#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "metadata_offsets.h"
#include "../Utilities/utilities.h"

const size_t wBytes = 1000;

TableOffset *NewTableOffset(const char *tableName, int offset)
{
    TableOffset *tableOffset;

    tableOffset = malloc(sizeof(TableOffset));
    if (!tableOffset)
    {
        return NULL;
    }
    tableOffset->Offset = offset;

    tableOffset->TableName = malloc(strlen(tableName) + 1);
    if (!tableOffset->TableName)
    {
        free(tableOffset);
        return NULL;
    }
    strcpy(tableOffset->TableName, tableName);

    return tableOffset;
}

void FreeTableOffset(TableOffset *offset)
{
    if (!offset)
    {
        return;
    }

    free(offset->TableName);
    free(offset);
}

char *FormatTableOffset(const TableOffset *tableOffset)
{
    char *str = malloc(wBytes);
    if (!str)
    {
        return NULL;
    }
    sprintf(str, "<TableOffset> Tablename: %s, Offset: %d\n", tableOffset->TableName, tableOffset->Offset);
    return str;
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
        fprintf(stderr, "[NewOffset] Error creating Offset. malloc failed. \n");
        return NULL;
    }

    offset->Offsets = malloc(sizeof(TableOffset *) * count);
    if (!offset->Offsets)
    {
        fprintf(stderr, "[NewOffset] Error creating Offset. malloc failed. \n");
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
        fprintf(stderr, "[NewOffsetN] Error creating Offset. offsets is NULL. \n");
        return NULL;
    }

    count = Count(offsets);

    offset = malloc(sizeof(Offset));
    if (!offset)
    {
        fprintf(stderr, "[NewOffsetN] Error creating Offset. malloc failed. \n");
        return NULL;
    }

    offset->Offsets = malloc(sizeof(TableOffset *) * count);
    if (!offset->Offsets)
    {
        free(offset);
        fprintf(stderr, "[NewOffsetN] Error creating Offset. malloc failed. \n");
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
            fprintf(stderr, "[NewOffsetN] Error creating Offset. One of the offset provided is null. \n");
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
        fprintf(stderr, "[AddOffset] Error adding Offset. Offset or table is NULL. \n");
        return -1;
    }

    newOff = malloc(sizeof(TableOffset *) * offset->OffsetCount + 1);
    if (!newOff)
    {
        fprintf(stderr, "[AddOffset] Error adding Offset. malloc failed. \n");
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
