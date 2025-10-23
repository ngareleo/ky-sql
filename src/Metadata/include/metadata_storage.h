#ifndef METADATA_STORAGE_H
#define METADATA_STORAGE_H

#include "metadata_schema.h"

typedef struct
{
    int RowSize; // The size of a single row
    int Count;   // The number of items in a table
    int TableId; // The table id
} StorageMetaItem;

typedef struct
{
    StorageMetaItem **Items; // Storage items
} StorageMeta;

void FreeStorageMeta(StorageMeta *);
StorageMeta *NewStorageMeta(SchemaDefinition *);
size_t CalculateRowSize(TableDefinition *);

#endif
