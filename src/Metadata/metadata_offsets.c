#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "metadata_offsets.h"
#include "../Utilities/utilities.h"

const size_t wBytes = 1000;

struct TableOffset *NewTableOffset(const char *tableName, int offset)
{
    struct TableOffset *tableOffset;
    tableOffset = malloc(sizeof(struct TableOffset));

    if (tableOffset == NULL)
    {
        return NULL;
    }

    tableOffset->Offset = offset;
    tableOffset->TableName = malloc(strlen(tableName) + 1);
    if (tableOffset->TableName == NULL)
    {
        free(tableOffset);
        return NULL;
    }
    strcpy(tableOffset->TableName, tableName);

    return tableOffset;
}

void FreeTableOffset(struct TableOffset *offset)
{
    if (!offset)
    {
        return;
    }

    free(offset->TableName);
    free(offset);
}

char *FormatTableOffset(const struct TableOffset *tableOffset)
{
    char *str = malloc(wBytes);
    if (str == NULL)
    {
        return NULL;
    }
    sprintf(str, "<TableOffset> Tablename: %s, Offset: %d\n", tableOffset->TableName, tableOffset->Offset);
    return str;
}

struct Offset *NewOffset(int imwebOffset, ...)
{
    va_list args;
    struct Offset *offset;
    struct TableOffset *curr, *allOffsets[MAX_TABLE_COUNT];
    int count = 0;

    va_start(args, imwebOffset);
    while ((curr = va_arg(args, struct TableOffset *)) != NULL)
    {
        allOffsets[count++] = curr;
    }
    va_end(args);

    offset = malloc(sizeof(struct Offset));
    if (offset == NULL)
    {
        fprintf(stderr, "[NewOffset] Error creating Offset. malloc failed. \n");
        return NULL;
    }

    offset->Offsets = malloc(sizeof(struct TableOffset *) * count);
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

struct Offset *NewOffsetN(int imwebOffset, const struct TableOffset **offsets)
{
    va_list args;
    struct Offset *offset;
    struct TableOffset *curr, *allOffsets[MAX_TABLE_COUNT];
    int count;

    if (!offsets)
    {
        fprintf(stderr, "[NewOffsetN] Error creating Offset. offsets is NULL. \n");
        return NULL;
    }

    offset = malloc(sizeof(struct Offset));
    if (offset == NULL)
    {
        fprintf(stderr, "[NewOffsetN] Error creating Offset. malloc failed. \n");
        return NULL;
    }

    count = Count(offsets);
    offset->Offsets = malloc(sizeof(struct TableOffset *) * count);
    if (!offset->Offsets)
    {
        fprintf(stderr, "[NewOffsetN] Error creating Offset. malloc failed. \n");
        free(offset);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        offset->Offsets[c] = allOffsets[c];

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

int AddTableOffset(struct Offset *offset, const struct TableOffset *table)
{
    struct TableOffset **newOff;

    if (!offset || !table)
    {
        fprintf(stderr, "[AddOffset] Error adding Offset. Offset or table is NULL. \n");
        return -1;
    }

    newOff = malloc(sizeof(struct TableOffset *) * offset->OffsetCount + 1);
    if (!newOff)
    {
        fprintf(stderr, "[AddOffset] Error adding Offset. malloc failed. \n");
        return -1;
    }

    if (!offset->Offsets)
    {
        newOff[0] = table;
        offset->Offsets = newOff;
        return 0;
    }

    for (int c = 0; c < offset->OffsetCount; c++)
    {
        newOff[c] = offset->Offsets[c];
    }
    newOff[offset->OffsetCount] = table;

    free(offset->Offsets);
    offset->Offsets = newOff;
    offset->OffsetCount++;

    return 0;
}

void FreeOffset(struct Offset *offset)
{

    if (offset == NULL)
    {
        return;
    }

    for (int c = 0; c < offset->OffsetCount; c++)
    {
        FreeTableOffset(offset->Offsets[c]);
    }

    free(offset->Offsets);
    free(offset);
}
