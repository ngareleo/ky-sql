#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/metadata_storage.h"
#include "Metadata/metadata.h"
#include "Utilities/utilities.h"

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
    }

    if (!VerifyAlloc(alloc))
    {
        free(meta->Items);
        free(meta);
    }

    for (int idx = 0; idx < def->TableCount; idx++)
    {
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