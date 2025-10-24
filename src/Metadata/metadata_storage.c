#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/metadata_storage.h"
#include "Metadata/metadata.h"
#include "Utilities/utilities.h"
#include "Writer/writer.h"

size_t **GetTableColSizes(TableDefinition *def)
{
    if (!def)
    {
        fprintf(stderr, "(calculate-row-size) Args are invalid \n");
        return -1;
    }

    size_t **sizes;
    sizes = malloc(sizeof(size_t *) * def->ColumnCount + 1);
    sizes[def->ColumnCount] = NULL;

    for (int cidx = 0; cidx < def->ColumnCount; cidx++)
    {
        *sizes[cidx] = GetDataTypeSize(def->Columns[cidx]->Type);
    }

    return sizes;
}

size_t CalculateRowSize(TableDefinition *def)
{
    if (!def)
    {
        fprintf(stderr, "(calculate-row-size) Args are invalid \n");
        return -1;
    }

    size_t totalS = 0;
    for (int rIdx = 0; rIdx < def->ColumnCount; rIdx++)
    {
        totalS += GetDataTypeSize(def->Columns[rIdx]->Type);
    }

    return totalS;
}

StorageMeta *NewStorageMeta(SchemaDefinition *def)
{
    StorageMeta *meta;
    Allocator *alloc = MallocInit();

    meta = Malloc(sizeof(StorageMeta), alloc);
    if (meta)
    {
        meta->Items = Malloc(sizeof(StorageMetaItem) * (def->TableCount + 1), alloc);
        meta->Items[def->TableCount] = NULL;

        for (int tIdx = 0; tIdx < def->TableCount; tIdx++)
        {
            for (int cidx = 0; cidx < def->TableDefs[tIdx]->ColumnCount; cidx++)
            {
                meta->Items[tIdx]->ColInfo = Malloc(sizeof(ColumnStorage *) * (def->TableDefs[tIdx]->ColumnCount + 1), alloc);
                meta->Items[tIdx]->ColInfo[cidx] = Malloc(sizeof(ColumnStorage), alloc);
                meta->Items[tIdx]->ColInfo[def->TableDefs[tIdx]->ColumnCount] = NULL;
            }
        }
    }

    if (!VerifyAlloc(alloc))
    {
        for (int tIdx = 0; tIdx < def->TableCount; tIdx++)
        {
            for (int cidx = 0; cidx < def->TableDefs[tIdx]->ColumnCount; cidx++)
            {
                free(meta->Items[tIdx]->ColInfo);
                free(meta->Items[tIdx]->ColInfo[cidx]);
            }
        }

        free(meta->Items);
        free(meta);
    }

    for (int idx = 0; idx < def->TableCount; idx++)
    {
        size_t **colSizes = GetTableColSizes(def->TableDefs[idx]);
        size_t culm = 0;

        for (int cidx = 0; cidx < def->TableDefs[idx]->ColumnCount; cidx++)
        {
            culm += *colSizes[cidx];
            meta->Items[idx]->ColInfo[cidx]->Id = def->TableDefs[idx]->Columns[cidx];
            meta->Items[idx]->ColInfo[cidx]->SequencePos = cidx;
            meta->Items[idx]->ColInfo[cidx]->Padding = culm;
        }

        meta->Items[idx]->Count = 0;
        meta->Items[idx]->TableId = def->TableDefs[idx]->Id;
        meta->Items[idx]->RowSize = CalculateRowSize(def->TableDefs[idx]);
    }

    return meta;
}

void FreeStorageMeta(StorageMeta *meta)
{
    if (meta)
    {
        free(meta->Items);
        free(meta);
    }
}