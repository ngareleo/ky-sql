#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_offsets.h"

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

char *FormatTableOffset(const struct TableOffset const *tableOffset)
{
    char *str = malloc(wBytes);
    if (str == NULL)
    {
        return NULL;
    }
    sprintf(str, "<TableOffset> Tablename: %s, Offset: %d\n", tableOffset->TableName, tableOffset->Offset);
    return str;
}

struct Offset *NewOffset(struct TableOffset **offsets, int imwebOffset)
{
    struct Offset *offset;
    offset = malloc(sizeof(struct Offset));

    if (offset == NULL)
    {
        return NULL;
    }

    offset->Offsets = offsets;
    offset->ImwebOffset = imwebOffset;

    return offset;
}

void FreeOffset(struct Offset *offset, int n)
{

    if (offset == NULL)
    {
        return;
    }

    for (int c = 0; c < n; c++)
    {
        FreeTableOffset(offset->Offsets[c]);
    }

    free(offset);
}
