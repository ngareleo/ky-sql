#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "include/metadata_extraction.h"
#include "include/metadata_offsets.h"
#include "include/metadata_default.h"
#include "include/metadata_storage.h"
#include "Utilities/utilities.h"
#include "Writer/writer.h"

FileMetadata *CreateNewFileMetadataFromSchema(SchemaDefinition *schema)
{
    FileMetadata *meta;
    Offset *offset;
    StorageMeta *storage;

    if (!schema)
    {
        fprintf(stderr, "(create-new-file-metadata-err) could not create schema\n");
        return NULL;
    }

    offset = NewOffset(0); // A default imweb of 0
    if (!offset)
    {
        fprintf(stderr, "(create-new-file-metadata-err) could not create offset\n");
        return NULL;
    }

    fprintf(stdout, "(create-new-file-metadata-info) offset creation success\n");
    fprintf(stderr, "(create-new-file-metadata-log) schema table count --> %d \n", schema->TableCount);
    for (int ti = 0; ti < schema->TableCount; ti++)
    {

        // Todo: Optimize this. AddTableOffset is not suitable for batch updates
        // Todo: Since we know the table count ahead of time, we don't have to link a table
        // Todo: One by one, we can do one allocation
        TableOffset *tableOff = NewTableOffset(
            schema->TableDefs[ti]->Id,
            0 // Todo: Compute this value based on real offsets
        );
        if (AddTableOffset(offset, tableOff) != 0)
        {
            FreeOffset(offset);
            fprintf(stderr, "(create-new-file-metadata-err) could not add table offset\n");
            return NULL;
        }
    }
    fprintf(stdout, "(create-new-file-metadata-info) table offsets linked\n");

    storage = NewStorageMeta(schema);
    meta = NewFileMetadata(offset, schema, storage);
    if (!meta)
    {
        FreeOffset(offset);
        fprintf(stderr, "(create-new-file-metadata-err) could not create metadata\n");
        return NULL;
    }

    return meta;
}

int ReadSchemaFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    if (!file || !meta)
    {
        fprintf(stderr, "(read-schema-from-writable) passed a null value. \n");
        return -1;
    }

    Allocator *alloc = MallocInit();

    meta->Schema = Malloc(sizeof(SchemaDefinition), alloc);
    meta->Schema->TagName = Malloc(strlen(file->Schema.SchemaTag) + 1, alloc);
    meta->Schema->TableDefs = Malloc(sizeof(TableDefinition *) * file->Schema.TableCount, alloc);

    if (meta)
    {
        for (int tIdx = 0; tIdx < file->Schema.TableCount; tIdx++)
        {
            TableDefinition *tableDef;
            tableDef = Malloc(sizeof(TableDefinition), alloc);
            tableDef->Name = Malloc(strlen(file->Schema.TableDefs[tIdx].TableName) + 1, alloc);
            tableDef->Columns = Malloc(sizeof(TableColDefinition) * file->Schema.TableDefs[tIdx].TableColumnCount, alloc);

            for (int cIdx = 0; cIdx < file->Schema.TableDefs[tIdx].TableColumnCount; cIdx++)
            {
                TableColDefinition *colDef;
                colDef = Malloc(sizeof(TableColDefinition), alloc);
                colDef->Name = Malloc(strlen(file->Schema.TableDefs[tIdx].TableColumnDefs[cIdx].ColumnName) + 1, alloc);
                if (strlen(file->Schema.TableDefs[tIdx].TableColumnDefs[cIdx].ColumnDefaultValue) > 0)
                {
                    colDef->DefaultValue = Malloc(strlen(file->Schema.TableDefs[tIdx].TableColumnDefs[cIdx].ColumnDefaultValue) + 1, alloc);
                }

                meta->Schema->TableDefs[tIdx]->Columns[cIdx] = colDef;
            }

            meta->Schema->TableDefs[tIdx] = tableDef;
        }
    }

    if (!VerifyAlloc(alloc))
    {
        if (meta)
        {
            fprintf(stderr, "(read-schema-from-writable) Malloc failed. \n");
            for (int tIdx = 0; tIdx < file->Schema.TableCount; tIdx++)
            {
                for (int cIdx = 0; cIdx < file->Schema.TableDefs[tIdx].TableColumnCount; cIdx++)
                {
                    free(meta->Schema->TableDefs[tIdx]->Columns[cIdx]->DefaultValue);
                    free(meta->Schema->TableDefs[tIdx]->Columns[cIdx]->Name);
                    free(meta->Schema->TableDefs[tIdx]->Columns[cIdx]);
                }

                free(meta->Schema->TableDefs[tIdx]->Name);
                free(meta->Schema->TableDefs[tIdx]->Columns);
                free(meta->Schema->TableDefs[tIdx]);
            }

            free(meta->Schema->TagName);
            free(meta->Schema->TableDefs);
            free(meta);
        }
        return -1;
    }

    for (int ti = 0; ti < file->Schema.TableCount; ti++)
    {
        for (int ci = 0; ci < file->Schema.TableDefs[ti].TableColumnCount; ci++)
        {
            // !! For now we treat default values as character array
            // !! The way we handle default values will be different for each data type
            if (strlen(file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue) > 0)
            {
                if (file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue)
                {
                    strcpy(meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue, &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue);
                }
            }

            strcpy(meta->Schema->TableDefs[ti]->Columns[ci]->Name, &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName);
            meta->Schema->TableDefs[ti]->Columns[ci]->CreatedAt = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt;
            meta->Schema->TableDefs[ti]->Columns[ci]->Id = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId;
            meta->Schema->TableDefs[ti]->Columns[ci]->IsUnique = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique;
            meta->Schema->TableDefs[ti]->Columns[ci]->IsNullable = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable;
            meta->Schema->TableDefs[ti]->Columns[ci]->IsPrimaryKey = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsPrimaryKey;
            meta->Schema->TableDefs[ti]->Columns[ci]->LastModified = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified;
            meta->Schema->TableDefs[ti]->Columns[ci]->Type = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType;
            fprintf(stderr, "(read-schema-from-writable-info) 2 table column copied over \n");
        }
        meta->Schema->TableDefs[ti]->PrimaryKeyId = file->Schema.TableDefs[ti].TablePrimaryKey;
        meta->Schema->TableDefs[ti]->Id = file->Schema.TableDefs[ti].TableId;
        meta->Schema->TableDefs[ti]->ColumnCount = file->Schema.TableDefs[ti].TableColumnCount;
        meta->Schema->TableDefs[ti]->CreatedAt = file->Schema.TableDefs[ti].TableCreatedAt;
        meta->Schema->TableDefs[ti]->LastModified = file->Schema.TableDefs[ti].TableLastModified;
        strcpy(meta->Schema->TableDefs[ti]->Name, &file->Schema.TableDefs[ti].TableName);
        fprintf(stderr, "(read-schema-from-writable-info) 1 table  copied over \n");
    }
    meta->Schema->TableCount = file->Schema.TableCount;
    meta->Schema->CreatedAt = file->FileCreatedAt;
    meta->Schema->LastModified = file->FileLastModified;
    strcpy(meta->Schema->TagName, &file->Schema.SchemaTag);

    FreeAlloc(alloc);

    return 0;
}

int ReadOffsetFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    if (!meta || !file)
    {
        fprintf(stderr, "(read-offset-from-writable) file or meta is null. \n");
        return -1;
    }

    TableOffset **offsets;
    Allocator *alloc = MallocInit();

    meta->Offset = Malloc(sizeof(TableOffset) * meta->Schema->TableCount, alloc);

    if (!VerifyAlloc(alloc))
    {
        free(meta->Offset);
        fprintf(stderr, "(read-offset-from-writable) malloc failed \n");
        return -1;
    }

    for (int tIdx = 0; tIdx < file->Schema.TableCount; tIdx++)
    {
        meta->Offset->Offsets[tIdx] = NewTableOffset(file->TableOffsets[tIdx].TableOffsetId, file->TableOffsets[tIdx].TableOffset);
    }

    meta->Offset->Offsets = offsets;
    meta->Offset->ImwebOffset = file->OffsetImweb;
    meta->Offset->TableCount = file->Schema.TableCount;

    FreeAlloc(alloc);
    return 0;
}

int ReadStorageFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    if (!meta || !file)
    {
        fprintf(stderr, "(read-storage-from-writable) file or meta is null. \n");
        return -1;
    }

    Allocator *alloc = MallocInit();

    meta->Storage = Malloc(sizeof(StorageMeta), alloc);
    meta->Storage->Items = Malloc(sizeof(StorageMetaItem) * (file->Schema.TableCount + 1), alloc);
    for (int idx = 0; idx < file->Schema.TableCount; idx++)
    {
        int colCount = file->Schema.TableDefs[idx].TableColumnCount;
        meta->Storage->Items[idx] = Malloc(sizeof(StorageMetaItem), alloc);
        for (int cidx = 0; cidx < colCount; cidx++)
        {
            meta->Storage->Items[idx]->ColInfo = Malloc(sizeof(ColumnStorage *) * (colCount + 1), alloc);
        }
    }

    if (!VerifyAlloc(alloc))
    {
        free(meta->Storage);
        for (int idx = 0; idx < file->Schema.TableCount; idx++)
        {
            for (int cidx = 0; cidx < file->Schema.TableDefs[idx].TableColumnCount; cidx++)
            {
                free(meta->Storage->Items[idx]->ColInfo);
            }
            free(meta->Storage->Items[idx]);
            free(meta->Storage->Items);
        }
    }

    for (int tIdx = 0; tIdx < file->Schema.TableCount; tIdx++)
    {
        meta->Storage->Items[tIdx]->RowSize = file->Storage[tIdx].RowSize;
        meta->Storage->Items[tIdx]->TableId = file->Storage[tIdx].TableId;
        meta->Storage->Items[tIdx]->Count = file->Storage[tIdx].Count;

        for (int cidx = 0; cidx < file->Schema.TableDefs[tIdx].TableColumnCount; cidx++)
        {
            meta->Storage->Items[tIdx]->ColInfo[cidx]->Id = file->Storage[tIdx].ColInfo[cidx].Id;
            meta->Storage->Items[tIdx]->ColInfo[cidx]->Padding = file->Storage[tIdx].ColInfo[cidx].Padding;
            meta->Storage->Items[tIdx]->ColInfo[cidx]->SequencePos = file->Storage[tIdx].ColInfo[cidx].SequencePos;
        }
    }

    FreeAlloc(alloc);
    return 0;
}

FileMetadata *CreateMetadataFromWritable(const WritableFileMetadata *metadata)
{
    FileMetadata *target;
    if (!metadata)
    {
        fprintf(stderr, "(create-metadata-from-info) metadata is null. \n");
        return NULL;
    }

    target = malloc(sizeof(FileMetadata));
    if (!target)
    {
        fprintf(stderr, "(create-metadata-from-writable) malloc failed. \n");
        return NULL;
    }

    if (ReadOffsetFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(create-metadata-from-writable) failed while copying over offsets. \n");
        return NULL;
    }
    fprintf(stdout, "(create-metadata-from-writable) offsets copied to metadata . \n");

    if (ReadSchemaFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-from-persisted-format-err) failed while copying over schema. \n");
        return NULL;
    }
    fprintf(stdout, "(create-metadata-from-writable) schema copied to metadata . \n");

    if (ReadStorageFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-from-persisted-format-err) failed while copying over storage info. \n");
        return NULL;
    }
    fprintf(stdout, "(create-metadata-from-writable) storage copied to metadata . \n");

    target->CreatedAt = metadata->FileCreatedAt;
    target->LastModified = metadata->FileLastModified;

    IntrospectMetadata(target);
    return target;
}

int CreateWritableFromMetadata(const FileMetadata *metadata, WritableFileMetadata *target)
{
    if (!metadata)
    {
        fprintf(stderr, "(create-writable-from-metadata) metadata is null \n");
        return -1;
    }

    if (!target)
    {
        fprintf(stderr, "(create-writable-from-metadata) target is not pre-allocated \n");
        return -1;
    }

    if (WriteOffset(target, metadata) != 0)
    {
        fprintf(stderr, "(create-writable-from-metadata) failed to write offset \n");
        return -1;
    }
    fprintf(stdout, "(create-writable-from-metadata) offsets copied to writable \n");

    if (WriteSchema(target, metadata) != 0)
    {
        fprintf(stderr, "(create-writable-from-metadata) failed to write schema \n");
        return -1;
    }
    fprintf(stdout, "(create-writable-from-metadata) schema written to writable metadata \n");

    if (WriteStorage(target, metadata) != 0)
    {
        fprintf(stderr, "(create-writable-from-metadata) failed to write storage \n");
        return -1;
    }
    fprintf(stdout, "(create-writable-from-metadata) storage written to writable metadata \n");

    target->FileCreatedAt = metadata->CreatedAt;
    target->FileLastModified = metadata->LastModified;
    target->OffsetImweb = metadata->Offset->ImwebOffset;
    return 0;
}

int WriteOffset(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-offset-err) meta is null\n");
        return -1;
    }

    file->OffsetImweb = meta->Offset->ImwebOffset;

    /**
     * !! Here lies Dragons !!
     *
     * Here I'm setting a hard rule that number of table offsets are always equal to the number of tables.
     * Although they're represented by different structs
     * There's no gurantee this holds in instructions upstream
     *
     * Validating seems like a waste for now so I'll keep it this way
     */
    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        file->TableOffsets[ti].TableOffset = meta->Offset->Offsets[ti]->Offset;
        file->TableOffsets[ti].TableOffsetId = meta->Offset->Offsets[ti]->Id;
    }

    return 0;
}

int WriteStorage(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-offset-err) meta is null\n");
        return -1;
    }

    for (int tIdx = 0; tIdx < meta->Schema->TableCount; tIdx++)
    {
        for (int cIdx = 0; cIdx < meta->Schema->TableDefs[tIdx]->ColumnCount; cIdx++)
        {
            file->Storage[tIdx].ColInfo[cIdx].Id = meta->Storage->Items[tIdx]->ColInfo[cIdx]->Id;
            file->Storage[tIdx].ColInfo[cIdx].Padding = meta->Storage->Items[tIdx]->ColInfo[cIdx]->Padding;
            file->Storage[tIdx].ColInfo[cIdx].SequencePos = meta->Storage->Items[tIdx]->ColInfo[cIdx]->SequencePos;
        }
        file->Storage[tIdx].Count = meta->Storage->Items[tIdx]->Count;
        file->Storage[tIdx].RowSize = meta->Storage->Items[tIdx]->RowSize;
        file->Storage[tIdx].TableId = meta->Storage->Items[tIdx]->TableId;
    }

    return 0;
}

int WriteSchema(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-schema-err) meta is null \n");
        return -1;
    }

    fprintf(stderr, "(write-schema-log) starting schema copy \n");
    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        fprintf(stderr, "(write-schema-log) starting table copy \n");
        for (int ci = 0; ci < meta->Schema->TableDefs[ti]->ColumnCount; ci++)
        {
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId = meta->Schema->TableDefs[ti]->Columns[ci]->Id;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt = meta->Schema->TableDefs[ti]->Columns[ci]->CreatedAt;
            strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName, meta->Schema->TableDefs[ti]->Columns[ci]->Name);
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable = meta->Schema->TableDefs[ti]->Columns[ci]->IsNullable;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsPrimaryKey = meta->Schema->TableDefs[ti]->Columns[ci]->IsPrimaryKey;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique = meta->Schema->TableDefs[ti]->Columns[ci]->IsUnique;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified = meta->Schema->TableDefs[ti]->Columns[ci]->LastModified;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType = meta->Schema->TableDefs[ti]->Columns[ci]->Type;
            if (meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue)
            {
                strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue, meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue);
            }
        }
        fprintf(stderr, "(write-schema-log) table columns written over \n");
        file->Schema.TableDefs[ti].TableColumnCount = meta->Schema->TableDefs[ti]->ColumnCount;
        file->Schema.TableDefs[ti].TableCreatedAt = meta->Schema->TableDefs[ti]->CreatedAt;
        file->Schema.TableDefs[ti].TableId = meta->Schema->TableDefs[ti]->Id;
        file->Schema.TableDefs[ti].TablePrimaryKey = meta->Schema->TableDefs[ti]->PrimaryKeyId;
        file->Schema.TableDefs[ti].TableLastModified = meta->Schema->TableDefs[ti]->LastModified;
        strcpy(&file->Schema.TableDefs[ti].TableName, meta->Schema->TableDefs[ti]->Name);
        fprintf(stderr, "(write-schema-log) table written over \n");
    }

    strcpy(&file->Schema.SchemaTag, meta->Schema->TagName);
    file->Schema.TableCount = meta->Schema->TableCount;

    return 0;
}

FileMetadata *NewFileMetadata(Offset *offset, SchemaDefinition *schema, StorageMeta *storage)
{
    FileMetadata *metadata;
    time_t now;

    if (!offset || !schema || !storage)
    {
        fprintf(stderr, "(new-file-metadata-err) offset or schema is null \n");
        return NULL;
    }

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(new-file-metadata-err) now failed \n");
        return NULL;
    }

    metadata = malloc(sizeof(FileMetadata));
    if (!metadata)
    {
        fprintf(stderr, "(new-file-metadata-err) malloc failed \n");
        return NULL;
    }

    metadata->Offset = offset;
    metadata->CreatedAt = now;
    metadata->LastModified = now;
    metadata->Schema = schema;
    metadata->Storage = storage;
    fprintf(stderr, "(new-file-metadata-info) file metadata creation success \n");
    return metadata;
}

void FreeFileMetadata(FileMetadata *metadata)
{
    if (metadata)
    {
        FreeOffset(metadata->Offset);
        free(metadata);
    }
}

void IntrospectMetadata(const FileMetadata *metadata)
{
    if (!metadata)
    {
        fprintf(stderr, "(introspect-metadata-err) metadata is NULL.\n");
        return;
    }

    fprintf(stdout, "############# Start of metadata log  #############\n");
    fprintf(stdout, "############# Start of schema log  #############\n");
    fprintf(stdout, "%s\n", NullGuardStr(FormatSchemaDefinition(metadata->Schema)));
    fprintf(stdout, "############# End of schema log    #############\n");
    fprintf(stdout, "(log) created-at                      = %ld.\n", metadata->CreatedAt);
    fprintf(stdout, "(log) last-modified                   = %ld.\n", metadata->LastModified);
    fprintf(stdout, "\n");
    fprintf(stdout, "############# Start of offsets log #############\n");
    fprintf(stdout, "%s\n", NullGuardStr(FormatOffset(metadata->Offset)));
    fprintf(stdout, "############# End of offsets log   #############\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "############# End of metadata log  #############\n");
}

int WriteMetadataToFile(FILE *file, FileMetadata *in, int map(const FileMetadata *metadata, WritableFileMetadata *writable))
{
    int status = -1;
    WritableFileMetadata target;

    if (!map || !file || !in)
    {
        fprintf(stderr, "(write-metadata-to-file-err) Either `map`, `file`, or `in` could be null \n");
        return status;
    }

    do
    {
        status = map(in, &target);
        if (status != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) mapping to persisted format failed \n");
            break;
        }

        if (fwrite(&target, sizeof(WritableFileMetadata), 1, file) < 1)
        {
            perror("fwrite");
            fprintf(stderr, "(write-metadata-to-file-err) fwrite failed \n");
            break;
        }
        else if (feof(file) != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) EOF reached \n");
            break;
        }

        status = 0;
    } while (0);
    return status;
}

int ReadMetadataFromFile(FILE *file, FileMetadata **out, FileMetadata *map(const WritableFileMetadata *metadata))
{
    WritableFileMetadata *writable;
    int status = -1;

    if (!map || !out || !file)
    {
        fprintf(stderr, "(read-metadata-from-file-err) Either `map`, `file`, or `out` could be null \n");
        return -1;
    }

    do
    {
        writable = malloc(sizeof(WritableFileMetadata));
        if (!writable)
        {
            fprintf(stderr, "(read-metadata-from-file-err) malloc failed \n");
            break;
        }

        int read = fread(writable, sizeof(WritableFileMetadata), 1, file);
        if (read < 1)
        {
            fprintf(stderr, "(read-metadata-from-file-err) fread failed \n");
        }
        else if (!writable)
        {
            fprintf(stderr, "(read-metadata-from-file-err) fread failed. writable is null \n");
            break;
        }
        fprintf(stdout, "(read-metadata-from-file-info) data read from file \n");

        *out = map(writable);
        if (!out)
        {
            fprintf(stderr, "(read-metadata-from-file-err) mapping from persisted format failed \n");
        }
        fprintf(stdout, "(read-metadata-from-file-info) metadata read successfully  \n");

        status = 0;
    } while (0);

    free(writable);
    return status;
}